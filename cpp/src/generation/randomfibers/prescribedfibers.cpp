#include "prescribedfibers.h"


//if you want the result to be continuous (non-thresholded)
bool puma::generatePrescribedFibers(puma::Workspace *work, std::vector< double > *positions, double radius, double scale) {

    Generate_PrescribedFibers generator(work,nullptr,positions,radius,scale);
    return generator.generate();
}

//if you want the result stored in a material workspace (discrete, thresholded)
bool puma::generatePrescribedFibers(puma::Workspace *mat, std::vector< double > *positions, double radius , double scale, short val) {

    puma::Workspace work(mat->voxelLength);
    Generate_PrescribedFibers generator(&work,nullptr,positions,radius,scale);

    bool success = generator.generate();
    if(!success){
        return false;
    }

    mat->resize(work.X(),work.Y(),work.Z());
    mat->setMaterialID(&work,puma::Cutoff(128,255),val);

    return true;
}

// with Direction matrix
//if you want the result to be continuous (non-thresholded)
bool puma::generatePrescribedFibers(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, std::vector< double > *positions, double radius, double scale) {

    Generate_PrescribedFibers generator(work,dirMatrix,positions,radius,scale);
    return generator.generate();
}

//if you want the result stored in a material workspace (discrete, thresholded)
bool puma::generatePrescribedFibers(puma::Workspace *mat, puma::MatVec3<double> *dirMatrix, std::vector< double > *positions, double radius , double scale, short val) {

    puma::Workspace work(mat->voxelLength);
    Generate_PrescribedFibers generator(&work,dirMatrix,positions,radius,scale);

    bool success = generator.generate();
    if(!success){
        return false;
    }

    mat->resize(work.X(),work.Y(),work.Z());
    mat->setMaterialID(&work,puma::Cutoff(128,255),val);

    return true;
}


Generate_PrescribedFibers::Generate_PrescribedFibers(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, std::vector< double > *positions, double radius, double scale) {
    this->work = work;
    this->positions = positions;
    this->radius = radius;
    this->scale = scale;

    this->dirMatrix = dirMatrix;
}


bool Generate_PrescribedFibers::generate() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed
        std::cout << "Error in Weave Generation: " << errorMessage << std::endl;
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


bool Generate_PrescribedFibers::normalizeScale() {
    int size = positions->size();

    double maxX=-1e10;
    double maxY=-1e10;
    double maxZ=-1e10;
    double minX=1e10;
    double minY=1e10;
    double minZ=1e10;

    for(int i=0;i<size;i+=3){
        if(positions->at(i) < minX ) minX = positions->at(i);
        if(positions->at(i) > maxX ) maxX = positions->at(i);
    }
    for(int i=1;i<size;i+=3){
        if(positions->at(i) < minY ) minY = positions->at(i);
        if(positions->at(i) > maxY ) maxY = positions->at(i);
    }
    for(int i=2;i<size;i+=3){
        if(positions->at(i) < minZ ) minZ = positions->at(i);
        if(positions->at(i) > maxZ ) maxZ = positions->at(i);
    }

    std::cout << "Min: " << minX << " " << minY << " " << minZ << std::endl;
    std::cout << "Max: " << maxX << " " << maxY << " " << maxZ << std::endl;

    for(int i=0;i<size;i+=3){
        positions->at(i) -= minX;
        positions->at(i) *= scale;
    }
    for(int i=1;i<size;i+=3){
        positions->at(i) -= minY;
        positions->at(i) *= scale;
    }
    for(int i=2;i<size;i+=3){
        positions->at(i) -= minZ;
        positions->at(i) *= scale;
    }

    maxX=-1e10;
    maxY=-1e10;
    maxZ=-1e10;
    minX=1e10;
    minY=1e10;
    minZ=1e10;

    for(int i=0;i<size;i+=3){
        if(positions->at(i) < minX ) minX = positions->at(i);
        if(positions->at(i) > maxX ) maxX = positions->at(i);
    }
    for(int i=1;i<size;i+=3){
        if(positions->at(i) < minY ) minY = positions->at(i);
        if(positions->at(i) > maxY ) maxY = positions->at(i);
    }
    for(int i=2;i<size;i+=3){
        if(positions->at(i) < minZ ) minZ = positions->at(i);
        if(positions->at(i) > maxZ ) maxZ = positions->at(i);
    }

    std::cout << "Min: " << minX << " " << minY << " " << minZ << std::endl;
    std::cout << "Max: " << maxX << " " << maxY << " " << maxZ << std::endl;


    radius = radius * scale;

    buffer = (int) radius + 2; //in voxels

    int sizeX = maxX + 2 * buffer;
    int sizeY = maxY + 2 * buffer;
    int sizeZ = maxZ + 2 * buffer;

    work->matrix.resize(sizeX,sizeY,sizeZ);
    work->matrix.set(0);
    if(dirMatrix){
        dirMatrix->resize(sizeX,sizeY,sizeZ);
    }


    return true;
}

bool Generate_PrescribedFibers::fibersGenerationHelper() {

    normalizeScale();

    int numFibers = positions->size()/6;
    int index=0;


    int counter=0;
    for(int i=0;i<numFibers;i++) {

        puma::Vec3<double> startPos,endPos;
        startPos.x = positions->at(index) + buffer; index++;
        startPos.y = positions->at(index) + buffer; index++;
        startPos.z = positions->at(index) + buffer; index++;
        endPos.x = positions->at(index) + buffer; index++;
        endPos.y = positions->at(index) + buffer; index++;
        endPos.z = positions->at(index) + buffer; index++;

        double length = sqrt((startPos.x-endPos.x)*(startPos.x-endPos.x)+(startPos.y-endPos.y)*(startPos.y-endPos.y)+(startPos.z-endPos.z)*(startPos.z-endPos.z));

        fiber.setValues(radius,length,startPos,endPos);
        fiber.addFiberToDomain(work, dirMatrix);

        counter++;

    }

    std::cout << "count: " << counter << std::endl;

    return true;
}


bool Generate_PrescribedFibers::errorCheck(std::string *errorMessage) {

    return true;
}

bool Generate_PrescribedFibers::logInput() {

    return true;
}

bool Generate_PrescribedFibers::logOutput() {

    return true;
}
