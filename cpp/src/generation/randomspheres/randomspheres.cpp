#include "randomspheres.h"


//if you want the result to be continuous (non-thresholded)
bool puma::generateRandomSpheres(puma::Workspace *sphereWS, RandomSpheresInput input) {

    Generate_RandomSpheres generator(sphereWS,input);
    return generator.generate();
}


//if you want the result stored in a material workspace (discrete, thresholded)
bool puma::generateRandomSpheres(puma::Workspace *mat, RandomSpheresInput input, short val) {

    puma::Workspace sphereWS(mat->voxelLength,mat->log);
    Generate_RandomSpheres generator(&sphereWS,input);

    bool success = generator.generate();
    if(!success){
        return false;
    }

    mat->resize(sphereWS.X(),sphereWS.Y(),sphereWS.Z());
    mat->setMaterialID(&sphereWS,puma::Cutoff(128,255),val);

    return true;
}


Generate_RandomSpheres::Generate_RandomSpheres(puma::Workspace *sphereWS, RandomSpheresInput input) {
    this->sphereWS = sphereWS;
    this->input = input;
}


bool Generate_RandomSpheres::generate() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed
        std::cout << "Error in Sphere Generation: " << errorMessage << std::endl;
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
double Generate_RandomSpheres::approximateNumSpheres(double porosity) {
    double volume = input.xSize*input.ySize*input.zSize;
    double poroRemaining = (porosity-input.poro);
    double avgSphereVolume = 0;

    Sphere sphere;
    avgSphereVolume = sphere.approximateVolume(input);

    double numSpheres  = volume * poroRemaining / avgSphereVolume;

    return numSpheres;
}


bool Generate_RandomSpheres::fibersGenerationHelper() {

    // decides what percent of the numFibersLeft are generated before:
    // 1. computing the porosity again
    // 2. reApproximating the number of fibers left
    //
    // if radius variation == 0, then you make 90.9% of numFibersLeft
    // if radius variation is 50% of avgRadius, then you make 62.5% of numFibersLeft
    double divisionParameter=1.1+input.dRadius/input.avgRadius;

    double porosity=1; //local(this iteration, not local in space) porosity value

    //approximate number of fibers needed to reach the porosity. Assuming no intersection
    double numSpheresLeft= approximateNumSpheres(porosity);
    if(input.print) { std::cout << "Approximate number of spheres to be generated: " << numSpheresLeft << std::endl; }

    sphereWS->setSize(input.xSize,input.ySize,input.zSize);

    int iteration=0;//total iterations

    int numSpheresGenerated=0; // number of fibers that have been generated
    int localSpheresGenerated=0;

    sphere = new Sphere();

    int countSinceSphere=0;//number of attempts since last successful generated fiber (only applies if fibers aren't allowed to intersect)

    engine.seed(input.randomSeed);    //telling the random number generator where to start in the random number table (called the seed)

    //while the porosity hasn't reached the desired porosity yet
    while(porosity>input.poro){

        //means something has gone wrong. this happens if you don't allow intersections and the porosity is too low
        if(countSinceSphere>1000000){
            std::cout << "Unable to generate material, too many failed iterations. Likely due to low porosity and no-intersections allowed" << std::endl;
            return false;
        }

        // generating random parameters (with defined parameter space) for a new sphere
        sphere->randomParameters(input,&engine);

        if(!input.intersect){ //if the user specified that fibers should not intersect
            bool intersected = sphere->doesSphereIntersect(sphereWS); //check to see if the fiber intersects with an existing fiber

            if(!intersected){ // if the fibers did not intersect
                sphere->addSphereToDomain(sphereWS); //add the test fiber to the domain
                iteration++;
                numSpheresGenerated++;
                localSpheresGenerated++;
                countSinceSphere=0; //reset the failed counter
                if(input.print) {
                    if(numSpheresGenerated == 1){
                        std::cout << '\r' << numSpheresGenerated << " Sphere generated" << std::flush;
                    } else {
                        std::cout << '\r' << numSpheresGenerated << " Spheres generated" << std::flush;
                    }
                }

            } else {
                countSinceSphere++; //counting the number of times that a fiber tried to generate but intersected
            }

        }else{
            sphere->addSphereToDomain(sphereWS);
            iteration++;
            numSpheresGenerated++;
            localSpheresGenerated++;
            if(input.print) {
                if(numSpheresGenerated == 1){
                    std::cout << '\r' << numSpheresGenerated << " Sphere generated" << std::flush;
                } else {
                    std::cout << '\r' << numSpheresGenerated << " Spheres generated" << std::flush;
                }
            }
        }

        if(localSpheresGenerated > numSpheresLeft/divisionParameter){
            porosity = puma::compute_VolumeFraction(sphereWS,puma::Cutoff(0,127));
            numSpheresLeft=approximateNumSpheres(porosity);
            localSpheresGenerated = 0;
            std::cout << "spheres left: " << numSpheresLeft << std::endl;
            std::cout << "Porosity: " << porosity << std::endl;
        }
    }
    if(input.print) { std::cout << "  ---  Porosity: " << porosity << std::endl; }

    return true;
}


bool Generate_RandomSpheres::errorCheck(std::string *errorMessage) {

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

    return true;
}


bool Generate_RandomSpheres::logInput() {
    puma::Logger *log;
    log = sphereWS->log;

    (*log).appendLogSection("Generate Random Sphere Structure");
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
    (*log).newLine();
    (*log).appendLogItem("Intersect: ");
    (*log).appendLogItem(input.intersect);
    (*log).newLine();
    (*log).appendLogItem("Porosity: ");
    (*log).appendLogItem(input.poro);
    (*log).newLine();
    (*log).appendLogItem("Random Seed: ");
    (*log).appendLogItem(input.randomSeed);
    (*log).newLine();

    (*log).writeLog();

    return true;
}


bool Generate_RandomSpheres::logOutput() {
    puma::Logger *log;
    log = sphereWS->log;

    (*log).appendLogItem("Successful Random Sphere Generation");
    (*log).newLine();
    (*log).appendLogItem("Time: ");
    (*log).appendLogItem(puma::Logger::getTime());
    (*log).newLine();

    (*log).writeLog();

    return true;
}
