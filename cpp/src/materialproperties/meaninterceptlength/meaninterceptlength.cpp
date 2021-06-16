#include "meaninterceptlength.h"


puma::Vec3<double> puma::compute_MeanInterceptLength(puma::Workspace *grayWS, puma::Cutoff cutoff, int numThreads) {

    if(cutoff.first < -0.5 || cutoff.second > 32767 || cutoff.first > cutoff.second){  // maximum signed short value 32767
        std::cout << "Mean Intercept Length Error: Invalid Cutoff Ranges" << std::endl;
        return puma::Vec3<double>(-1,-1,-1);
    }

    MeanInterceptLength mil(grayWS,cutoff, numThreads);
    return mil.compute();
}


MeanInterceptLength::MeanInterceptLength(puma::Workspace *segWS, puma::Cutoff cutoff, int numThreads) {

    this->segWS = segWS;
    this->cutoff = cutoff;
    this->numThreads = numThreads;
}


puma::Vec3<double> MeanInterceptLength::compute() {

    logInput();
    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        std::cout << "Mean Intercept Length Error: " <<  errorMessage << std::endl;
        return puma::Vec3<double>(-1,-1,-1);
    }

    meanInterceptLengthHelper();

    logOutput();

    return meanInterceptLength;
}


void MeanInterceptLength::meanInterceptLengthHelper() {

    long totalCollisionsX = getNumCollisionsX();
    long totalCollisionsY = getNumCollisionsY();
    long totalCollisionsZ = getNumCollisionsZ();

    long numVoidCells = getNumVoidCells();

    //in the case of a fully-dense domain
    if(numVoidCells == 0){
        meanInterceptLength.x = 0;
        meanInterceptLength.y = 0;
        meanInterceptLength.z = 0;
    }
    else{
        if(totalCollisionsX == 0){
            meanInterceptLength.x = -1;
            std::cout << "Mean Intercept Length Warning: Infinite Mean Intercept Length in X Direction" << std::endl;
        }
        else{
            meanInterceptLength.x = (double)numVoidCells*segWS->voxelLength / (double)totalCollisionsX;
        }

        if(totalCollisionsY == 0){
            meanInterceptLength.y = -1;
            std::cout << "Mean Intercept Length Warning: Infinite Mean Intercept Length in Y Direction" << std::endl;
        }
        else{
            meanInterceptLength.y = (double)numVoidCells*segWS->voxelLength / (double)totalCollisionsY;
        }

        if(totalCollisionsZ == 0){
            meanInterceptLength.z = -1;
            std::cout << "Mean Intercept Length Warning: Infinite Mean Intercept Length in Z Direction" << std::endl;
        }
        else{
            meanInterceptLength.z = (double)numVoidCells*segWS->voxelLength / (double)totalCollisionsZ;
        }
    }
}

long MeanInterceptLength::getNumVoidCells() {

    long x = segWS->X();
    long y = segWS->Y();
    long z = segWS->Z();

    // Find Number of Void Cells
    long numVoidCells = 0;
    for(long i=0; i<x; i++) {
        for (long j = 0; j < y; j++) {
            for (long k = 0; k < z; k++) {
                if (segWS->matrix(i, j, k) <= cutoff.second && segWS->matrix(i, j, k) >= cutoff.first) {
                    numVoidCells++;
                }
            }
        }
    }

    return numVoidCells;
}


long MeanInterceptLength::getNumCollisionsX() {

    long x = segWS->X();
    long y = segWS->Y();
    long z = segWS->Z();

    // Number of Intersections in X Direction
    std::vector<int> numVoidToSolidX(x);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(long i=0; i<x-1; i++) {
        for (long j=0; j<y; j++) {
            for (long k=0; k<z; k++) {
                if(segWS->matrix(i,j,k) <= cutoff.second && segWS->matrix(i,j,k) >= cutoff.first) {
                    if(!(segWS->matrix(i+1,j,k) <= cutoff.second && segWS->matrix(i+1,j,k) >= cutoff.first)) {
                        numVoidToSolidX[i]++;
                    }
                }
            }
        }
    }
    long totalCollisionsX = 0;
    for(long l=0; l<x; l++){
        totalCollisionsX += numVoidToSolidX[l];
    }

    return totalCollisionsX;
}


long MeanInterceptLength::getNumCollisionsY() {

    long x = segWS->X();
    long y = segWS->Y();
    long z = segWS->Z();

    // Number of Intersections in Y Direction
    std::vector<int> numVoidToSolidY(y);
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (long j = 0; j < y - 1; j++) {
        for(long i=0; i<x; i++) {
            for (long k = 0; k < z; k++) {
                if (segWS->matrix(i, j, k) <= cutoff.second && segWS->matrix(i, j, k) >= cutoff.first) {
                    if (!(segWS->matrix(i, j + 1, k) <= cutoff.second &&
                          segWS->matrix(i, j + 1, k) >= cutoff.first)) {
                        numVoidToSolidY[j]++;
                    }
                }
            }
        }
    }
    long totalCollisionsY = 0;
    for(long l=0; l<y; l++){
        totalCollisionsY += numVoidToSolidY[l];
    }

    return totalCollisionsY;
}


long MeanInterceptLength::getNumCollisionsZ() {

    long x = segWS->X();
    long y = segWS->Y();
    long z = segWS->Z();

    // Number of Intersections in Z Direction
    std::vector<int> numVoidToSolidZ(z);
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (long k = 0; k < z - 1; k++) {
        for(long i=0; i<x; i++) {
            for (long j = 0; j < y; j++) {
                if (segWS->matrix(i, j, k) <= cutoff.second && segWS->matrix(i, j, k) >= cutoff.first) {
                    if (!(segWS->matrix(i, j, k + 1) <= cutoff.second &&
                          segWS->matrix(i, j, k + 1) >= cutoff.first)) {
                        numVoidToSolidZ[k]++;
                    }
                }
            }
        }
    }
    long totalCollisionsZ = 0;
    for(long l=0; l<z; l++){
        totalCollisionsZ += numVoidToSolidZ[l];
    }

    return totalCollisionsZ;
}


bool MeanInterceptLength::logInput() {
    puma::Logger *logger = segWS->log;

    logger->appendLogSection("Compute Mean Intercept Length");
    logger->appendLogLine(" -- Inputs:");
    logger->appendLogItem("Domain Size: ");

    logger->appendLogItem(segWS->X());
    logger->appendLogItem(", ");
    logger->appendLogItem(segWS->Y());
    logger->appendLogItem(", ");
    logger->appendLogItem(segWS->Z());
    logger->newLine();

    logger->appendLogItem("puma::Cutoff Values: ");
    logger->appendLogItem(cutoff.first);
    logger->appendLogItem(cutoff.second);

    logger->newLine();
    logger->writeLog();

    return true;
}

bool MeanInterceptLength::logOutput() {

    puma::Logger *logger = segWS->log;

    logger->appendLogLine(" -- Output:");
    logger->appendLogItem("Mean Intercept Length: ");
    logger->appendLogItem(meanInterceptLength.x);
    logger->appendLogItem(",");
    logger->appendLogItem(meanInterceptLength.y);
    logger->appendLogItem(",");
    logger->appendLogItem(meanInterceptLength.z);

    logger->newLine();
    logger->writeLog();

    return true;
}

bool MeanInterceptLength::errorCheck(std::string *errorMessage) {

    if(segWS->matrix.size() == 0) {
        *errorMessage = "Empty Material Matrix";
        return false;
    }

    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }

    return true;
}
