#include "particles_isosurfacetortuosity.h"
#include "isosurface.h"
#include "export_STL.h"
#include "export_3DTiff.h"


puma::TortuosityReturn puma::compute_particle_isosurface_Tortuosity(puma::Workspace *grayWS, puma::Cutoff cutoff,
                                                                    int numParticles, double meanFreePath, double meanVelocity,
                                                                    int randomSeed, double totalLength, int numThreads) {
    if (cutoff.first < -0.5 || cutoff.second > 32767.5 || cutoff.first > cutoff.second) {
        grayWS->printer->print("DiffusivityError: Invalid Cutoff Ranges");
        puma::TortuosityReturn returnVals;
        returnVals.diffusionCoefficient = puma::Vec3<double>(-1.,-1.,-1.);
        returnVals.tortuosity = puma::Vec3<double>(-1.,-1.,-1.);
        returnVals.diffusivity = puma::Vec3<double>(-1.,-1.,-1.);
        returnVals.meanInterceptLength = -1.;
        return returnVals;
    }

    TortuosityParticles_Isosurface solver(grayWS,cutoff,numParticles,meanFreePath,meanVelocity,randomSeed,totalLength,numThreads);
    return solver.compute();

}


TortuosityParticles_Isosurface::TortuosityParticles_Isosurface(puma::Workspace *grayWS, puma::Cutoff cutoff, int numParticles, double meanFreePath,
                                                               double meanVelocity, int randomSeed, double totalLength, int numThreads){

    this->grayWS = grayWS;
    this->cutoff = cutoff;
    this->numParticles=numParticles;
    this->meanFreePath=meanFreePath;
    this->meanVelocity=meanVelocity;
    this->numThreads=numThreads;
    this->randomSeed=randomSeed*10000+1;
    this->totalLength=totalLength;
    this->voxelLength = grayWS->voxelLength;

    X = (int)grayWS->X();
    Y = (int)grayWS->Y();
    Z = (int)grayWS->Z();

}


puma::TortuosityReturn TortuosityParticles_Isosurface::compute(){

    logInput();

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        grayWS->printer->print(puma::StringBuilder() << "Tortuosity Particles Error: " <<  errorMessage);
        returnVals.diffusionCoefficient = puma::Vec3<double>(-1.,-1.,-1.);
        returnVals.tortuosity = puma::Vec3<double>(-1.,-1.,-1.);
        returnVals.diffusivity = puma::Vec3<double>(-1.,-1.,-1.);
        returnVals.meanInterceptLength = -1.;

        return returnVals;
    }

    if ( !solve() )  {
        returnVals.diffusionCoefficient = puma::Vec3<double>(-1.,-1.,-1.);
        returnVals.tortuosity = puma::Vec3<double>(-1.,-1.,-1.);
        returnVals.diffusivity = puma::Vec3<double>(-1.,-1.,-1.);
        returnVals.meanInterceptLength = -1.;

        return returnVals;
    }

    porosity = puma::compute_VolumeFraction(grayWS,cutoff,numThreads);

    returnVals.tortuosity.x = porosity/returnVals.diffusivity.x;
    returnVals.tortuosity.y = porosity/returnVals.diffusivity.y;
    returnVals.tortuosity.z = porosity/returnVals.diffusivity.z;

    logOutput();

    return returnVals;
}



bool TortuosityParticles_Isosurface::solve(){

    grayWS->printer->print("Starting Solve ");
    grayWS->printer->print("Generating Surfaces ... " );

    generateSurfaces();

    grayWS->printer->print("Completed");
    grayWS->printer->print("Computing Mean Intercept Length ... " );

    computeMeanInterceptLength();

    grayWS->printer->print("Completed");
    grayWS->printer->print("Generating Walkers ... " );

    computeTotalLength();
    generateWalkers();

    grayWS->printer->print("Completed");
    grayWS->printer->print("Executing Random Walks ... " );

    executeRandomWalks();

    grayWS->printer->print("Completed");
    grayWS->printer->print("Computing Solution ... ");

    computeDiffusionCoefficient();
    computeDiffusivity();

    grayWS->printer->print("Completed");

    return true;
}


double manipulateGrayscaleMatrix(puma::Matrix<float> *Matrix, puma::Cutoff cutoff){

    double average = (double)(cutoff.first+cutoff.second)/2;
    Matrix->flipAroundValue((float)average);
    return cutoff.first;

}

bool TortuosityParticles_Isosurface::generateSurfaces() {

    //generating the array for the vectors
    puma::Timer time;
    voxels = new Voxel[grayWS->size()];

//    int count = 0;

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<X-1;i++){
        for(int j=0;j<Y-1;j++){
            for(int k=0;k<Z-1;k++){

                puma::Matrix<float> newWork(2,2,2,1e-6);

                newWork.at(0)=grayWS->matrix.at((long)Z*(Y*(i) + j) + k);
                newWork.at(1)=grayWS->matrix.at((long)Z*(Y*(i) + j) + k+1);
                newWork.at(2)=grayWS->matrix.at((long)Z*(Y*(i) + j+1) + k);
                newWork.at(3)=grayWS->matrix.at((long)Z*(Y*(i) + j+1) + k+1);
                newWork.at(4)=grayWS->matrix.at((long)Z*(Y*(i+1) + j) + k);
                newWork.at(5)=grayWS->matrix.at((long)Z*(Y*(i+1) + j) + k+1);
                newWork.at(6)=grayWS->matrix.at((long)Z*(Y*(i+1) + j+1) + k);
                newWork.at(7)=grayWS->matrix.at((long)Z*(Y*(i+1) + j+1) + k+1);

                double isoVal = manipulateGrayscaleMatrix(&newWork, cutoff);

                MarchingCubes mc(&newWork, &voxels[(long)Z*(Y*(i) + j) + k].triangles);
                mc.run( isoVal ) ;

//                count += voxels[(long)Z*(Y*(i) + j) + k].triangles.size();

            }
        }
    }

