#include "raycasting.h"


////// PARTICLE METHOD //////

// Functions to find longest direction for EVERY solid voxel

// With Refinement
bool puma::compute_orientationRC(puma::Workspace *grayWS, puma::Cutoff voidRange, int initAccuracy, int degreeAccuracy,
                                 puma::MatVec3<double> *direction, bool print, int numThreads) {
    if (voidRange.first < -0.5 || voidRange.second> 32767 || voidRange.first > voidRange.second) {
        std::cout << "Error: Invalid voidRange Ranges" << std::endl;
        return false;
    }
    puma::Workspace segWS(grayWS->getShape(), grayWS->log);
    segWS.setMaterialID(grayWS,voidRange,1); // selecting fibers

    RayCasting solver(&segWS,initAccuracy,degreeAccuracy,direction,print,numThreads);
    return solver.computeLongestDirection();
}

// Without Refinement
bool puma::compute_orientationRC(puma::Workspace *grayWS, puma::Cutoff voidRange, int degreeAccuracy,
                                 puma::MatVec3<double> *direction, bool print, int numThreads) {
    if (voidRange.first < -0.5 || voidRange.second> 32767 || voidRange.first > voidRange.second) {
        std::cout << "Error: Invalid voidRange Ranges" << std::endl;
        return false;
    }
    puma::Workspace segWS(grayWS->getShape(), grayWS->log);
    segWS.setMaterialID(grayWS,voidRange,1); // selecting fibers

    RayCasting solver(&segWS,0,degreeAccuracy,direction,print,numThreads);
    return solver.computeLongestDirection();
}


RayCasting::RayCasting(puma::Workspace *segWS, int initAccuracy,int degreeAccuracy,
                       puma::MatVec3<double> *direction, bool print, int numThreads){
    this->segWS = segWS;
    this->direction = direction;
    this->degreeAccuracy = degreeAccuracy;
    this->initAccuracy = initAccuracy;
    this->print = print;
    this->numThreads = numThreads;
    X = (int)segWS->X();
    Y = (int)segWS->Y();
    Z = (int)segWS->Z();

}


bool RayCasting::computeLongestDirection(){

    logInput();

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        std::cout << "Direction Particles Error: " <<  errorMessage << std::endl;
        return false;
    }

    directionSimulation();

    logOutput();

    return true;
}


// Simulation control
bool RayCasting::directionSimulation(){

    direction->resize(segWS->X(),segWS->Y(),segWS->Z(), puma::Vec3<double>(0.,0.,0.));

    bool refined = true;
    if(initAccuracy==0 || initAccuracy == degreeAccuracy) {
        refined = false;
        initAccuracy = degreeAccuracy;
    }

    assignWalkersDirection(); //assigning the initial direction vector for each walker

    for (int k=0; k<Z; k++){

        if(print) { std::cout << std::setprecision(3) << '\r' << ((double)(k+1)/(double)Z)*100 << "%" << std::flush; }

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (int i=0; i<X; i++){
            for (int j=0; j<Y; j++){
                if (segWS->matrix(i,j,k) != 0){
                    resetSphericalWalkers(puma::Vec3<int>(i,j,k),true, i);
                    executeSphericalWalks(true,i);

                    invertParticles(i);
                    resetSphericalWalkers(puma::Vec3<int>(i,j,k),true, i);

                    executeSphericalWalks(true,i);

                    direction->at(i,j,k) = determineDirection(i);

                    invertParticles(i);
                }
            }
        }
    }

    if(refined) {
        if(print) { std::cout << std::endl << "Begin Refinement" << std::endl; }

        for (int k=0; k<Z; k++){

            if(print) { std::cout << std::setprecision(3) << '\r' << ((double)(k+1)/(double)Z)*100 << "%" << std::flush; }

            omp_set_num_threads(numThreads);
#pragma omp parallel for
            for (int i=0; i<X; i++){
                for (int j=0; j<Y; j++){
                    if (segWS->matrix(i,j,k) != 0){
                        setSphericalWalkersRefined(puma::Vec3<int>(i,j,k), i);
                        resetSphericalWalkersRefined(puma::Vec3<int>(i,j,k),true, i);

                        executeSphericalWalks(true,i);

                        invertParticles(i);
                        resetSphericalWalkersRefined(puma::Vec3<int>(i,j,k),true, i);

                        executeSphericalWalks(true,i);

                        direction->at(i,j,k) = determineDirection(i);

                        invertParticles(i);
                    }
                }
            }
        }
    }

    return true;
}


