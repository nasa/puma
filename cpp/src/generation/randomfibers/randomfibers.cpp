#include "randomfibers.h"
#include <algorithm>


//if you want the result to be continuous (non-thresholded)
bool puma::generateRandomFibers(Workspace *work, RandomFibersInput input) {

    Generate_RandomFibers generator(work, nullptr,input);
    return generator.generate();
}

//if you want the result stored in a material workspace (discrete, thresholded)
bool puma::generateRandomFibers(Workspace *mat, RandomFibersInput input, short val) {

    Workspace work(mat->voxelLength,mat->log);
    Generate_RandomFibers generator(&work, nullptr,input);

    bool success = generator.generate();
    if(!success){
        return false;
    }

    mat->resize(work.X(),work.Y(),work.Z());
    mat->setMaterialID(&work,puma::Cutoff(128,255),val);

    return true;
}

//if you want the result to be continuous (non-thresholded)
bool puma::generateRandomFibers(Workspace *work, puma::MatVec3<double> *dirMatrix,  RandomFibersInput input) {

    Generate_RandomFibers generator(work,dirMatrix, input);
    return generator.generate();
}

//if you want the result stored in a material workspace (discrete, thresholded)
bool puma::generateRandomFibers(Workspace *mat, puma::MatVec3<double> *dirMatrix , RandomFibersInput input, short val) {

    Workspace work(mat->voxelLength, mat->log);
    Generate_RandomFibers generator(&work,dirMatrix, input);

    bool success = generator.generate();
    if(!success){
        return false;
    }

    mat->resize(work.X(),work.Y(),work.Z());
    mat->setMaterialID(&work,puma::Cutoff(128,255),val);

    return true;
}


Generate_RandomFibers::Generate_RandomFibers(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, RandomFibersInput input) {

    this->dirMatrix = dirMatrix;


    this->work = work;
    this->input = input;
}


bool Generate_RandomFibers::generate() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed
        std::cout << "Error in Fiber Generation: " << errorMessage << std::endl;
        return false;
    }

    bool success = fibersGenerationHelper();
    if(!success){
        return false;
    }

    if( !logOutput() ) {
        std::cout << "Failed to log Outputs" << std::endl;
        return false;
    }

    return true;
}

//approximates the number of fibers that need to be built in order to reach the desired porosity level
//takes the current porosity as an import. If the domain is currently empty, porosity should be 1
double Generate_RandomFibers::approximateNumFibers(double porosity) {
    double volume = input.xSize*input.ySize*input.zSize;
    double poroRemaining = (porosity-input.poro);

    double avgFiberVolume = 0;

    if(input.fiberType == 0) {
        StraightCircleFiber fiber;
        avgFiberVolume = fiber.approximateVolume(input);
    }
    else if(input.fiberType == 1) {
        CurvedCircleFiber fiber;
        avgFiberVolume = fiber.approximateVolume(input);
    }
    else if(input.fiberType == 2) {
        StraightFlowerFiber fiber;
        avgFiberVolume = fiber.approximateVolume(input);
    }
    else if(input.fiberType == 3) {
        CurvedFlowerFiber fiber;
        avgFiberVolume = fiber.approximateVolume(input);
    }

    double numFibers  = volume * poroRemaining / avgFiberVolume;

    return numFibers;
}