//    std::vector< puma::Triangle<float> > triangles(count);
//    int newcount = 0;
//    for(int i=0;i<X-1;i++){
//        for(int j=0;j<Y-1;j++) {
//            for (int k = 0; k < Z - 1; k++) {
//                for(int t=0;t<voxels[(long)Z*(Y*(i) + j) + k].triangles.size();t++) {
//                    triangles[newcount] = voxels[(long)Z*(Y*(i) + j) + k].triangles[t] + puma::Vec3<float>((float)i,(float)j,(float)k);
//                    newcount++;
//                }
//
//            }
//        }
//    }
//
//    puma::export_STL(&triangles,false,"/Users/josephferguson/Desktop/wtfiswrongwithmytriangles.stl");
//
//    puma::Workspace outwork(grayWS->X()-1, grayWS->Y()-1, grayWS->Z()-1, 1e-6);
//    puma::export_3DTiff(&outwork,"/Users/josephferguson/Desktop/testmatrix.tif",false);



    int numTriangles =0;
    for(long i=0;i<grayWS->matrix.size();i++){
        numTriangles+=voxels[i].triangles.size();
    }
    grayWS->printer->print(puma::StringBuilder() << "Number of Triangles: " << numTriangles);

    grayWS->printer->print(puma::StringBuilder() << "Time to create voxel array: " << time.elapsed());

    return true;
}

void TortuosityParticles_Isosurface::computeDiffusionCoefficient() {
    double avgTime=0;

    walkerCount=0;
    for(auto & walker : walkers){if(!walker.skip) walkerCount++;}

    for(auto & walker : walkers){
        avgTime+=walker.walkTime/walkerCount;
    }

    double time = avgTime*0.9;
    double meanSqDisplacementX = y_MSD0[99] - y_MSD0[9];
    double meanSqDisplacementY = y_MSD1[99] - y_MSD1[9];
    double meanSqDisplacementZ = y_MSD2[99] - y_MSD2[9];

    returnVals.diffusionCoefficient.x = meanSqDisplacementX/(time*2);
    returnVals.diffusionCoefficient.y = meanSqDisplacementY/(time*2);
    returnVals.diffusionCoefficient.z = meanSqDisplacementZ/(time*2);

}

void TortuosityParticles_Isosurface::computeDiffusivity(){
    double minX=1e10;
    double maxX=-1e10;
    for(auto & walker : walkers){
        if(!walker.skip){
            if(walker.xPos<minX) minX=walker.xPos;
            if(walker.xPos>maxX) maxX=walker.xPos;
        }
    }

    grayWS->printer->print(puma::StringBuilder() << "minX: " << minX << "    maxX: " << maxX);
    grayWS->printer->print(puma::StringBuilder() << "Walker count: " << walkerCount << " out of " << walkers.size());

    meanFreePath*=grayWS->voxelLength;
    Dp=meanVelocity*((meanFreePath * returnVals.meanInterceptLength)/(meanFreePath + returnVals.meanInterceptLength))/3.0;
    if(c_temp==0){Dp=meanVelocity*meanFreePath/3.0;}//in the case that there are no wall collisions at all (usually empty box)

    porosity = puma::compute_VolumeFraction(grayWS,cutoff,numThreads);

    //Deff now contains De/Dp used for the calculation of the tortuosity
    returnVals.diffusivity.x=returnVals.diffusionCoefficient.x*porosity/Dp;
    returnVals.diffusivity.y=returnVals.diffusionCoefficient.y*porosity/Dp;
    returnVals.diffusivity.z=returnVals.diffusionCoefficient.z*porosity/Dp;
}

void TortuosityParticles_Isosurface::executeRandomWalks() {

    int intervals = 100;

    for(int t=0;t<intervals;t++){
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(int i=0;i<numParticles;i++){//executing random walks
            randomWalk(i,(double)(t+1)/intervals*totalLength);
        }

        walkerCount=0;
        for(auto & walker : walkers){if(!walker.skip) walkerCount++;}


        std::vector<double> MSD(3, 0);//The mean square displacement of the particles
        for(auto & walker : walkers){

            if(!walker.skip){
                MSD[0]+=((walker.xPos-walker.startX))*((walker.xPos-walker.startX));
                MSD[1]+=((walker.yPos-walker.startY))*((walker.yPos-walker.startY));
                MSD[2]+=((walker.zPos-walker.startZ))*((walker.zPos-walker.startZ));
            }
        }
        MSD[0]*=(grayWS->voxelLength*grayWS->voxelLength/walkerCount);
        MSD[1]*=(grayWS->voxelLength*grayWS->voxelLength/walkerCount);
        MSD[2]*=(grayWS->voxelLength*grayWS->voxelLength/walkerCount);

        double avgTimeLocal=0;
        long collisionCount=0;

        for(auto & walker : walkers){
            if(!walker.skip){
                collisionCount+=walker.collisionCount;
                avgTimeLocal+=walker.walkTime/walkerCount;
            }
        }

        x_time.push_back(avgTimeLocal);
        y_MSD0.push_back(MSD[0]);
        y_MSD1.push_back(MSD[1]);
        y_MSD2.push_back(MSD[2]);

        // For data shuffle to the GUI if the GUI is being used
        std::vector<double> pltData(4);
        pltData[0] = MSD[0];
        pltData[1] = MSD[1];
        pltData[2] = MSD[2];
        pltData[3] = avgTimeLocal;

        grayWS->printer->passData("mean square displacement", pltData);


    }
}

void TortuosityParticles_Isosurface::computeTotalLength(){
    if(std::abs(totalLength)<1){
        totalLength=std::max(grayWS->X(), grayWS->Y());
        totalLength=100*std::max(totalLength,(double)grayWS->Z()); //setting totalDistance to 1x the longest parameter
    }
}