void RayCasting::invertParticles(int ind){
    for(auto & i : sphericalWalkers[ind]){
        i.dir.x *= -1;
        i.dir.y *= -1;
        i.dir.z *= -1;

        i.invDistance = i.distance;

    }
}


// Assigning unit vector directions to each particle (only done once)
void RayCasting::assignWalkersDirection(){

    nParticles = (int)(180/initAccuracy-1)*(360/initAccuracy)+2;
    if(print) { std::cout << "Number of particles: " << nParticles << " for " << initAccuracy <<  "° Degree of Accuracy" << std::endl; }

    sphericalWalkers.resize(X); // create walkers
    for(int i=0;i<X;i++){
        sphericalWalkers[i].resize(nParticles);
    }

    for(int ind = 0; ind<X; ind ++) {

        sphericalWalkers[ind][0].dir.x = std::sin(0.) * std::cos(0.);
        sphericalWalkers[ind][0].dir.y = std::sin(0.) * std::sin(0.);
        sphericalWalkers[ind][0].dir.z = std::cos(0.);

        double theta(initAccuracy),phi(0);

        for(int i=1; i<nParticles-1; i++){

            // Convert theta and phi for each walker into Cartesian unit vectors
            sphericalWalkers[ind][i].dir.x = std::sin(theta*(M_PI/180.)) * std::cos(phi*(M_PI/180.));
            sphericalWalkers[ind][i].dir.y = std::sin(theta*(M_PI/180.)) * std::sin(phi*(M_PI/180.));
            sphericalWalkers[ind][i].dir.z = std::cos(theta*(M_PI/180.));

            theta += initAccuracy;

            if(theta>=180){
                theta = initAccuracy;
                phi += initAccuracy;
            }
        }

        sphericalWalkers[ind][nParticles-1].dir.x = std::sin(M_PI) * std::cos(2.*M_PI);
        sphericalWalkers[ind][nParticles-1].dir.y = std::sin(M_PI) * std::sin(2.*M_PI);
        sphericalWalkers[ind][nParticles-1].dir.z = std::cos(M_PI);


    }

    std::vector<int> maxAngle(sphericalWalkers[0].size(),0);

#pragma omp parallel for
    for(unsigned int i=0;i<sphericalWalkers[0].size();i++) {
        for(unsigned int j=0;j<sphericalWalkers[0].size();j++) {
            double theta = acos(sphericalWalkers[0][i].dir.x * sphericalWalkers[0][j].dir.x + sphericalWalkers[0][i].dir.y * sphericalWalkers[0][j].dir.y + sphericalWalkers[0][i].dir.z * sphericalWalkers[0][j].dir.z) * (180./M_PI);
            double phi = acos(sphericalWalkers[0][i].dir.x * (-sphericalWalkers[0][j].dir.x) + sphericalWalkers[0][i].dir.y * (-sphericalWalkers[0][j].dir.y) + sphericalWalkers[0][i].dir.z * (-sphericalWalkers[0][j].dir.z)) * (180./M_PI);

            double error = 0;

            if(theta>phi){ error = phi; }
            else { error = theta; }

            if(error > 80) {
                maxAngle[i]++;
            }

        }
    }
    if(print) { std::cout << "Directions for spherical walkers set." << std::endl; }
}


