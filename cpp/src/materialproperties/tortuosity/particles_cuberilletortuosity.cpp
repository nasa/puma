#include "particles_cuberilletortuosity.h"


puma::TortuosityReturn puma::compute_particle_cuberille_Tortuosity(puma::Workspace *grayWS, puma::Cutoff cutoff,
                                                                   int numParticles, double meanFreePath, double meanVelocity,
                                                                   int randomSeed, double totalLength, int numThreads) {
    if (cutoff.first < -0.5 || cutoff.second> 32767 || cutoff.first > cutoff.second) {
        grayWS->printer->print("DiffusivityError: Invalid Cutoff Ranges");
        puma::TortuosityReturn returnVals;
        returnVals.diffusionCoefficient = puma::Vec3<double>(-1.,-1.,-1.);
        returnVals.tortuosity = puma::Vec3<double>(-1.,-1.,-1.);
        returnVals.diffusivity = puma::Vec3<double>(-1.,-1.,-1.);
        returnVals.meanInterceptLength = -1.;
        return returnVals;
    }

    puma::Workspace segWS(grayWS->shape(), grayWS->log);
    segWS.setPrinter(grayWS->printer);

    segWS.setMaterialID(grayWS,cutoff,1);

    TortuosityParticles solver(&segWS,1,numParticles,meanFreePath,meanVelocity,randomSeed,totalLength,numThreads);
    return solver.compute();

}


TortuosityParticles::TortuosityParticles(puma::Workspace *segWS, int voidValue, int numParticles, double meanFreePath,
                                         double meanVelocity, int randomSeed, double totalLength, int numThreads){

    this->segWS = segWS;
    this->voidValue = voidValue;
    this->numParticles=numParticles;
    this->meanFreePath=meanFreePath;
    this->meanVelocity=meanVelocity;
    this->numThreads=numThreads;
    this->randomSeed=randomSeed*10000+1;
    this->totalLength=totalLength;

    X = (int)segWS->X();
    Y = (int)segWS->Y();
    Z = (int)segWS->Z();

}


puma::TortuosityReturn TortuosityParticles::compute(){

    logInput();

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        segWS->printer->print(puma::StringBuilder() << "Tortuosity Particles Error: " <<  errorMessage);
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

    porosity = puma::compute_VolumeFraction(segWS,voidValue,numThreads);

    returnVals.tortuosity.x = porosity/returnVals.diffusivity.x;
    returnVals.tortuosity.y = porosity/returnVals.diffusivity.y;
    returnVals.tortuosity.z = porosity/returnVals.diffusivity.z;

    logOutput();

    return returnVals;
}



bool TortuosityParticles::solve(){

    segWS->printer->print("Starting Solve ");
    segWS->printer->print("Computing Mean Intercept Length ... " );

    computeMeanInterceptLength();
    computeTotalLength();

    segWS->printer->print("Completed");
    segWS->printer->print("Generating Walkers ... " );

    generateWalkers();

    segWS->printer->print("Completed");
    segWS->printer->print("Executing Random Walks ... ");

    executeRandomWalks();

    segWS->printer->print("Completed");
    segWS->printer->print("Computing Solution ... ");

    computeDiffusionCoefficient();
    computeDiffusivity();

    segWS->printer->print("Completed");

    return true;
}