void TortuosityParticles_Isosurface::computeMeanInterceptLength() {
    //Determining the mean intercept length, which has to be done in the Knudsen regime
    int b_numParticles=numParticles;//backing up the inputs
    double b_totalLength=totalLength;
    double b_meanFreePath=meanFreePath;

    walkers.clear();
    numParticles=5000;
    totalLength=100.*std::max(std::max(grayWS->X(), grayWS->Y()),grayWS->Z());
    meanFreePath=totalLength;
    generateWalkers();

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<numParticles;i++){//executing random walks
        randomWalk(i,totalLength);
    }

    t_temp=0;
    c_temp=0;
    for(auto & walker : walkers){
        if(!walker.skip){
            c_temp+=(double)walker.collisionCount;
            t_temp+=walker.walkTime*meanVelocity;
        }
    }
    dBar=t_temp/c_temp;
    returnVals.meanInterceptLength=dBar;

    walkers.clear();
    meanFreePath=b_meanFreePath;
    numParticles=b_numParticles;
    totalLength=b_totalLength;

}


bool TortuosityParticles_Isosurface::generateWalkers(){

    walkers.clear();
    walkers.resize(numParticles);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<(int)walkers.size();i++){
        walkers[i].engine.seed((i+1)*randomSeed);

        double tCount = 1;
        double iVal = 255;


        while(tCount!=0||iVal>cutoff.second||iVal<cutoff.first){ //making sure that the walker is placed within the voids
            walkers[i].xPos = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*(X-2);
            walkers[i].yPos = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*(Y-2);
            walkers[i].zPos = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*(Z-2);
            while(walkers[i].xPos==(int)walkers[i].xPos||walkers[i].yPos==(int)walkers[i].yPos||walkers[i].zPos==(int)walkers[i].zPos){
                walkers[i].xPos = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*(X-2);
                walkers[i].yPos = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*(Y-2);
                walkers[i].zPos = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*(Z-2);
            }
            tCount=voxels[(long)Z*(Y*((int)walkers[i].xPos) + (int)walkers[i].yPos) + (int)walkers[i].zPos].triangles.size();
            iVal=interpolatedGrayscale(walkers[i].xPos, walkers[i].yPos, walkers[i].zPos);

        }

        walkers[i].startX = walkers[i].xPos;
        walkers[i].startY = walkers[i].yPos;
        walkers[i].startZ = walkers[i].zPos;


        walkers[i].lastXCollision=-1;
        walkers[i].lastYCollision=-1;
        walkers[i].lastZCollision=-1;

        walkers[i].xVox=(int)walkers[i].xPos;
        walkers[i].yVox=(int)walkers[i].yPos;
        walkers[i].zVox=(int)walkers[i].zPos;
        walkers[i].velocity=meanVelocity;
        newDirParticleCollision(i);

        walkers[i].lastTrianglePos=-1;
        walkers[i].lastTriangleNum=-1;
        walkers[i].skip=false;

        walkers[i].walkTime=0;
        walkers[i].deltaRw=0;
        walkers[i].collisionCount=0;
        walkers[i].particleCollisionCount=0;//since this is after the first call to the particle collision, the initial call won't bias the results

    }
    return true;
}