// Relocating the particles in the middle of the voxel at the beginning of each spherical walk
bool RayCasting::resetSphericalWalkers(const puma::Vec3<int>& vox, bool isDir, int ind){

    for(int i=0; i<nParticles; i++){


        // voxel space (not physical space)
        sphericalWalkers[ind][i].pos.x = vox.x + 0.5;
        sphericalWalkers[ind][i].pos.y = vox.y + 0.5;
        sphericalWalkers[ind][i].pos.z = vox.z + 0.5;

        // Assign voxel location
        sphericalWalkers[ind][i].vox.x = vox.x;
        sphericalWalkers[ind][i].vox.y = vox.y;
        sphericalWalkers[ind][i].vox.z = vox.z;


        sphericalWalkers[ind][i].start.x = sphericalWalkers[ind][i].pos.x;
        sphericalWalkers[ind][i].start.y = sphericalWalkers[ind][i].pos.y;
        sphericalWalkers[ind][i].start.z = sphericalWalkers[ind][i].pos.z;

        sphericalWalkers[ind][i].fiberValue = segWS->matrix(vox.x,vox.y,vox.z);

        sphericalWalkers[ind][i].intersection = false;
    }

    if(!isDir){
        for(int i=0; i<nParticles; i++){
            sphericalWalkers[ind][i].positions.clear();
            sphericalWalkers[ind][i].positions.push_back( sphericalWalkers[ind][i].pos );
        }
    }

    return true;
}


// Relocating the particles in the middle of the voxel at the beginning of each spherical walk
bool RayCasting::setSphericalWalkersRefined(const puma::Vec3<int>& vox, int ind){

    double x = direction->at(vox.x,vox.y,vox.z).x;
    double y = direction->at(vox.x,vox.y,vox.z).y;
    double z = direction->at(vox.x,vox.y,vox.z).z;

    double theta0 = std::acos(z)*180/M_PI;
    double phi0 = std::atan2(y,x)*180/M_PI;

    double startTheta = theta0 - initAccuracy;
    double endTheta = theta0 + initAccuracy;
    double startPhi = phi0 - initAccuracy;
    double endPhi = phi0 + initAccuracy;

    sphericalWalkers[ind].clear();
    int index=0;

    for(double theta = startTheta; theta < endTheta; theta = theta + degreeAccuracy) {
        for(double phi = startPhi; phi < endPhi; phi = phi + degreeAccuracy) {

            sphericalWalker walker;

            walker.dir.x = std::sin(theta*(M_PI/180.)) * std::cos(phi*(M_PI/180.));
            walker.dir.y = std::sin(theta*(M_PI/180.)) * std::sin(phi*(M_PI/180.));
            walker.dir.z = std::cos(theta*(M_PI/180.));
            index++;

            sphericalWalkers[ind].push_back(walker);
        }
    }
    return true;
}


// Relocating the particles in the middle of the voxel at the beginning of each spherical walk
bool RayCasting::resetSphericalWalkersRefined(const puma::Vec3<int>& vox, bool isDir, int ind){


    for(auto & i : sphericalWalkers[ind]){

        // voxel space (not physical space)
        i.pos.x = vox.x + 0.5;
        i.pos.y = vox.y + 0.5;
        i.pos.z = vox.z + 0.5;

        // Assign voxel location
        i.vox.x = vox.x;
        i.vox.y = vox.y;
        i.vox.z = vox.z;

        i.start.x = i.pos.x;
        i.start.y = i.pos.y;
        i.start.z = i.pos.z;

        i.fiberValue = segWS->matrix(vox.x,vox.y,vox.z);

        i.intersection = false;
    }

    if(!isDir){
        for(auto & i : sphericalWalkers[ind]){
            i.positions.clear();
            i.positions.push_back( i.pos );
        }
    }

    return true;
}