bool Generate_RandomFibers::fibersGenerationHelper() {

    // decides what percent of the numFibersLeft are generated before:
    // 1. computing the porosity again
    // 2. reApproximating the number of fibers left
    //
    // if radius variation == 0, then you make 90.9% of numFibersLeft
    // if radius variation is 50% of avgRadius, then you make 62.5% of numFibersLeft
    double divisionParameter=1.1+input.dRadius/input.avgRadius;

    double porosity=1; //local(this iteration, not local in space) porosity value

    //approximate number of fibers needed to reach the porosity. Assuming no intersection
    double numFibersLeft= approximateNumFibers(porosity);
    if(input.print) { std::cout << "Approximate number of fibers to be generated: " << numFibersLeft << std::endl; }

    work->resize(input.xSize,input.ySize,input.zSize);
    if(dirMatrix){
        dirMatrix->resize(input.xSize,input.ySize,input.zSize);
    }

    int iteration=0;//total iterations

    int numFibersGenerated=0; // number of fibers that have been generated

    //setting the fiber to the determined type
    switch(input.fiberType) {
    case 0 :
        fiber = new StraightCircleFiber();
        break;
    case 1 :
        fiber = new CurvedCircleFiber();
        break;
    case 2 :
        fiber = new StraightFlowerFiber();
        break;
    case 3 :
        fiber = new CurvedFlowerFiber();
        break;
    }

    int countSinceFiber=0;//number of attempts since last successful generated fiber (only applies if fibers aren't allowed to intersect)

    engine.seed(input.randomSeed);    //telling the random number generator where to start in the random number table (called the seed)

    //while the porosity hasn't reached the desired porosity yet
    while(porosity>input.poro){

        int id = numFibersGenerated + 2;
        if(!input.bindFibers) {
            id = -1;
        }

        //means something has gone wrong. this happens if you don't allow intersections and the porosity is too low
        if(countSinceFiber>1000000){
            std::cout << "Unable to generate material, too many failed iterations. Likely due to low porosity and no-intersections allowed" << std::endl;
            return false;
        }

        // generating random parameters (with defined parameter space) for a new fiber.
        switch(input.fiberType) {
        case 0 :
            fiber->randomParameters(input, &engine);
            break;
        case 1 :
            fiber->randomParameters(input, &engine);
            break;
        case 2 :
            fiber->randomParameters(input, &engine);
            break;
        case 3 :
            fiber->randomParameters(input, &engine);
            break;
        }

        if(!input.intersect){ //if the user specified that fibers should not intersect

            bool intersected = fiber->doesFiberIntersect(work, id); //check to see if the fiber intersects with an existing fiber

            if(!intersected){ // if the fibers did not intersect
                fiber->addFiberToDomain(work, dirMatrix, id); //add the test fiber to the domain
                iteration++;
                numFibersGenerated++;
                countSinceFiber=0; //reset the failed counter
                if(input.print) {
                    if(numFibersGenerated == 1){
                        std::cout << '\r' << numFibersGenerated << " Fiber generated" << std::flush;
                    } else {
                        std::cout << '\r' << numFibersGenerated << " Fibers generated" << std::flush;
                    }
                }

            } else {
                countSinceFiber++; //counting the number of times that a fiber tried to generate but intersected
            }

        }else{
            fiber->addFiberToDomain(work, dirMatrix, id);
            iteration++;
            numFibersGenerated++;
            if(input.print) {
                if(numFibersGenerated == 1){
                    std::cout << '\r' << numFibersGenerated << " Fiber generated" << std::flush;
                } else {
                    std::cout << '\r' << numFibersGenerated << " Fibers generated" << std::flush;
                }
            }
        }

        if(numFibersGenerated > numFibersLeft/divisionParameter){

            if(input.bindFibers) {
                porosity = puma::compute_VolumeFraction(work,puma::Cutoff(0,0));
            } else {
                porosity = puma::compute_VolumeFraction(work,puma::Cutoff(0,127));
            }

            numFibersLeft=approximateNumFibers(porosity);
        }
    }
    if(input.print) { std::cout << "  ---  Porosity: " << porosity << std::endl; }


    if(input.bindFibers) {
        Generate_RandomFibers::bindFibers();
    }


    return true;
}