bool TortuosityParticles_Isosurface::randomWalk(int i, double walkLength){
    int reverseX, reverseY, reverseZ;
    std::vector<double> nextPos(4);//calculating the next position of the walk at the closest face

    //Based on the precision of a double, and the
    while(walkers[i].walkTime<(walkLength*voxelLength/meanVelocity)){

        bool stepping = true;
        if(walkers[i].skip){
            return false;
        }

        walkers[i].walkTime-=((walkers[i].deltaRw*voxelLength)/walkers[i].velocity);

        while(stepping){//while the walker is continuing it's walk

            if(walkers[i].xPos!=walkers[i].xPos||walkers[i].xPos==INFINITY||walkers[i].yPos!=walkers[i].yPos||walkers[i].yPos==INFINITY||walkers[i].zPos!=walkers[i].zPos||walkers[i].zPos==INFINITY){
                grayWS->printer->print(puma::StringBuilder() << "Numerical Error #1: Walker Position nan or inf");
                walkers[i].skip=true;
                return false;
            }


//            int tCount=voxels[periodicVoxels(walkers[i].xVox, walkers[i].yVox, walkers[i].zVox, &reverseX, &reverseY, &reverseZ)].triangles.size();
//            double iVal=interpolatedGrayscale(walkers[i].xPos, walkers[i].yPos, walkers[i].zPos);
//            if(tCount==0&&iVal>cutoff.second){
//                walkers[i].skip=true;
//            }

            if(walkers[i].skip){
                return false;
            }

            double Xo=walkers[i].xPos;
            double Yo=walkers[i].yPos;
            double Zo=walkers[i].zPos;

            char intersection='a';
            nextPos=nextEdgeIntersection(i);//finding the next edge intersection along the walk
            if(nextPos[3]==0){intersection='x';}
            else if(nextPos[3]==1){intersection='y';}
            else if(nextPos[3]==2){intersection='z';}


            if(nextPos[0]!=nextPos[0]||nextPos[0]==INFINITY||nextPos[1]!=nextPos[1]||nextPos[1]==INFINITY||nextPos[2]!=nextPos[2]||nextPos[2]==INFINITY
               || nextPos[0]>1e12||nextPos[0]<-1e12||nextPos[1]>1e12||nextPos[1]<-1e12||nextPos[2]>1e12||nextPos[2]<-1e12){
                grayWS->printer->print(puma::StringBuilder() << "Numerical Error #2: NextPos nan or inf");
                walkers[i].skip=true;
                return false;
            }

            int intersectCount=0;
            puma::Vec3<double> intersectLocation;
            std::vector<int> triangles;
            std::vector< puma::Vec3<double> > intersectLocations;
            int triangle=-1;
            puma::Triangle<float> testTriangle;
            std::vector< puma::Triangle<float> > testTriangles;

            if((int)voxels[periodicVoxels(walkers[i].xVox, walkers[i].yVox, walkers[i].zVox, &reverseX, &reverseY, &reverseZ)].triangles.size()!=0){

                std::vector< puma::Vec3<double> > S(2);
                S[0].x=walkers[i].xPos-walkers[i].xVox;
                S[0].y=walkers[i].yPos-walkers[i].yVox;
                S[0].z=walkers[i].zPos-walkers[i].zVox;
                S[1].x=nextPos[0]-walkers[i].xVox;
                S[1].y=nextPos[1]-walkers[i].yVox;
                S[1].z=nextPos[2]-walkers[i].zVox;

                for(int t=0;t<(int)voxels[periodicVoxels(walkers[i].xVox, walkers[i].yVox, walkers[i].zVox, &reverseX, &reverseY, &reverseZ)].triangles.size();t++){//checking all triangles for intersection

                    testTriangle = voxels[periodicVoxels(walkers[i].xVox, walkers[i].yVox, walkers[i].zVox, &reverseX, &reverseY, &reverseZ)].triangles[t];
                    if(reverseX==-1){
                        testTriangle.p0.x=testTriangle.p0.x*(-1)+1;
                        testTriangle.p1.x=testTriangle.p1.x*(-1)+1;
                        testTriangle.p2.x=testTriangle.p2.x*(-1)+1;
                    }
                    if(reverseY==-1){
                        testTriangle.p0.y=testTriangle.p0.y*(-1)+1;
                        testTriangle.p1.y=testTriangle.p1.y*(-1)+1;
                        testTriangle.p2.y=testTriangle.p2.y*(-1)+1;
                    }
                    if(reverseZ==-1){
                        testTriangle.p0.z=testTriangle.p0.z*(-1)+1;
                        testTriangle.p1.z=testTriangle.p1.z*(-1)+1;
                        testTriangle.p2.z=testTriangle.p2.z*(-1)+1;
                    }

                    if(segmentTriangle(&S[0], &S[1] ,&testTriangle,&intersectLocation)){
                        long long tempTrianglePos=(long)Z*(Y*((int)walkers[i].xVox) + (int)walkers[i].yVox) + (int)walkers[i].zVox;
                        if(walkers[i].lastTriangleNum==t&&walkers[i].lastTrianglePos==tempTrianglePos){

                        }else{
                            intersectCount++;
                            triangles.push_back(t);
                            intersectLocations.push_back(intersectLocation);
                            testTriangles.push_back(testTriangle);
                            walkers[i].lastTrianglePos=tempTrianglePos;
                        }
                    }
                }

                if(intersectCount==1){
                    triangle=triangles[0];
                    intersectLocation=intersectLocations[0];
                    testTriangle=testTriangles[0];
                }else if(intersectCount>1){
                    double minDistance=sqrt(pow(((walkers[i].xPos-walkers[i].xVox)-intersectLocations[0].x),2)+pow(((walkers[i].yPos-walkers[i].yVox)-intersectLocations[0].y),2)+pow(((walkers[i].zPos-walkers[i].zVox)-intersectLocations[0].z),2));

                    triangle=triangles[0];
                    intersectLocation=intersectLocations[0];
                    testTriangle=testTriangles[0];
                    for(unsigned int tri=1;tri<triangles.size();tri++){
                        double tempDist=sqrt(pow(((walkers[i].xPos-walkers[i].xVox)-intersectLocations[tri].x),2)+pow(((walkers[i].yPos-walkers[i].yVox)-intersectLocations[tri].y),2)+pow(((walkers[i].zPos-walkers[i].zVox)-intersectLocations[tri].z),2));

                        if(tempDist<minDistance&&tempDist>1e-9){
                            minDistance=tempDist;
                            triangle=triangles[tri];
                            intersectLocation=intersectLocations[tri];
                            testTriangle=testTriangles[tri];
                        }
                    }



                }
                walkers[i].lastTriangleNum=triangle;


            }

            if(intersectCount>0){

                //intersect happened but it's farther than the overall walk
                if(walkers[i].deltaRw + sqrt((walkers[i].xPos-(intersectLocation.x+walkers[i].xVox))*(walkers[i].xPos-(intersectLocation.x+walkers[i].xVox))+(walkers[i].yPos-(intersectLocation.y+walkers[i].yVox))*(walkers[i].yPos-(intersectLocation.y+walkers[i].yVox))+(walkers[i].zPos-(intersectLocation.z+walkers[i].zVox))*(walkers[i].zPos-(intersectLocation.z+walkers[i].zVox))) > walkers[i].deltaR){

                    walkers[i].xPos += (walkers[i].deltaR-walkers[i].deltaRw)*walkers[i].xDir;//interpolating the end point within the voxel
                    walkers[i].yPos += (walkers[i].deltaR-walkers[i].deltaRw)*walkers[i].yDir;
                    walkers[i].zPos += (walkers[i].deltaR-walkers[i].deltaRw)*walkers[i].zDir;
                    walkers[i].deltaRw += sqrt((walkers[i].xPos-Xo)*(walkers[i].xPos-Xo)+(walkers[i].yPos-Yo)*(walkers[i].yPos-Yo)+(walkers[i].zPos-Zo)*(walkers[i].zPos-Zo));
                    walkers[i].walkTime+=((walkers[i].deltaRw*voxelLength)/walkers[i].velocity);
                    newDirParticleCollision(i);
                    walkers[i].deltaRw = 0;

                    break;
                }else{//intersection happened
                    walkers[i].xPos=intersectLocation.x+walkers[i].xVox;
                    walkers[i].yPos=intersectLocation.y+walkers[i].yVox;
                    walkers[i].zPos=intersectLocation.z+walkers[i].zVox;

                    if(walkers[i].xPos==walkers[i].lastXCollision&&walkers[i].yPos==walkers[i].lastYCollision&&walkers[i].zPos==walkers[i].lastZCollision){
                        grayWS->printer->print(puma::StringBuilder() << "Numerical Error #3: Triangle Collision Error - Due to MC");
                        walkers[i].skip=true;
                        return false;
                    }else{
                        walkers[i].lastXCollision=walkers[i].xPos;
                        walkers[i].lastYCollision=walkers[i].yPos;
                        walkers[i].lastZCollision=walkers[i].zPos;


                        walkers[i].walkTime+=((walkers[i].deltaRw*voxelLength)/walkers[i].velocity);

                        double normX = (((testTriangle.p1 - testTriangle.p0).cross(testTriangle.p2 - testTriangle.p0)).norm()).x;
                        double normY = (((testTriangle.p1 - testTriangle.p0).cross(testTriangle.p2 - testTriangle.p0)).norm()).y;
                        double normZ = (((testTriangle.p1 - testTriangle.p0).cross(testTriangle.p2 - testTriangle.p0)).norm()).z;
                        newDirSurfaceCollision(i,normX, normY, normZ);
                        walkers[i].deltaRw = 0;

                        break;
                    }
                }
            }else{// There was no intersection with a triangle

                if(walkers[i].deltaRw + sqrt((walkers[i].xPos-nextPos[0])*(walkers[i].xPos-nextPos[0])+(walkers[i].yPos-nextPos[1])*(walkers[i].yPos-nextPos[1])+(walkers[i].zPos-nextPos[2])*(walkers[i].zPos-nextPos[2])) > walkers[i].deltaR){
                    walkers[i].xPos += (walkers[i].deltaR-walkers[i].deltaRw)*walkers[i].xDir;//interpolating the end point within the voxel
                    walkers[i].yPos += (walkers[i].deltaR-walkers[i].deltaRw)*walkers[i].yDir;
                    walkers[i].zPos += (walkers[i].deltaR-walkers[i].deltaRw)*walkers[i].zDir;
                    walkers[i].deltaRw += sqrt((walkers[i].xPos-Xo)*(walkers[i].xPos-Xo)+(walkers[i].yPos-Yo)*(walkers[i].yPos-Yo)+(walkers[i].zPos-Zo)*(walkers[i].zPos-Zo));
                    walkers[i].walkTime+=((walkers[i].deltaRw*voxelLength)/walkers[i].velocity);
                    newDirParticleCollision(i);
                    walkers[i].deltaRw = 0;
                    break;
                }else{
                    walkers[i].xPos=nextPos[0];
                    walkers[i].yPos=nextPos[1];
                    walkers[i].zPos=nextPos[2];
                    walkers[i].deltaRw += sqrt((walkers[i].xPos-Xo)*(walkers[i].xPos-Xo)+(walkers[i].yPos-Yo)*(walkers[i].yPos-Yo)+(walkers[i].zPos-Zo)*(walkers[i].zPos-Zo));
                    switch(intersection){
                        case 'x'://intersection on x
                            if(walkers[i].xDir>0){walkers[i].xVox+=1;}
                            else{walkers[i].xVox-=1;}
                            break;
                        case 'y'://intersection on y
                            if(walkers[i].yDir>0){walkers[i].yVox+=1;}
                            else{walkers[i].yVox-=1;}
                            break;
                        case 'z'://intersection on z
                            if(walkers[i].zDir>0){walkers[i].zVox+=1;}
                            else{walkers[i].zVox-=1;}
                            break;
                        default:
                            return false;
                    }

                }

            }
            if((walkers[i].walkTime+(walkers[i].deltaRw*voxelLength/walkers[i].velocity))>(walkLength*voxelLength/meanVelocity))
            {
                double extraTime=(walkers[i].walkTime+(walkers[i].deltaRw*voxelLength/walkers[i].velocity))-(walkLength*voxelLength/meanVelocity);
                walkers[i].xPos -= extraTime*walkers[i].velocity*walkers[i].xDir;//interpolating the end point within the voxel
                walkers[i].yPos -= extraTime*walkers[i].velocity*walkers[i].yDir;
                walkers[i].zPos -= extraTime*walkers[i].velocity*walkers[i].zDir;
                walkers[i].walkTime=(walkLength*voxelLength/meanVelocity);
                stepping=false;
            }
        }
    }
    return true;
}