void RayCasting::executeSphericalWalks(bool isDir, int ind){


    for(unsigned int i=0; i<sphericalWalkers[ind].size(); i++){

        std::vector<double> nextPos(4); // calculating the next position of the walk at the closest face
        puma::Vec3<int> vox; // calculating the next voxel that the particle is going to enter
        char intersection; // telling you whether the grid cell collision is in the X,Y, or Z direction
        double val; // grayscale value in the next voxel to be entered

        // keep walking until an intersection
        double currentDistance = sphericalWalkers[ind][i].pos.distance(sphericalWalkers[ind][i].start);
        double distancevoidRange =  std::sqrt(3)* std::max( std::max(segWS->X(), segWS->Y() ), segWS->Z() ); // longest distance is major diagonal of sample
        while(!sphericalWalkers[ind][i].intersection && currentDistance < distancevoidRange){

            // nextPos[0-2] - x,y,z coordinates of next edge intersection
            // nextPos[3] - The edge at which the intersection occurs (x=0, y=1, z=2)
            nextPos = nextEdgeIntersection(i,ind);//finding the next edge intersection along the walk
            if(nextPos[3]==0){intersection='x';}
            else if(nextPos[3]==1){intersection = 'y';}
            else if(nextPos[3]==2){intersection = 'z';}

            //setting the sphericalWalker position to the intersection point with the next gridcell
            sphericalWalkers[ind][i].pos.x = nextPos[0];
            sphericalWalkers[ind][i].pos.y = nextPos[1];
            sphericalWalkers[ind][i].pos.z = nextPos[2];

            switch(intersection){ // determining the grayscale value at the next voxel along the sphericalWalker's path
                case 'x': // if the intersection happened on the x face

                    // setting the new voxel position of the sphericalWalker
                    if(sphericalWalkers[ind][i].dir.x>0) vox.x = sphericalWalkers[ind][i].vox.x+1;
                    else vox.x=sphericalWalkers[ind][i].vox.x-1;

                    // computing the value at the new voxel

                    val = grayAt(puma::Vec3<int>(vox.x, sphericalWalkers[ind][i].vox.y, sphericalWalkers[ind][i].vox.z));

                    if(val != sphericalWalkers[ind][i].fiberValue){ // if the new voxel is void or other fiber
                        //executing a surface collision (kill particle)
                        sphericalWalkers[ind][i].intersection = true;
                    }else{
                        //assigning the calculated voxel position to the sphericalWalker
                        sphericalWalkers[ind][i].vox.x = vox.x;
                    }
                    break;
                case 'y'://if the intersection happened on the y face
                    //see comments from case 'x'
                    if(sphericalWalkers[ind][i].dir.y>0) vox.y = sphericalWalkers[ind][i].vox.y+1;
                    else vox.y = sphericalWalkers[ind][i].vox.y-1;
                    val = grayAt(puma::Vec3<int>(sphericalWalkers[ind][i].vox.x, vox.y, sphericalWalkers[ind][i].vox.z));
                    if(val != sphericalWalkers[ind][i].fiberValue){
                        sphericalWalkers[ind][i].intersection = true;
                    }else{
                        sphericalWalkers[ind][i].vox.y = vox.y;
                    }
                    break;
                case 'z'://if the intersection happened on the z face
                    //see comments from case 'y'
                    if(sphericalWalkers[ind][i].dir.z>0) vox.z = sphericalWalkers[ind][i].vox.z+1;
                    else vox.z = sphericalWalkers[ind][i].vox.z-1;
                    val = grayAt(puma::Vec3<int>(sphericalWalkers[ind][i].vox.x, sphericalWalkers[ind][i].vox.y, vox.z));
                    if(val != sphericalWalkers[ind][i].fiberValue){
                        sphericalWalkers[ind][i].intersection = true;
                    }else{
                        sphericalWalkers[ind][i].vox.z = vox.z;
                    }
                    break;
            }

            if(!isDir){ sphericalWalkers[ind][i].positions.push_back( sphericalWalkers[ind][i].pos ); } // pushback for path export

            currentDistance = sphericalWalkers[ind][i].pos.distance(sphericalWalkers[ind][i].start);

        } //sphericalWalker collided with a wall

        if(!isDir){ sphericalWalkers[ind][i].positions.push_back( sphericalWalkers[ind][i].pos ); } // pushback for path export

        sphericalWalkers[ind][i].distance = sphericalWalkers[ind][i].pos.distance(sphericalWalkers[ind][i].start);
    }
}