void TortuosityParticles::computeDiffusionCoefficient() {
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

void TortuosityParticles::computeDiffusivity(){
    double minX=1e10;
    double maxX=-1e10;
    for(auto & walker : walkers){
        if(!walker.skip){
            if(walker.xPos<minX) minX=walker.xPos;
            if(walker.xPos>maxX) maxX=walker.xPos;
        }
    }

    segWS->printer->print(puma::StringBuilder() << "minX: " << minX << "    maxX: " << maxX);
    segWS->printer->print(puma::StringBuilder() << "Walker count: " << walkerCount << " out of " << walkers.size());

    meanFreePath*=segWS->voxelLength;
    Dp=meanVelocity*((meanFreePath* returnVals.meanInterceptLength)/(meanFreePath+ returnVals.meanInterceptLength))/3.0;
    if(c_temp==0){Dp=meanVelocity*meanFreePath/3.0;}//in the case that there are no wall collisions at all (usually empty box)

    porosity = puma::compute_VolumeFraction(segWS,voidValue,numThreads);

    //Deff now contains De/Dp used for the calculation of the tortuosity
    returnVals.diffusivity.x=returnVals.diffusionCoefficient.x*porosity/Dp;
    returnVals.diffusivity.y=returnVals.diffusionCoefficient.y*porosity/Dp;
    returnVals.diffusivity.z=returnVals.diffusionCoefficient.z*porosity/Dp;
}

void TortuosityParticles::executeRandomWalks() {
    for(int t=0;t<100;t++){
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(int i=0;i<numParticles;i++){//executing random walks
            randomWalk(i,(double)(t+1)/100.0*totalLength);//executing a random walk for 1/10th of the total length.
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
        MSD[0]*=(segWS->voxelLength*segWS->voxelLength/walkerCount);
        MSD[1]*=(segWS->voxelLength*segWS->voxelLength/walkerCount);
        MSD[2]*=(segWS->voxelLength*segWS->voxelLength/walkerCount);

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

        segWS->printer->passData("mean square displacement", pltData);

    }
}

void TortuosityParticles::computeTotalLength(){
    if(std::abs(totalLength)<1){
        totalLength=std::max(segWS->X(), segWS->Y());
        totalLength=100*std::max(totalLength,(double)segWS->Z()); //setting totalDistance to 1x the longest parameter
    }
}

void TortuosityParticles::computeMeanInterceptLength() {
    //Determining the mean intercept length, which has to be done in the Knudsen regime
    int b_numParticles=numParticles;//backing up the inputs
    double b_totalLength=totalLength;
    double b_meanFreePath=meanFreePath;

    walkers.clear();
    numParticles=5000;
    totalLength=100.*std::max(std::max(segWS->X(), segWS->Y()),segWS->Z());
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


bool TortuosityParticles::generateWalkers(){
    walkers.resize(numParticles);
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(int i=0;i<(int)walkers.size();i++){
        walkers[i].engine.seed((i+1)*randomSeed);

        double val = -1;
        while(val != voidValue){ //making sure that the walker is placed within the voids
            walkers[i].xPos = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*(X-1);
            walkers[i].yPos = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*(Y-1);
            walkers[i].zPos = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*(Z-1);
            while(walkers[i].xPos==(int)walkers[i].xPos||walkers[i].yPos==(int)walkers[i].yPos||walkers[i].zPos==(int)walkers[i].zPos){
                walkers[i].xPos = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*(X-1);
                walkers[i].yPos = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*(Y-1);
                walkers[i].zPos = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8)*(Z-1);
            }
            val =  segWS->operator ()((int)walkers[i].xPos, (int)walkers[i].yPos, (int)walkers[i].zPos);
        }

        walkers[i].startX = walkers[i].xPos;
        walkers[i].startY = walkers[i].yPos;
        walkers[i].startZ = walkers[i].zPos;

        walkers[i].xVox=(int)walkers[i].xPos;
        walkers[i].yVox=(int)walkers[i].yPos;
        walkers[i].zVox=(int)walkers[i].zPos;
        newDirParticleCollision(i);

        walkers[i].walkTime=0;
        walkers[i].deltaRw=0;
        walkers[i].collisionCount=0;
        walkers[i].velocity=meanVelocity;
        walkers[i].skip=false;
    }
    return true;
}

bool TortuosityParticles::randomWalk(int i, double walkLength){
    std::vector<double> nextPos(4);//calculating the next position of the walk at the closest face
    int voxelX,voxelY,voxelZ;//calculating the next voxel that the particle is going to enter
    char intersection = 0; //telling you whether the grid cell colission is in the X,Y, or Z direction
    double val; // grayscale value in the next voxel to be entered

    if(walkers[i].skip){ //Doesn't happen anymore in the code, leftover from when there was a numerical error. Just Ignore this
        return true;
    }


    // total time of walk = (walkLength*segWS->voxelLength/meanVelocity)
    // this is saying to loop though this until the walkers walk time reaches the desired length of a random walk
    while(walkers[i].walkTime<(walkLength*segWS->voxelLength/meanVelocity)){

        if(walkers[i].skip){// again, ignore this piece
            break;
        }

        bool stepping = true; //is the walker still moving
        bool collision=false; //did a collision occur

        //adding the walk time of this latest walk to the overall walk time of the walker
        walkers[i].walkTime-=((walkers[i].deltaRw*segWS->voxelLength)/walkers[i].velocity);

        while(stepping){//while the walker is continuing it's walk
            double Xo=walkers[i].xPos; //initial xyz position of the walker for this walk
            double Yo=walkers[i].yPos;
            double Zo=walkers[i].zPos;

            //nextPos[0] contains the x location of the next intersection
            //nextPos[1] contains the y location of the next intersection
            //nextPos[2] contains the z location of the next intersection
            //nextPos[3] specifies on which face the walker intersected (0=x, 1=y, 2=z)
            nextPos=nextEdgeIntersection(i);//finding the next edge intersection along the walk
            if(nextPos[3]==0){intersection='x';}
            else if(nextPos[3]==1){intersection='y';}
            else if(nextPos[3]==2){intersection='z';}

            //if this walk length was longer the distance to the next particle-particle collision
            if(walkers[i].deltaRw + sqrt((walkers[i].xPos-nextPos[0])*(walkers[i].xPos-nextPos[0])+(walkers[i].yPos-nextPos[1])*(walkers[i].yPos-nextPos[1])+(walkers[i].zPos-nextPos[2])*(walkers[i].zPos-nextPos[2])) > walkers[i].deltaR){
                //interpolating to the collision location
                walkers[i].xPos += (walkers[i].deltaR-walkers[i].deltaRw)*walkers[i].xDir;//interpolating the end point within the voxel
                walkers[i].yPos += (walkers[i].deltaR-walkers[i].deltaRw)*walkers[i].yDir;
                walkers[i].zPos += (walkers[i].deltaR-walkers[i].deltaRw)*walkers[i].zDir;
                walkers[i].deltaRw += sqrt((walkers[i].xPos-Xo)*(walkers[i].xPos-Xo)+(walkers[i].yPos-Yo)*(walkers[i].yPos-Yo)+(walkers[i].zPos-Zo)*(walkers[i].zPos-Zo));
                walkers[i].walkTime+=((walkers[i].deltaRw*segWS->voxelLength)/walkers[i].velocity);


                //setting the new direction after the particle-particle collision
                newDirParticleCollision(i);
                walkers[i].deltaRw = 0;

                stepping=false;

            }else{ // if no particle-particle collision happened in the traversed cell

                //setting the walker position to the intersection point with the next gridcell
                walkers[i].xPos=nextPos[0];
                walkers[i].yPos=nextPos[1];
                walkers[i].zPos=nextPos[2];

                //updating the total distance travelled
                walkers[i].deltaRw+=sqrt((walkers[i].xPos-Xo)*(walkers[i].xPos-Xo)+(walkers[i].yPos-Yo)*(walkers[i].yPos-Yo)+(walkers[i].zPos-Zo)*(walkers[i].zPos-Zo));

                switch(intersection){//determining the grayscale value at the next voxel along the walker's path
                    case 'x': //if the intersection happened on the x face

                        //setting the new voxel position of the walker
                        if(walkers[i].xDir>0) voxelX=walkers[i].xVox+1;
                        else voxelX=walkers[i].xVox-1;

                        // computing the value at the new voxel
                        val = grayAt_Symmetric(voxelX, walkers[i].yVox, walkers[i].zVox);

                        if(val != voidValue){ //if the new voxel is void
                            //executing a surface collision
                            newDirSurfaceCollision(i, intersection);

                            //setting the new walk time
                            walkers[i].walkTime+=(walkers[i].deltaRw*segWS->voxelLength/walkers[i].velocity);

                            //reseting the distance since last collision
                            walkers[i].deltaRw = 0;

                            stepping=false;
                            collision=true;
                            walkers[i].collisionCount++;
                        }else{
                            //assigning the calculated voxel position to the walker
                            walkers[i].xVox=voxelX;
                        }
                        break;
                    case 'y'://if the intersection happened on the y face
                        //see comments from case 'x'
                        if(walkers[i].yDir>0) voxelY=walkers[i].yVox+1;
                        else voxelY=walkers[i].yVox-1;
                        val = grayAt_Symmetric(walkers[i].xVox, voxelY, walkers[i].zVox);
                        if(val != voidValue){
                            newDirSurfaceCollision(i, intersection);

                            walkers[i].walkTime+=((walkers[i].deltaRw*segWS->voxelLength)/walkers[i].velocity);
                            walkers[i].deltaRw = 0;

                            stepping=false;
                            collision=true;
                            walkers[i].collisionCount++;
                        }else{
                            walkers[i].yVox=voxelY;
                        }
                        break;
                    case 'z'://if the intersection happened on the z face
                        //see comments from case 'y'
                        if(walkers[i].zDir>0) voxelZ=walkers[i].zVox+1;
                        else voxelZ=walkers[i].zVox-1;
                        val = grayAt_Symmetric(walkers[i].xVox, walkers[i].yVox, voxelZ);
                        if(val != voidValue){
                            newDirSurfaceCollision(i, intersection);

                            walkers[i].walkTime+=((walkers[i].deltaRw*segWS->voxelLength)/walkers[i].velocity);
                            walkers[i].deltaRw = 0;

                            collision=true;
                            stepping=false;
                            walkers[i].collisionCount++;

                        }else{
                            walkers[i].zVox=voxelZ;
                        }
                        break;
                    default:
                        return false;
                }
            }

            //if the new position was beyond the end of the total walk, then we interpolate to the middle location
            if(!collision&&(walkers[i].walkTime+(walkers[i].deltaRw*segWS->voxelLength/meanVelocity))>(walkLength*segWS->voxelLength/meanVelocity)){
                walkers[i].walkTime+=((walkers[i].deltaRw*segWS->voxelLength)/walkers[i].velocity);
                stepping=false;
            }
        }
    }
    return true;
}

double TortuosityParticles::grayAt_Symmetric(int xVox, int yVox, int zVox){

    while((xVox<0||xVox>X-1)){
        if(xVox<0) xVox =-xVox-1;
        else if(xVox>X-1) xVox=(X-1)-(xVox-X);
    }
    while((yVox<0||yVox>Y-1)){
        if(yVox<0) yVox=-yVox-1;
        else if(yVox>Y-1) yVox=(Y-1)-(yVox-Y);
    }
    while((zVox<0||zVox>Z-1)){
        if(zVox<0) zVox=-zVox-1;
        else if(zVox>Z-1) zVox=(Z-1)-(zVox-Z);
    }

    return segWS->operator ()(xVox,yVox,zVox);
}

//double TortuosityParticles::grayAt_Periodic(int xVox, int yVox, int zVox){
//
//    while((xVox<0||xVox>X-1)){
//        if(xVox<0) xVox = (X)+xVox;
//        else if(xVox>X-1) xVox=xVox-X;
//    }
//    while((yVox<0||yVox>Y-1)){
//        if(yVox<0) yVox = (Y)+yVox;
//        else if(yVox>Y-1) yVox=yVox-Y;
//    }
//    while((zVox<0||zVox>Z-1)){
//        if(zVox<0) zVox = (Z)+zVox;
//        else if(zVox>Z-1) zVox=zVox-Z;
//    }
//
//    return segWS->operator ()(xVox,yVox,zVox);
//}


bool TortuosityParticles::newDirParticleCollision(int i){

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

    walkers[i].deltaR = -log(std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8))*meanFreePath;

    if(walkers[i].xDir==0||walkers[i].xDir==-0||walkers[i].yDir==0||walkers[i].yDir==-0||walkers[i].zDir==0||walkers[i].zDir==-0){
        newDirParticleCollision(i);
    }

    return true;
}

bool TortuosityParticles::newDirSurfaceCollision(int i, char dir){

    double xDirOriginal=walkers[i].xDir;
    double yDirOriginal=walkers[i].yDir;
    double zDirOriginal=walkers[i].zDir;

    walkers[i].xDir=xDirOriginal;
    walkers[i].yDir=yDirOriginal;
    walkers[i].zDir=zDirOriginal;


    double pi = atan(1)*4; //This is the same as pi by machine precision. (it underestimates by ~1.3932677 * 10^-37)

    double rand1 = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8);
    double rand2 = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8);
    double rand3 = std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8);

    std::vector<double> surfaceNorm(3);
    switch(dir){
        case 'x':
            if(walkers[i].xDir>=0){surfaceNorm[0]=-1;surfaceNorm[1]=0;surfaceNorm[2]=0;}
            else{surfaceNorm[0]=1;surfaceNorm[1]=0;surfaceNorm[2]=0;}
            break;
        case 'y':
            if(walkers[i].yDir>=0){surfaceNorm[0]=0;surfaceNorm[1]=-1;surfaceNorm[2]=0;}
            else{surfaceNorm[0]=0;surfaceNorm[1]=1;surfaceNorm[2]=0;}
            break;
        case 'z':
            if(walkers[i].zDir>=0){surfaceNorm[0]=0;surfaceNorm[1]=0;surfaceNorm[2]=-1;}
            else{surfaceNorm[0]=0;surfaceNorm[1]=0;surfaceNorm[2]=1;}
            break;
        default:
            return false;
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
    walkers[i].deltaR = -log(std::max((double)walkers[i].engine()/sitmo::prng_engine::max(),1e-8))*meanFreePath;

    if(walkers[i].xDir==0||walkers[i].xDir==-0||walkers[i].yDir==0||walkers[i].yDir==-0||walkers[i].zDir==0||walkers[i].zDir==-0){
        newDirSurfaceCollision(i,dir);
    }

    return true;
}

std::vector<double> TortuosityParticles::nextEdgeIntersection(int i){
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

bool TortuosityParticles::logInput() {
    puma::Logger *logger = segWS->log;

    logger->appendLogSection("Compute Tortuosity");
    logger->appendLogLine(" -- Inputs:");
    logger->appendLogItem("interior Size: ");
    logger->appendLogItem(segWS->X());
    logger->appendLogItem(", ");
    logger->appendLogItem(segWS->Y());
    logger->appendLogItem(", ");
    logger->appendLogItem(segWS->Z());
    logger->newLine();

    logger->appendLogItem("Void Value: ");
    logger->appendLogItem(voidValue);
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

bool TortuosityParticles::logOutput() {
    puma::Logger *logger = segWS->log;

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

bool TortuosityParticles::errorCheck(std::string *errorMessage) {
    if(segWS->matrix.size() == 0) {
        *errorMessage = "Empty Material Matrix";
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
