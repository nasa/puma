#include "meanfilter3d.h"


bool puma::filter_Mean3D(Workspace *work, int window_radius, int numThreads) {

    MeanFilter3D filter(work,window_radius,numThreads);
    return filter.execute();
}


MeanFilter3D::MeanFilter3D(puma::Workspace *work, int window_radius, int numThreads) {

    this->work = work;
    this->window_radius = window_radius;
    this->numThreads = numThreads;
}


bool MeanFilter3D::execute() {

    //execute function needs to do four things:
    // 1. log the inputs
    // 2. error check the inputs
    // 3. computation
    // 4. log the outputs

    //step 1. log the inputs
    logInput();

    //step 2. error check the inputs
    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        std::cout << "Mean 3d Filter Error: " <<  errorMessage << std::endl;
        return false;
    }

    //step 3. computation
    bool success = filterHelper();

    if(!success) {
        std::cout << "Unable to execute filter" << std::endl;
        return false;
    }

    //step 4. log the outputs
    logOutput();

    return true;
}


bool MeanFilter3D::filterHelper() {

    //creating copy matrix
    puma::Matrix<short> copyMatrix(work->X(),work->Y(),work->Z());

    //finding the mean value for every point in the 3d Workspace.
    //Assigning the mean value to the copyMatrix
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for( long i=0 ; i<work->X(); i++ ) {
        for( long j=0 ; j<work->Y(); j++ ) {
            for( long k=0 ; k<work->Z(); k++ ) {
                copyMatrix(i,j,k) = meanValue(i,j,k);
            }
        }
    }

    // since the filtered results are now stored in the copymatrix, they need to be
    // moved to the Workspace. This is done through the Workspace
    // copy function, which is already parallel
    work->matrix.copy(&copyMatrix);

    return true;
}


short MeanFilter3D::meanValue(long x, long y, long z) {

    // determing the appropriate window for the filter algorithm. If the window
    // goes outside the computational domain, the window is cut short.
    int xStart = 0;
    if(xStart < x-window_radius) {
        xStart = x-window_radius;
    }

    int yStart = 0;
    if(yStart < y-window_radius) {
        yStart = y-window_radius;
    }

    int zStart = 0;
    if(zStart < z-window_radius) {
        zStart = z-window_radius;
    }

    int xEnd = x+window_radius;
    if(xEnd > work->X()-1) {
        xEnd = work->X()-1;
    }

    int yEnd = y+window_radius;
    if(yEnd > work->Y()-1) {
        yEnd = work->Y()-1;
    }

    int zEnd = z+window_radius;
    if(zEnd > work->Z()-1) {
        zEnd = work->Z()-1;
    }

    // computing the size of the window in each direction
    int xSize = xEnd - xStart + 1;
    int ySize = yEnd - yStart + 1;
    int zSize = zEnd - zStart + 1;

    //Creating new variables for the average calculation
    int sum = 0;
    int count = xSize* ySize * zSize;

    //iterating through the window and adding all the elements into the totalValues
    for( long i=xStart; i<=xEnd; i++ ) {
        for( long j=yStart; j<=yEnd; j++ ) {
            for( long k=zStart; k<=zEnd; k++ ) {
                sum += work->matrix(i,j,k);
            }
        }
    }

    short mean = sum / count;

    return mean;
}


bool MeanFilter3D::logInput() {

    puma::Logger *logger = work->log;

    logger->appendLogSection("Execute Mean 3D Filter");
    logger->appendLogItem("Current Time: ");
    logger->appendLogItem(logger->getTime());
    logger->newLine();
    logger->appendLogLine(" -- Inputs:");
    logger->appendLogItem("Window Radius: ");
    logger->appendLogItem(window_radius);
    logger->newLine();

    logger->writeLog();

    return true;
}


bool MeanFilter3D::logOutput() {

    puma::Logger *logger = work->log;

    logger->appendLogLine("Successfully Executed Filter");
    logger->appendLogItem("Current Time: ");
    logger->appendLogItem(logger->getTime());
    logger->newLine();

    logger->writeLog();

    return true;
}


bool MeanFilter3D::errorCheck(std::string *errorMessage) {

    bool returnBool = true;
    *errorMessage = "";

    if(work->size() == 0) {
        (*errorMessage).append("Empty Grayscale Workspace\n");
        returnBool = false;
    }

    if(window_radius <= 0) {
        (*errorMessage).append("Invalid window radius, must be >= 0\n");
        returnBool = false;
    }
    else if(window_radius > work->size()) {
        (*errorMessage).append("Invalid window radius, must be smaller than the domain size. Check inputs\n");
        returnBool = false;
    }

    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }

    return returnBool;
}