double TortuosityParticles_Isosurface::interpolatedGrayscale(double x, double y, double z){

    while(x<0||x>X-1){
        if(x<0) x = -x;
        else if(x>X-1) x=2*(X-1)-x;
    }
    while(y<0||y>Y-1){
        if(y<0) y = -y;
        else if(y>Y-1) y=2*(Y-1)-y;
    }
    while(z<0||z>Z-1){
        if(z<0) z = -z;
        else if(z>Z-1) z=2*(Z-1)-z;
    }

    double dx,dy,dz;

    if((int)x==x){
        dy = y - (int)y;
        dz = z - (int)z;
        return ((grayWS->matrix.at((long)Z*(Y*((int)x) + ((int)y)) + ((int)(z+1))))*(1-dy)+(grayWS->matrix.at((long)Z*(Y*((int)(x)) + ((int)(y+1))) + ((int)(z+1))))*dy)*dz+((grayWS->matrix.at((long)Z*(Y*((int)x) + ((int)y)) + ((int)z)))*(1-dy)+(grayWS->matrix.at((long)Z*(Y*((int)x) + ((int)(y+1))) + ((int)z)))*dy)*(1-dz);

    }
    else if((int)y==y){
        dx = x - (int)x;
        dz = z - (int)z;
        return ((grayWS->matrix.at((long)Z*(Y*((int)x) + ((int)y)) + ((int)(z+1))))*(1-dx)+(grayWS->matrix.at((long)Z*(Y*((int)(x+1)) + ((int)(y))) + ((int)(z+1))))*dx)*dz+((grayWS->matrix.at((long)Z*(Y*((int)x) + ((int)y)) + ((int)z)))*(1-dx)+(grayWS->matrix.at((long)Z*(Y*((int)(x+1)) + ((int)(y))) + ((int)z)))*dx)*(1-dz);

    }
    else if((int)z==z){
        dx = x - (int)x;
        dy = y - (int)y;

        return ((grayWS->matrix.at((long)Z*(Y*((int)x) + ((int)(y+1))) + ((int)z)))*(1-dx)+(grayWS->matrix.at((long)Z*(Y*((int)(x+1)) + ((int)y+1)) + ((int)(z))))*dx)*dy+((grayWS->matrix.at((long)Z*(Y*((int)x) + ((int)y)) + ((int)z)))*(1-dx)+(grayWS->matrix.at((long)Z*(Y*((int)(x+1)) + ((int)y)) + ((int)z)))*dx)*(1-dy);
    }else{
        double c1,c2,c3,c4,p1,p2,v;

        dx = x - (int)x;
        dy = y - (int)y;
        dz = z - (int)z;

        c1=grayWS->matrix.at((long)Z*(Y*((int)x) + ((int)(y))) + ((int)z))*(1-dx)+grayWS->matrix.at((long)Z*(Y*((int)x+1) + ((int)(y))) + ((int)z))*(dx);
        c2=grayWS->matrix.at((long)Z*(Y*((int)x) + ((int)(y+1))) + ((int)z))*(1-dx)+grayWS->matrix.at((long)Z*(Y*((int)x+1) + ((int)(y+1))) + ((int)z))*(dx);
        c3=grayWS->matrix.at((long)Z*(Y*((int)x) + ((int)(y))) + ((int)z+1))*(1-dx)+grayWS->matrix.at((long)Z*(Y*((int)x+1) + ((int)(y))) + ((int)z+1))*(dx);
        c4=grayWS->matrix.at((long)Z*(Y*((int)x) + ((int)(y+1))) + ((int)z+1))*(1-dx)+grayWS->matrix.at((long)Z*(Y*((int)x+1) + ((int)(y+1))) + ((int)z+1))*(dx);

        p1=c1*(1-dy)+c2*(dy);
        p2=c3*(1-dy)+c4*(dy);

        v = p1*(1-dz)+p2*(dz);

        return v;
    }
}