bool Generate_RandomFibers::bindFibers() {

    std::cout << "New binder" << std::endl;

    puma::Matrix<short> idMap(input.xSize, input.ySize, input.zSize, 0); //used to store the ID of the neighboring fiber
    puma::Matrix<float> distanceMap(input.xSize, input.ySize, input.zSize, 0); //used to store the distance to that fiber

    double radius = input.binderRadius;

    for(int i2=0;i2<input.xSize;i2++) {
        for (int j2 = 0; j2 < input.ySize; j2++) {
            for (int k2 = 0; k2 < input.zSize; k2++) {

                if(work->at(i2,j2,k2) <= 1) {
                    continue;
                } else {

                    bool boundary = false;
                    if(i2>0 and work->at(i2-1,j2,k2) <=1){
                        boundary = true;
                    }
                    else if( j2>0 and work->at(i2,j2-1,k2) <=1 ) {
                        boundary = true;
                    }
                    else if( k2>0 and work->at(i2,j2,k2-1) <=1 ) {
                        boundary = true;
                    }
                    else if(i2<input.xSize-1 and work->at(i2+1,j2,k2) <=1){
                        boundary = true;
                    }
                    else if(j2<input.ySize-1 and work->at(i2,j2+1,k2) <=1){
                        boundary = true;
                    }
                    else if(k2<input.zSize-1 and work->at(i2,j2,k2+1) <=1){
                        boundary = true;
                    }

                    if (boundary == false) {
                        continue;
                    }
                }

                int iMin = std::max(i2 - radius,0.0);
                int iMax = std::min(i2 + radius, (double)input.xSize);
                int jMin = std::max(j2 - radius,0.0);
                int jMax = std::min(j2 + radius, (double)input.ySize);
                int kMin = std::max(k2 - radius,0.0);
                int kMax = std::min(k2 + radius, (double)input.zSize);

                for (int i = iMin; i < iMax; i++) {
                    for (int j = jMin; j < jMax; j++) {
                        for (int k = kMin; k < kMax; k++) {

                            float distance = sqrt((i - i2) * (i - i2) + (j - j2) * (j - j2) + (k - k2) * (k - k2));

                            if(distance < radius && work->at(i,j,k) <= 1 ) {

                                if(idMap(i, j, k) == 0 ) {
                                    idMap(i, j, k) = work->at(i2,j2,k2) ;
                                    distanceMap(i,j,k) = distance;
                                }
                                else if( idMap(i, j, k) == work->at(i2,j2,k2) ) {
                                    distanceMap(i,j,k) = std::min(distance, distanceMap(i,j,k));

                                }
                                else {

                                    double distance2 = distanceMap(i,j,k);

                                    if(distance2 + distance < radius) {
                                        work->matrix.at(i,j,k) = 1;
                                    }

                                }
                            }

                        }
                    }
                }
            }
        }
    }


    return true;
}