// Returns x,y,z position of the next voxel intersection for a particle and which edge
std::vector<double> RayCasting::nextEdgeIntersection(int i, int ind){

    // nextPos[0-2] - x,y,z coordinates of next edge intersection
    // nextPos[3] - The edge at which the intersection occurs (x=0, y=1, z=2)
    std::vector<double> nextPos(4);
    int nX,nY,nZ; // for calculating the next intersecting face in each direction
    double tX,tY,tZ; // for calculating the time to intersecting face in each direction

    // Step 1: Setting nX, nY, and nZ to be the next interfaces reached for each direction
    if(sphericalWalkers[ind][i].dir.x>0){nX = sphericalWalkers[ind][i].vox.x+1;} else{nX = sphericalWalkers[ind][i].vox.x;}
    if(sphericalWalkers[ind][i].dir.y>0){nY = sphericalWalkers[ind][i].vox.y+1;} else{nY = sphericalWalkers[ind][i].vox.y;}
    if(sphericalWalkers[ind][i].dir.z>0){nZ = sphericalWalkers[ind][i].vox.z+1;} else{nZ = sphericalWalkers[ind][i].vox.z;}

    // Step 2: Settng tX, tY, and tZ to be the time to reach the next interface in each direction
    tX = (nX-sphericalWalkers[ind][i].pos.x)/sphericalWalkers[ind][i].dir.x; // time to get to x wall //not a real time
    tY = (nY-sphericalWalkers[ind][i].pos.y)/sphericalWalkers[ind][i].dir.y; // time to get to y wall
    tZ = (nZ-sphericalWalkers[ind][i].pos.z)/sphericalWalkers[ind][i].dir.z; // time to get to z wall

    // For the cases in which the angle is exactly 0 (preventing Inf or Nan)
    if(tX != tX || tX  == +1.0/0.0 || tX  == -1.0/0.0) tX = 1e10;
    if(tY != tY || tY  == +1.0/0.0 || tY  == -1.0/0.0) tY = 1e10;
    if(tZ != tZ || tZ  == +1.0/0.0 || tZ  == -1.0/0.0) tZ = 1e10;

    // Step 3: Setting the next position of the sphericalWalker based on the nearest interface
    if(tX<=tY && tX<=tZ){ // if the x wall is the closest
        nextPos[0] = nX;
        nextPos[1] = sphericalWalkers[ind][i].pos.y+sphericalWalkers[ind][i].dir.y*tX;
        nextPos[2] = sphericalWalkers[ind][i].pos.z+sphericalWalkers[ind][i].dir.z*tX;
        nextPos[3]=0;
    }else if(tY<=tZ && tY<=tX){ // if the y wall is the closest
        nextPos[0] = sphericalWalkers[ind][i].pos.x+sphericalWalkers[ind][i].dir.x*tY;
        nextPos[1] = nY;
        nextPos[2] = sphericalWalkers[ind][i].pos.z+sphericalWalkers[ind][i].dir.z*tY;
        nextPos[3]=1;
    }else{ // if the z wall is the closest
        nextPos[0] = sphericalWalkers[ind][i].pos.x+sphericalWalkers[ind][i].dir.x*tZ;
        nextPos[1] = sphericalWalkers[ind][i].pos.y+sphericalWalkers[ind][i].dir.y*tZ;
        nextPos[2] = nZ;
        nextPos[3]=2;
    }

    return nextPos;
}