long TortuosityParticles_Isosurface::periodicVoxels(int xVox, int yVox, int zVox, int *reverseX , int *reverseY , int *reverseZ){
    *reverseX=1;
    *reverseY=1;
    *reverseZ=1;
    while(xVox<0||xVox>X-2){
        if(xVox<0){
            xVox =-xVox-1;
            *reverseX*=-1;
        }
        else if(xVox>X-2){
            xVox=(X-2)-(xVox-(X-1));
            *reverseX*=-1;
        }
    }
    while(yVox<0||yVox>Y-2){
        if(yVox<0){
            yVox=-yVox-1;
            *reverseY*=-1;
        }
        else if(yVox>Y-2){
            yVox=(Y-2)-(yVox-(Y-1));
            *reverseY*=-1;
        }
    }
    while(zVox<0||zVox>Z-2){
        if(zVox<0){
            zVox=-zVox-1;
            *reverseZ*=-1;
        }
        else if(zVox>Z-2){
            zVox=(Z-2)-(zVox-(Z-1));
            *reverseZ*=-1;
        }
    }

    return (long)Z*(Y*(xVox) + yVox) + zVox;
}

double TortuosityParticles_Isosurface::newFreePath(int i){
    return -log(std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8))*meanFreePath;
}

bool TortuosityParticles_Isosurface::newDirParticleCollision(int i){

    double xD=0.,yD=0.,zD=0.;
    double distance = 2;
    while(distance>1){
        xD = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*2-1;
        yD = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*2-1;
        zD = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*2-1;
        distance = sqrt(xD*xD+yD*yD+zD*zD);
    }

    walkers[i].xDir = xD/distance;
    walkers[i].yDir = yD/distance;
    walkers[i].zDir = zD/distance;
    walkers[i].velocity = meanVelocity;

    walkers[i].deltaR = newFreePath(i);

    walkers[i].lastTrianglePos=-1;
    walkers[i].lastTriangleNum=-1;
    walkers[i].particleCollisionCount++;

    return true;

}