bool Generate_RandomFibers::errorCheck(std::string *errorMessage) {

    if(input.numThreads<=0 || input.numThreads>1000) {
        input.numThreads = omp_get_num_procs();
    }
    omp_set_num_threads(input.numThreads);

    if(input.randomSeed <= 0) {
        (*errorMessage) = "Invalid random Seed. Must be between 1 and 2^15";
        return false;
    }
    if(input.randomSeed >= pow(2,15)) {
        (*errorMessage) = "Invalid random Seed. Must be between 1 and 2^15";
        return false;
    }
    if(input.xSize <= 0) {
        (*errorMessage) = "xSize invalid. Must be >0";
        return false;
    }
    if(input.ySize <= 0) {
        (*errorMessage) = "ySize invalid. Must be >0";
        return false;
    }
    if(input.zSize <= 0) {
        (*errorMessage) = "zSize invalid. Must be >0";
        return false;
    }
    if(input.avgRadius <= 0) {
        (*errorMessage) = "avgRadius invalid. Must be >0";
        return false;
    }
    if(input.dRadius < 0) {
        (*errorMessage) = "dRadius invalid. Must be >=0";
        return false;
    }
    if(input.dRadius >= input.avgRadius) {
        (*errorMessage) = "dRadius invalid. Must be < avgRadius";
        return false;
    }
    if(input.avgLength <= 0) {
        (*errorMessage) = "avgLength invalid. Must be >0";
        return false;
    }
    if(input.dLength < 0) {
        (*errorMessage) = "dLength invalid. Must be >=0";
        return false;
    }
    if(input.dLength >= input.avgLength) {
        (*errorMessage) = "dLength invalid. Must be < avgLength";
        return false;
    }

    if((input.angleType == 1 ) && (input.angle_variability < 0 || input.angle_variability > 90)) {
        (*errorMessage) = "Invalid angle variability - should be between 0 and 90";
        return false;
    }

    if(input.angleType < 0 || input.angleType > 2) {
        (*errorMessage) = "Invalid angle type - should be 0 for isotropic, 1 for transverse isotropic, and 2 for 1d";
        return false;
    }

    if( (input.angleType == 1 || input.angleType == 2) && (input.var_direction < 0 || input.var_direction > 2)) {
        (*errorMessage) = "Invalid direction, should be 0 for x, 1 for y, 2 for z";
        return false;
    }

    if(input.poro < 0 || input.poro > 1) {
        (*errorMessage) = "poro invalid. Must be between 0 and 1";
        return false;
    }
    if(input.randomSeed == 0) {
        input.randomSeed = rand();
    }
    if(input.randomSeed < 0 || input.randomSeed >= INT32_MAX ) {
        (*errorMessage) = "Invalid Random seed. Must be between 0 and INT_MAX";
        return false;
    }
    if(input.fiberType <0 || input.fiberType > 3) {
        (*errorMessage) = "Invalid fiberType. 0 for straightCircle, 1 for curvedCircle, 2 for StraightFlower, 3 for CurvedFlower";
        return false;
    }

    if(input.hollow) {
        if(input.fractionOfHollowFibers < 0 || input.fractionOfHollowFibers > 1) {
            (*errorMessage) = "Invalid Fraction of Hollow Fibers. Must be >= 0 and <=1";
            return false;
        }
        if(input.avgHollowRadius <= 0) {
            (*errorMessage) = "Invalid avgHollowRadius. Must be > 0";
            return false;
        }
        if(input.avgHollowRadius >= input.avgRadius - input.dRadius) {
            (*errorMessage) = "Invalid avgHollowRadius. Must be < (avgRadius - dRadius)";
            return false;
        }
        if(input.dHollowRadius < 0) {
            (*errorMessage) = "dHollowRadius invalid. Must be >= 0";
            return false;
        }
        if(input.dHollowRadius >= input.avgHollowRadius) {
            (*errorMessage) = "dHollowRadius invalid. Must be < avgHollowRadius";
            return false;
        }
    }

    if(input.bindFibers && input.binderRadius <= 0 ) {
        (*errorMessage) = "invalid binder radius. Must be > 0 ";
        return false;
    }

    switch(input.fiberType) {
    case 1 :
        if(input.avgRadiusOfCurvature <= 0) {
            (*errorMessage) = "Invalid avgRadiusOfCurvature. Must be >= 0.";
            return false;
        }
        if(input.avgRadiusOfCurvature - input.dRadiusOfCurvature < ( (input.avgLength + input.dLength) / 2 ) ) {
            (*errorMessage) = "Invalid avgRadiusOfCurvature and deviation. Must be equal or higher than half of length ( (AvgLength + dLength)/2 ).";
            return false;
        }
        if(input.dRadiusOfCurvature < 0) {
            (*errorMessage) = "dRadiusOfCurvature invalid. Must be >=0";
            return false;
        }
        if(input.dRadiusOfCurvature >= input.avgRadiusOfCurvature) {
            (*errorMessage) = "dRadiusOfCurvature invalid. Must be < avgRadiusOfCurvature";
            return false;
        }
        if(input.accuracy <= 0) {
            (*errorMessage) = "Invalid accuracy. Must be > 0";
            return false;
        }
        break;
    case 2 :
        if(input.AvgSmallRadius <= 0) {
            (*errorMessage) = "Invalid avgSmallRadius. Must be > 0";
            return false;
        }
        if(input.AvgSmallRadius > 2*input.avgRadius) {
            (*errorMessage) = "Invalid avgSmallRadius. Must be less than two times the radius of the middle fiber";
            return false;
        }
        if(input.dSmallRadius < 0) {
            (*errorMessage) = "dSmallRadius invalid. Must be >=0";
            return false;
        }
        if(input.dSmallRadius >= input.AvgSmallRadius) {
            (*errorMessage) = "dSmallRadius invalid. Must be < avgSmallRadius";
            return false;
        }
        if(input.AvgNumSmallFibers <0) {
            (*errorMessage) = "Invalid avgNumSmallFibers. Must be >= 0";
            return false;
        }
        if(input.dNumSmallFibers < 0) {
            (*errorMessage) = "dNumSmallFibers invalid. Must be >= 0";
            return false;
        }
        if(input.dNumSmallFibers > input.AvgNumSmallFibers) {
            (*errorMessage) = "dNumSmallFibers invalid. Must be <= avgNumSmallFibers";
            return false;
        }
        if(input.dPlacement < 0) {
            (*errorMessage) = "dPlacement invalid. Must be >= 0";
            return false;
        }
        if(input.dPlacement > 360.0/(  std::max(2.0*(input.AvgNumSmallFibers-input.dNumSmallFibers), 1e-6 ) )) {
            (*errorMessage) = "dPlacement invalid. Must be <= 360 / (2*(AvgNumSmallFibers-dNumSmallFibers))";
            return false;
        }
        break;
    case 3 :
        if(input.avgRadiusOfCurvature <= 0) {
            (*errorMessage) = "Invalid avgRadiusOfCurvature. Must be >= 0.";
            return false;
        }
        if(input.avgRadiusOfCurvature - input.dRadiusOfCurvature < ( (input.avgLength + input.dLength) / 2 ) ) {
            (*errorMessage) = "Invalid avgRadiusOfCurvature and deviation. Must be equal or higher than half of length ( (AvgLength + dLength)/2 ).";
            return false;
        }
        if(input.dRadiusOfCurvature < 0) {
            (*errorMessage) = "dRadiusOfCurvature invalid. Must be >=0";
            return false;
        }
        if(input.dRadiusOfCurvature >= input.avgRadiusOfCurvature) {
            (*errorMessage) = "dRadiusOfCurvature invalid. Must be < avgRadiusOfCurvature";
            return false;
        }
        if(input.accuracy <= 0) {
            (*errorMessage) = "Invalid accuracy. Must be > 0";
            return false;
        }
        if(input.AvgSmallRadius <= 0) {
            (*errorMessage) = "Invalid avgSmallRadius. Must be > 0";
            return false;
        }
        if(input.AvgSmallRadius > 2*input.avgRadius) {
            (*errorMessage) = "Invalid avgSmallRadius. Must be less than two times the radius of the middle fiber";
            return false;
        }
        if(input.dSmallRadius < 0) {
            (*errorMessage) = "dSmallRadius invalid. Must be >=0";
            return false;
        }
        if(input.dSmallRadius >= input.AvgSmallRadius) {
            (*errorMessage) = "dSmallRadius invalid. Must be < avgSmallRadius";
            return false;
        }
        if(input.AvgNumSmallFibers <0) {
            (*errorMessage) = "Invalid avgNumSmallFibers. Must be >= 0";
            return false;
        }
        if(input.dNumSmallFibers < 0) {
            (*errorMessage) = "dNumSmallFibers invalid. Must be >= 0";
            return false;
        }
        if(input.dNumSmallFibers > input.AvgNumSmallFibers) {
            (*errorMessage) = "dNumSmallFibers invalid. Must be <= avgNumSmallFibers";
            return false;
        }
        if(input.dPlacement < 0) {
            (*errorMessage) = "dPlacement invalid. Must be >= 0";
            return false;
        }
        if(input.dPlacement > 360.0/(  std::max(2.0*(input.AvgNumSmallFibers-input.dNumSmallFibers), 1e-6 ) )) {
            (*errorMessage) = "dPlacement invalid. Must be <= 360 / (2*(AvgNumSmallFibers-dNumSmallFibers))";
            return false;
        }
        break;
    }

    return true;
}

