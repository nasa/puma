#include "bilateralfilter.h"


bool puma::filter_Bilateral(puma::Workspace *work, int window_radius, double sigma_d, double sigma_r, int numThreads) {

    BilateralFilter filter(work,window_radius,sigma_d,sigma_r, numThreads);
    return filter.execute();
}


BilateralFilter::BilateralFilter(puma::Workspace *work, int window_radius, double sigma_d, double sigma_r, int numThreads) {

    this->work = work;
    this->window_radius = window_radius;
    this->sigma_d = sigma_d;
    this->sigma_r = sigma_r;
    this->numThreads = numThreads;
}


bool BilateralFilter::execute() {

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
        std::cout << "Bilateral Filter Error: " <<  errorMessage << std::endl;
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


bool BilateralFilter::filterHelper() {

    //creating copy matrix
    puma::Matrix<short> copyMatrix(work->X(),work->Y(),work->Z());

    //finding the mean value for every point in the 3d Workspace.
    //Assigning the mean value to the copyMatrix
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for( long i=0 ; i<work->X(); i++ ) {
        for( long j=0 ; j<work->Y(); j++ ) {
            for( long k=0 ; k<work->Z(); k++ ) {
                copyMatrix(i,j,k) = bilateralValue(i,j,k);
            }
        }
    }

    // since the filtered results are now stored in the copymatrix, they need to be
    // moved to the Workspace. This is done through the Workspace
    // copy function, which is already parallel
    work->matrix.copy(&copyMatrix);

    return true;
}


short BilateralFilter::bilateralValue(long x, long y, long z) {

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

    // defining variables for calculations
    double bilateral;
    double w;
    double term_1_w;
    double term_2_w;
    double sum_w = 0;
    double sum_I_times_w = 0;
    double work_value = work->matrix(x,y,z);

    //iterating through the window and calculating
    for( long i=xStart; i<=xEnd; i++ ) {
        for( long j=yStart; j<=yEnd; j++ ) {
            for( long k=zStart; k<=zEnd; k++ ) {
                term_1_w = - ( (x-i)*(x-i) + (y-j)*(y-j) + (z-k)*(z-k) ) / (2* sigma_d*sigma_d);
                term_2_w = - ( work_value - work->matrix(i,j,k) )*( work_value - work->matrix(i,j,k) ) / (2*sigma_r*sigma_r);
                w = exp( term_1_w + term_2_w );

                sum_w += w;
                sum_I_times_w += work->matrix(i,j,k)*w;
            }
        }
    }

    bilateral = sum_I_times_w/sum_w;

    return bilateral;
}


bool BilateralFilter::logInput() {

    puma::Logger *logger = work->log;

    logger->appendLogSection("Execute Bilateral 3D Filter");
    logger->appendLogItem("Current Time: ");
    logger->appendLogItem(logger->getTime());
    logger->newLine();
    logger->appendLogLine(" -- Inputs:");
    logger->appendLogItem("Window Radius: ");
    logger->appendLogItem(window_radius);
    logger->newLine();
    logger->appendLogItem("Domain Parameter (sigma_d): ");
    logger->appendLogItem(sigma_d);
    logger->newLine();
    logger->appendLogItem("Range Parameter (sigma_r): ");
    logger->appendLogItem(sigma_r);
    logger->newLine();

    logger->writeLog();

    return true;
}


bool BilateralFilter::logOutput() {

    puma::Logger *logger = work->log;

    logger->appendLogLine("Successfully Executed Filter");
    logger->appendLogItem("Current Time: ");
    logger->appendLogItem(logger->getTime());
    logger->newLine();

    logger->writeLog();

    return true;
}


bool BilateralFilter::errorCheck(std::string *errorMessage) {

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

    if(sigma_d <= 0) {
        (*errorMessage).append("Invalid sigma_d, must be > 0\n");
        returnBool = false;
    }
    else if(sigma_d > work->size()) {
        (*errorMessage).append("Invalid sigma_d, must be smaller than the domain size. Check inputs\n");
        returnBool = false;
    }

    if(sigma_r <= 0) {
        (*errorMessage).append("Invalid sigma_r, must be > 0\n");
        returnBool = false;
    }
    else if(sigma_r > 65536) {
        (*errorMessage).append("Invalid sigma_r, must be less than 2^16. Check inputs\n");
        returnBool = false;
    }

    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }

    return returnBool;
}