bool TortuosityParticles_Isosurface::newDirSurfaceCollision(int i, double xNorm, double yNorm, double zNorm){

    bool loop=true;
    double xDirOriginal=walkers[i].xDir;
    double yDirOriginal=walkers[i].yDir;
    double zDirOriginal=walkers[i].zDir;

    while(loop){
        walkers[i].xDir=xDirOriginal;
        walkers[i].yDir=yDirOriginal;
        walkers[i].zDir=zDirOriginal;

        loop=false;
        double pi = atan(1)*4; //This is the same as pi by machine precision. (it underestimates by ~1.3932677 * 10^-37)

        double rand1 = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8);
        double rand2 = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8);
        double rand3 = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8);

        std::vector<double> surfaceNorm(3);
        surfaceNorm[0]=xNorm;
        surfaceNorm[1]=yNorm;
        surfaceNorm[2]=zNorm;

        double dotP=surfaceNorm[0]*xDirOriginal+surfaceNorm[1]*yDirOriginal+surfaceNorm[2]*zDirOriginal;
        double mag1=sqrt(pow(surfaceNorm[0],2)+pow(surfaceNorm[1],2)+pow(surfaceNorm[2],2));
        double mag2=sqrt(pow(xDirOriginal,2)+pow(yDirOriginal,2)+pow(zDirOriginal,2));
        if(dotP/(mag1*mag2)>0){
            surfaceNorm[0]*=-1;
            surfaceNorm[1]*=-1;
            surfaceNorm[2]*=-1;
        }

        double vrms = sqrt(pi)/2*meanVelocity;
        double vPerp=vrms*sqrt(-log(rand1));
        double theta = 2*pi * rand2;
        double vPar=vrms*sqrt(-log(rand3));
        double vPar1=vPar*sin(theta);
        double vPar2=vPar*cos(theta);
        double dot = surfaceNorm[0]*walkers[i].xDir*walkers[i].velocity+ surfaceNorm[1]*walkers[i].yDir*walkers[i].velocity+ surfaceNorm[2]*walkers[i].zDir*walkers[i].velocity;

        std::vector<double> tan1(3);
        tan1[0]=walkers[i].xDir*walkers[i].velocity-dot*surfaceNorm[0];
        tan1[1]=walkers[i].yDir*walkers[i].velocity-dot*surfaceNorm[1];
        tan1[2]=walkers[i].zDir*walkers[i].velocity-dot*surfaceNorm[2];

        std::vector<double> tan2(3);
        if(pow(tan1[0],2)+pow(tan1[1],2)+pow(tan1[2],2)==0){
            rand1 = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8);
            rand2 = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8);
            rand3 = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8);

            tan2[0]=rand1;
            tan2[1]=rand2;
            tan2[2]=rand3;
            tan1[0]=surfaceNorm[1]*tan2[2]-surfaceNorm[2]*tan2[1];
            tan1[1]=surfaceNorm[2]*tan2[0]-surfaceNorm[0]*tan2[2];
            tan1[2]=surfaceNorm[0]*tan2[1]-surfaceNorm[1]*tan2[0];
        }

        double magnitude=sqrt(pow(tan1[0],2)+pow(tan1[1],2)+pow(tan1[2],2));
        tan1[0]/=magnitude;
        tan1[1]/=magnitude;
        tan1[2]/=magnitude;

        tan2[0]=surfaceNorm[1]*tan1[2]-surfaceNorm[2]*tan1[1];
        tan2[1]=surfaceNorm[2]*tan1[0]-surfaceNorm[0]*tan1[2];
        tan2[2]=surfaceNorm[0]*tan1[1]-surfaceNorm[1]*tan1[0];

        std::vector<double> v(3);
        v[0]=vPerp*surfaceNorm[0]+vPar1*tan1[0]+vPar2*tan2[0];
        v[1]=vPerp*surfaceNorm[1]+vPar1*tan1[1]+vPar2*tan2[1];
        v[2]=vPerp*surfaceNorm[2]+vPar1*tan1[2]+vPar2*tan2[2];

        magnitude=sqrt(pow(v[0],2)+pow(v[1],2)+pow(v[2],2));
        walkers[i].xDir=v[0]/magnitude;
        walkers[i].yDir=v[1]/magnitude;
        walkers[i].zDir=v[2]/magnitude;

        walkers[i].velocity=meanVelocity;//magnitude;
        walkers[i].deltaR = newFreePath(i);

        //temp ferguson 8-15-2017
        //        meanVelocityCount+=walkers[i].velocity;
        //        meanCount++;

    }
    walkers[i].collisionCount++;

    return true;

}

std::vector<double> TortuosityParticles_Isosurface::nextEdgeIntersection(int i){
    int nX,nY,nZ;//for calculating the next intersecting face in each direction
    double tX,tY,tZ;//for calculating the time to intersecting face in each direction
    std::vector<double> nextPos(4);
    // nextPos[0] - x coordinate of next edge intersection
    // nextPos[1] - y coordinate of next edge intersection
    // nextPos[2] - z coordinate of next edge intersection
    // nextPos[3] - The edge at which the intersection occurs (x=0, y=1, z=2)

    //Step 1: Setting nX, nY, and nZ to be the next interfaces reached for each direction
    if(walkers[i].xDir>0){nX = walkers[i].xVox+1;} else{nX = walkers[i].xVox;}
    if(walkers[i].yDir>0){nY = walkers[i].yVox+1;} else{nY = walkers[i].yVox;}
    if(walkers[i].zDir>0){nZ = walkers[i].zVox+1;} else{nZ = walkers[i].zVox;}

    //Step 2: Settng tX, tY, and tZ to be the time to reach the next interface in each direction
    tX = (nX-walkers[i].xPos)/walkers[i].xDir;//time to get to x wall
    tY = (nY-walkers[i].yPos)/walkers[i].yDir;//time to get to y wall
    tZ = (nZ-walkers[i].zPos)/walkers[i].zDir;//time to get to z wall

    //Step 3: Setting the next position of the walker based on the nearest interface
    if(tX<=tY&&tX<=tZ){//if the x wall is the closest
        nextPos[0] = nX;
        nextPos[1] = walkers[i].yPos+walkers[i].yDir*tX;
        nextPos[2] = walkers[i].zPos+walkers[i].zDir*tX;
        nextPos[3]=0;
    }else if(tY<=tZ&&tY<=tX){//if the y wall is the closest
        nextPos[0] = walkers[i].xPos+walkers[i].xDir*tY;
        nextPos[1] = nY;
        nextPos[2] = walkers[i].zPos+walkers[i].zDir*tY;
        nextPos[3]=1;
    }else{//if the z wall is the closest
        nextPos[0] = walkers[i].xPos+walkers[i].xDir*tZ;
        nextPos[1] = walkers[i].yPos+walkers[i].yDir*tZ;
        nextPos[2] = nZ;
        nextPos[3]=2;
    }
    return nextPos;
}