bool Generate_RandomFibers::logInput() {
    puma::Logger *log;
    log = work->log;

    (*log).appendLogSection("Generate Random Fiber Structure");
    (*log).appendLogItem("Time: ");
    (*log).appendLogItem(puma::Logger::getTime());
    (*log).newLine();
    (*log).appendLogLine(" -- Inputs:");
    (*log).appendLogItem("Domain Size: ");
    (*log).appendLogItem(input.xSize);
    (*log).appendLogItem(", ");
    (*log).appendLogItem(input.ySize);
    (*log).appendLogItem(", ");
    (*log).appendLogItem(input.zSize);
    (*log).newLine();
    (*log).appendLogItem("Radius: ");
    (*log).appendLogItem(input.avgRadius);
    (*log).appendLogItem(" +- ");
    (*log).appendLogItem(input.dRadius);
    (*log).newLine();
    (*log).appendLogItem("Length: ");
    (*log).appendLogItem(input.avgLength);
    (*log).appendLogItem(" +- ");
    (*log).appendLogItem(input.dLength);
    (*log).newLine();
    (*log).appendLogItem("Angle Type: ");
    (*log).appendLogItem(input.angleType);
    (*log).newLine();
    (*log).appendLogItem("Angle Variability: ");
    (*log).appendLogItem(input.angle_variability);
    (*log).newLine();
    (*log).appendLogItem("Direction for variability: ");
    (*log).appendLogItem(input.var_direction);
    (*log).newLine();
    (*log).appendLogItem("Intersect: ");
    (*log).appendLogItem(input.intersect);
    (*log).newLine();
    (*log).appendLogItem("Porosity: ");
    (*log).appendLogItem(input.poro);
    (*log).newLine();
    (*log).appendLogItem("Random Seed: ");
    (*log).appendLogItem(input.randomSeed);
    if(input.bindFibers) {
        (*log).newLine();
        (*log).appendLogItem("Binder Radius: ");
        (*log).appendLogItem(input.binderRadius);
    }
    (*log).newLine();
    (*log).appendLogItem("Fiber Type: ");
    switch(input.fiberType) {
    case 0 :
        (*log).appendLogItem("StraightCircle fiber");
        break;
    case 1 :
        (*log).appendLogItem("CurvedCircle fiber");
        (*log).newLine();
        (*log).appendLogItem("RadiusOfCurvature: ");
        (*log).appendLogItem(input.avgRadiusOfCurvature);
        (*log).appendLogItem(" +- ");
        (*log).appendLogItem(input.dRadiusOfCurvature);
        (*log).newLine();
        (*log).appendLogItem("Curvature Accuracy: ");
        (*log).appendLogItem(input.accuracy);
        break;
    case 2 :
        (*log).appendLogItem("StraightFlower fiber");
        (*log).newLine();
        (*log).appendLogItem("Small Fiber Radius: ");
        (*log).appendLogItem(input.AvgSmallRadius);
        (*log).appendLogItem(" +- ");
        (*log).appendLogItem(input.dSmallRadius);
        (*log).newLine();
        (*log).appendLogItem("Number of Small Fibers: ");
        (*log).appendLogItem(input.AvgNumSmallFibers);
        (*log).appendLogItem(" +- ");
        (*log).appendLogItem(input.dNumSmallFibers);
        (*log).newLine();
        (*log).appendLogItem("Deviation in Placement: ");
        (*log).appendLogItem(input.dPlacement);
        (*log).newLine();
        (*log).appendLogItem("Hollow: ");
        (*log).appendLogItem(input.hollow);
        (*log).newLine();
        (*log).appendLogItem("Fraction of Hollow Fibers: ");
        (*log).appendLogItem(input.fractionOfHollowFibers);
        (*log).newLine();
        (*log).appendLogItem("Hollow Radius: ");
        (*log).appendLogItem(input.avgHollowRadius);
        (*log).appendLogItem(" +- ");
        (*log).appendLogItem(input.dHollowRadius);
        (*log).newLine();
        break;
    case 3 :
        (*log).appendLogItem("CurvedFlower fiber");
        (*log).newLine();
        (*log).appendLogItem("Small Fiber Radius: ");
        (*log).appendLogItem(input.AvgSmallRadius);
        (*log).appendLogItem(" +- ");
        (*log).appendLogItem(input.dSmallRadius);
        (*log).newLine();
        (*log).appendLogItem("Number of Small Fibers: ");
        (*log).appendLogItem(input.AvgNumSmallFibers);
        (*log).appendLogItem(" +- ");
        (*log).appendLogItem(input.dNumSmallFibers);
        (*log).newLine();
        (*log).appendLogItem("Deviation in Placement: ");
        (*log).appendLogItem(input.dPlacement);
        (*log).newLine();
        (*log).appendLogItem("Hollow: ");
        (*log).appendLogItem(input.hollow);
        (*log).newLine();
        (*log).appendLogItem("Fraction of Hollow Fibers: ");
        (*log).appendLogItem(input.fractionOfHollowFibers);
        (*log).newLine();
        (*log).appendLogItem("Hollow Radius: ");
        (*log).appendLogItem(input.avgHollowRadius);
        (*log).appendLogItem(" +- ");
        (*log).appendLogItem(input.dHollowRadius);
        (*log).newLine();
        (*log).appendLogItem("RadiusOfCurvature: ");
        (*log).appendLogItem(input.avgRadiusOfCurvature);
        (*log).appendLogItem(" +- ");
        (*log).appendLogItem(input.dRadiusOfCurvature);
        (*log).newLine();
        (*log).appendLogItem("Curvature Accuracy: ");
        (*log).appendLogItem(input.accuracy);
        break;
    }
    (*log).newLine();

    (*log).writeLog();

    return true;
}


bool Generate_RandomFibers::logOutput() {
    puma::Logger *log;
    log = work->log;

    (*log).appendLogItem("Successful Random Fiber Generation");
    (*log).newLine();
    (*log).appendLogItem("Time: ");
    (*log).appendLogItem(puma::Logger::getTime());
    (*log).newLine();

    (*log).writeLog();

    return true;
}