puma::Vec3<double> RayCasting::determineDirection(int ind) {

    puma::Vec3<double> dir(0,0,0);

    double maxDistance = 0;
    for(auto & i : sphericalWalkers[ind]){
        if(i.distance + i.invDistance  > maxDistance){
            maxDistance = i.distance + i.invDistance;

            dir = i.dir;
        }
    }

    return dir;
}


double RayCasting::grayAt(const puma::Vec3<int>& vox){

    if((vox.x<0||vox.x>X-1)){
        return 0;
    }
    if((vox.y<0||vox.y>Y-1)){
        return 0;
    }
    if((vox.z<0||vox.z>Z-1)){
        return 0;
    }

    return segWS->operator ()(vox.x,vox.y,vox.z);
}


bool RayCasting::logInput() {
    puma::Logger *logger = segWS->log;

    logger->appendLogSection("Compute Orientation using Structure Tensor");
    logger->appendLogLine(" -- Inputs:");
    logger->appendLogItem("interior Size: ");

    logger->appendLogItem(segWS->X());
    logger->appendLogItem(", ");
    logger->appendLogItem(segWS->Y());
    logger->appendLogItem(", ");
    logger->appendLogItem(segWS->Z());
    logger->newLine();


    logger->appendLogItem("Ray angle separation: ");
    logger->appendLogItem("Initial angle: ");
    logger->appendLogItem(initAccuracy);
    logger->appendLogItem("Refinement angle: ");
    logger->appendLogItem(degreeAccuracy);
    logger->newLine();
    logger->writeLog();

    return true;
}


bool RayCasting::logOutput() {
    puma::Logger *logger = segWS->log;

    logger->appendLogItem("Completed Orientation computation using the Ray Casting (RC) method");

    logger->newLine();
    logger->writeLog();

    return true;
}


bool RayCasting::errorCheck(std::string *errorMessage) {
    if(segWS->matrix.size() == 0) {
        *errorMessage = "Empty Material Matrix";
        return false;
    }

    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }

    if(initAccuracy<0) {
        *errorMessage = "Initial Angle separation must be > 0";
        return false;
    }

    if(degreeAccuracy<=0) {
        if(initAccuracy == 0){
            *errorMessage = "Angle separation must be > 0";
        } else {
            *errorMessage = "Refined Angle separation must be > 0";
        }
        return false;
    }

    // degreeAccuracy can only be an exact divider of 360
    int check[] = {1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 15, 18, 20, 24, 30, 36, 40, 45, 60, 72, 90, 120, 180};
    if(std::find(std::begin(check), std::end(check), degreeAccuracy) == std::end(check)) {
        if(initAccuracy == 0){
            *errorMessage = "Ray separation can only be an exact divider of 360° \nChoose from:1,2,3,4,5,6,8,9,10,12,15,18,20,24,30,36,40,45,60,72,90,120,180";
        } else {
            *errorMessage = "Refined ray separation can only be an exact divider of 360° \nChoose from:1,2,3,4,5,6,8,9,10,12,15,18,20,24,30,36,40,45,60,72,90,120,180";
        }
        return false;
    }

    // initAccuracy can only be an exact divider of 360
    if(initAccuracy > 0){
        if(std::find(std::begin(check), std::end(check), initAccuracy) == std::end(check)) {
            *errorMessage = "Initial ray separation can only be an exact divider of 360° \nChoose from:1,2,3,4,5,6,8,9,10,12,15,18,20,24,30,36,40,45,60,72,90,120,180";
            return false;
        }
        if(initAccuracy < degreeAccuracy){
            *errorMessage = "Refined ray separation has to be < Initial one";
            return false;
        }
        if(initAccuracy % degreeAccuracy > 0){
            *errorMessage = "Refined ray separation has to be an exact divider of the Initial ray separation";
            return false;
        }
    }

    return true;
}