bool TortuosityParticles_Isosurface::segmentTriangle(puma::Vec3<double> *S1, puma::Vec3<double> *S2, puma::Triangle<float> *T , puma::Vec3<double> *Intersect){

    puma::Vec3<double> Td0 = puma::Vec3<double>(T->p0.x,T->p0.y,T->p0.z);
    puma::Vec3<double> Td1 = puma::Vec3<double>(T->p1.x,T->p1.y,T->p1.z);
    puma::Vec3<double> Td2 = puma::Vec3<double>(T->p2.x,T->p2.y,T->p2.z);

    puma::Vec3<double>    u, v, n;
    puma::Vec3<double>    dir, w0, w;
    double     r, a, b;

    u = Td1 - Td0;
    v = Td2 - Td0;
    n = u.cross(v);
    if (n.x==0&&n.y==0&&n.z==0)  {
        return false;
    }

    dir = *S2 - *S1;
    w0 = *S1 - Td0;
    a = (n.dot(w0))*-1;
    b = n.dot(dir);

    if (fabs(b) < 0.00000001) {
        return a == 0;
    }

    r = a / b;

    if (r < 0.0){return false;}

    Intersect->operator =( *S1 + (dir * r));

    double    uu, uv, vv, wu, wv, D;
    uu = u.dot(u);
    uv = u.dot(v);
    vv = v.dot(v);
    w = Intersect->operator -( Td0 );
    wu = w.dot(u);
    wv = w.dot(v);
    D = uv * uv - uu * vv;

    double s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < 0.0 || s > 1.0){return false;}

    t = (uv * wu - uu * wv) / D;
    return !(t < 0.0 || (s + t) > 1.0);

}

bool TortuosityParticles_Isosurface::logInput() {
    puma::Logger *logger = grayWS->log;

    logger->appendLogSection("Compute Tortuosity");
    logger->appendLogLine(" -- Inputs:");
    logger->appendLogItem("interior Size: ");
    logger->appendLogItem(grayWS->X());
    logger->appendLogItem(", ");
    logger->appendLogItem(grayWS->Y());
    logger->appendLogItem(", ");
    logger->appendLogItem(grayWS->Z());
    logger->newLine();

    logger->appendLogItem("Low puma::Cutoff: ");
    logger->appendLogItem(cutoff.first);
    logger->newLine();
    logger->appendLogItem("High puma::Cutoff: ");
    logger->appendLogItem(cutoff.second);
    logger->newLine();
    logger->appendLogItem("Number of Particles: ");
    logger->appendLogItem(numParticles);
    logger->newLine();
    logger->appendLogItem("Mean Free Path: ");
    logger->appendLogItem(meanFreePath);
    logger->newLine();
    logger->appendLogItem("Mean Velocity: ");
    logger->appendLogItem(meanVelocity);
    logger->newLine();
    logger->appendLogItem("Random Seed: ");
    logger->appendLogItem(randomSeed);
    logger->newLine();
    logger->appendLogItem("Total Walk Length: ");
    logger->appendLogItem(totalLength);
    logger->newLine();
    logger->appendLogItem("Number of Threads: ");
    logger->appendLogItem(numThreads);
    logger->writeLog();

    return true;
}

bool TortuosityParticles_Isosurface::logOutput() {
    puma::Logger *logger = grayWS->log;

    logger->appendLogLine(" -- Output:");
    logger->appendLogItem("Diffusity: (Assuming Variable)");
    logger->appendLogItem(returnVals.diffusivity.x);
    logger->appendLogItem(",");
    logger->appendLogItem(returnVals.diffusivity.y);
    logger->appendLogItem(",");
    logger->appendLogItem(returnVals.diffusivity.z);
    logger->newLine();
    logger->appendLogItem("Tortuosity: (Assuming Variable) ");
    logger->appendLogItem(returnVals.tortuosity.x);
    logger->appendLogItem(",");
    logger->appendLogItem(returnVals.tortuosity.y);
    logger->appendLogItem(",");
    logger->appendLogItem(returnVals.tortuosity.z);
    logger->newLine();
    logger->appendLogItem("Diffusion Coefficient ");
    logger->appendLogItem(returnVals.diffusionCoefficient.x);
    logger->appendLogItem(",");
    logger->appendLogItem(returnVals.diffusionCoefficient.y);
    logger->appendLogItem(",");
    logger->appendLogItem(returnVals.diffusionCoefficient.z);
    logger->newLine();
    logger->appendLogItem("Mean Intercept Length ");
    logger->appendLogItem(returnVals.meanInterceptLength);
    logger->newLine();
    logger->writeLog();

    return true;
}

bool TortuosityParticles_Isosurface::errorCheck(std::string *errorMessage) {
    if(grayWS->matrix.size() == 0) {
        *errorMessage = "Empty Material Matrix";
        return false;
    }

    if(grayWS->X() < 2) {
        *errorMessage = "Invalid X dimension. Must be >=2 in each direction for the marching cubes to work";
        return false;
    }

    if(grayWS->Y() < 2) {
        *errorMessage = "Invalid Y dimension. Must be >=2 in each direction for the marching cubes to work";
        return false;
    }

    if(grayWS->Z() < 2) {
        *errorMessage = "Invalid Z dimension. Must be >=2 in each direction for the marching cubes to work";
        return false;
    }

    if(numParticles < 1 || numParticles > 1e9) {
        *errorMessage = "Invalid Number of Particles. Should be between 1 and 1e9 (Usually 100,000 - 1,000,000) ";
        return false;
    }

    if(meanFreePath < 0) {
        *errorMessage = "Invalid Mean Free Path. Must be > 0 ";
        return false;
    }

    if(meanVelocity < 0) {
        *errorMessage = "Invalid Mean Velocity. Must be > 0 ";
        return false;
    }
    if(totalLength < 0) {
        *errorMessage = "Invalid Total Length. Must be > 0 ";
        return false;
    }
    if(randomSeed < 0 || randomSeed > UINT32_MAX) {
        *errorMessage = "Invalid Mean Velocity. Must be > 0 and below UINT32_MAX";
        return false;
    }

    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }

    return true;
}
