#include "medianfilter3d.h"


bool puma::filter_Median3D(Workspace *work, int window_radius, int numThreads) {

    MedianFilter3D filter(work,window_radius,numThreads);
    return filter.execute();
}


MedianFilter3D::MedianFilter3D(puma::Workspace *work, int window_radius, int numThreads) {

    this->work = work;
    this->window_radius = window_radius;
    this->numThreads = numThreads;
}


bool MedianFilter3D::execute() {

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
        std::cout << "Median 3d Filter Error: " <<  errorMessage << std::endl;
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


bool MedianFilter3D::filterHelper() {

    //creating copy matrix
    puma::Matrix<short> copyMatrix(work->X(),work->Y(),work->Z());

    //finding the median value for every point in the 3d Workspace.
    //Assigning the median value to the copyMatrix
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for( long i=0 ; i<work->X(); i++ ) {
        for( long j=0 ; j<work->Y(); j++ ) {
            for( long k=0 ; k<work->Z(); k++ ) {
                copyMatrix(i,j,k) = medianValue(i,j,k);
            }
        }
    }

    // since the filtered results are now stored in the copymatrix, they need to be
    // moved to the Workspace. This is done through the Workspace
    // copy function, which is already parallel
    work->matrix.copy(&copyMatrix);

    return true;
}


void MedianFilter3D::swap(short &val1, short &val2) {

    short temp = val1;
    val1 = val2;
    val2 = temp;
}


short MedianFilter3D::partition(short *arIntegers, int left, int right, int pivot) {

    short pivotValue = arIntegers[pivot];

    swap(arIntegers[pivot], arIntegers[right]);
    short storeIndex = left;

    for (int i = left; i < right; i++)
    {
        if (arIntegers[i] <= pivotValue)
        {
            swap(arIntegers[i], arIntegers[storeIndex]);
            storeIndex++;
        }
    }
    swap(arIntegers[storeIndex], arIntegers[right]);
    return storeIndex;
}


short MedianFilter3D::median3(short *arIntegers,int left,int right) {

    short center = (left+right)/2;

    if(arIntegers[center] < arIntegers[left])
        swap(arIntegers[left],arIntegers[center]);
    if(arIntegers[right] < arIntegers[left])
        swap(arIntegers[left],arIntegers[right]);
    if(arIntegers[right] < arIntegers[center])
        swap(arIntegers[center],arIntegers[right]);

    swap(arIntegers[center],arIntegers[right-1]);

    return center;
}


void MedianFilter3D::quicksort(short* arIntegers, int left, int right) {

    if (right > left) {
        short pivotIndex = median3(arIntegers,left,right);
        short pivotNewIndex = partition(arIntegers, left, right, pivotIndex);

        quicksort(arIntegers, left, pivotNewIndex-1);
        quicksort(arIntegers, pivotNewIndex+1, right);
    }
}


short MedianFilter3D::getMedian(short* daArray, int iSize) {

    //taking the unsorted input array and sorting it
    quicksort(daArray,0,iSize-1);

    //computed the median value of the array
    double dMedian = 0.0;
    if ((iSize % 2) == 0) {
        dMedian = (daArray[iSize/2] + daArray[(iSize/2) - 1])/2.0;
    } else {
        dMedian = daArray[iSize/2];
    }
    return (short)dMedian;
}


short MedianFilter3D::medianValue(long x, long y, long z) {

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

    // creating an array that will store all of the values in the window
    short totalValues[xSize * ySize * zSize];

    //iterating through the window and adding all the elements into the totalValues
    for( long i=xStart; i<=xEnd; i++ ) {
        for( long j=yStart; j<=yEnd; j++ ) {
            for( long k=zStart; k<=zEnd; k++ ) {
                totalValues[zSize*ySize*(i-xStart)+zSize*(j-yStart)+(k-zStart)] = work->matrix(i,j,k);
            }
        }
    }

    //computing the median value of the window elements
    short median = getMedian(totalValues, xSize*ySize*zSize);

    return median;
}


bool MedianFilter3D::logInput() {

    puma::Logger *logger = work->log;

    logger->appendLogSection("Execute Median 3D Filter");
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


bool MedianFilter3D::logOutput() {

    puma::Logger *logger = work->log;

    logger->appendLogLine("Successfully Executed Filter");
    logger->appendLogItem("Current Time: ");
    logger->appendLogItem(logger->getTime());
    logger->newLine();

    logger->writeLog();

    return true;
}


bool MedianFilter3D::errorCheck(std::string *errorMessage) {

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
