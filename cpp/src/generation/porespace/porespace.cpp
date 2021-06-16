#include "porespace.h"
#include <numeric>


bool puma::identifyPoreSpace(Workspace *ws, puma::Matrix<long> *poreMatrix, puma::Cutoff voidRange, bool print, int numThreads){
    PoreSpace myPoreSpace(ws, poreMatrix, voidRange, print, numThreads);
    return myPoreSpace.identify();
}

// Important note: This function assumes that the pore with the largest volume fraction is the only open pore.
// All other pores will be filled in
bool puma::fillClosedPores(Workspace *ws, puma::Cutoff voidRange, short filledValue, bool print, int numThreads){

    if(filledValue < -0.5) {
        std::cout << "Invalid filledValue. Must be >= 0" << std::endl;
        return false;
    }
    if(filledValue > 32767.5) {
        std::cout << "Invalid filledValue. Must be <= 32767" << std::endl;
        return false;
    }

    puma::Matrix<long> poreMatrix;
    PoreSpace myPoreSpace(ws, &poreMatrix, voidRange, print, numThreads);

    if( ! myPoreSpace.identify() ) {
        return false;
    }

    for(long i = 0; i < poreMatrix.size(); i++ ) {
        if(poreMatrix.at(i) > 0 && poreMatrix.at(i) != 1) {
            ws->at(i) = filledValue;
        }
    }

    return true;
}


PoreSpace::PoreSpace(puma::Workspace *ws, puma::Matrix<long> *poreMatrix, puma::Cutoff voidRange, bool print, int numThreads) {
    this->poreMatrix = poreMatrix;
    this->ws = ws;
    this->voidRange = voidRange;
    this->print = print;
    this->numThreads = numThreads;
}


bool PoreSpace::identify() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed
        std::cout << "Error in Pore Identification: " << errorMessage << std::endl;
        return false;
    }

    bool success = PoreSpaceHelper();
    if(!success){
        return false;
    }

    if( !logOutput() ) {
        std::cout << "Failed to log Outputs" << std::endl;
        return false;
    }

    return true;
}

long PoreSpace::maxNeighbors(long i, long j, long k) {
    if( poreMatrix->at(i,j,k) == 0 ) {
        return 0;
    }

    long minValue;
    if(i == 0) {
        minValue = poreMatrix->at(i,j,k);
    } else {
        minValue = std::max(poreMatrix->at(i,j,k), poreMatrix->at(i-1,j,k));
    }
    if(i != poreMatrix->X() - 1) {
        minValue = std::max(minValue, poreMatrix->at(i+1,j,k));
    }
    if(j != 0) {
        minValue = std::max(minValue, poreMatrix->at(i,j-1,k));
    }
    if(j != poreMatrix->Y() - 1) {
        minValue = std::max(minValue, poreMatrix->at(i,j+1,k));
    }
    if(k != 0) {
        minValue = std::max(minValue, poreMatrix->at(i,j,k-1));
    }
    if(k != poreMatrix->Z() - 1) {
        minValue = std::max(minValue, poreMatrix->at(i,j,k+1));
    }

    return minValue;
}


bool PoreSpace::PoreSpaceHelper() {

    // Giving each void voxel a unique identifier
    poreMatrix->resize(ws->getX(), ws->getY(), ws->getZ());
    for(long i = 0; i < poreMatrix->size(); i++) {
        if((float)ws->matrix.at(i) > voidRange.first && (float)ws->matrix.at(i) < voidRange.second) { // voxel within the void range
            poreMatrix->at(i) = i + 1;
        } else { // Voxel outside of the void range
            poreMatrix->at(i) = 0;
        }
    }

    int loopCount = 0;
    bool change = true;
    while(change) { // while the voxels are still being modified

        if(print) { std::cout << '\r' << "Iteration: " << loopCount << std::flush; }
        
        std::vector<bool> change_local(poreMatrix->X(), false);

        change = false;

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i = 0; i < poreMatrix->X(); i++) {
            for(long j = 0; j < poreMatrix->Y(); j++) {
                for(long k = 0; k < poreMatrix->Z(); k++) {
                    long oldValue = poreMatrix->at(i,j,k);
                    poreMatrix->at(i,j,k) = maxNeighbors(i,j,k);
                    if(oldValue != poreMatrix->at(i,j,k)) { change_local[i] = true; }
                }
            }
        }

        auto it = std::find(change_local.begin(), change_local.end(), true);
        if (it != change_local.end()) {
            change = true;
        }

        loopCount ++;
    }

    if(print) { std::cout << std::endl << "Pore Space Identified, setting the pore IDs" << std::endl; }

    std::vector<long> uniqueValues;
    std::vector<long> uniqueCounter;
    for(long i=0;i<poreMatrix->size(); i++) {
        if(poreMatrix->at(i) == 0) {
            continue;
        }

        bool found = false;
        for(int j = 0; j < uniqueValues.size(); j++) {
            if(uniqueValues[j] == poreMatrix->at(i)) {
                found = true;
                uniqueCounter[j] ++;
                break;
            }
        }

        if(!found) {
            uniqueValues.push_back(poreMatrix->at(i));
            uniqueCounter.push_back(1);
        }
    }

    std::vector<int> indices(uniqueCounter.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](int A, int B)->bool{return uniqueCounter[A]>uniqueCounter[B];});

    std::map<long, long> values_counter;
    for(int j = 0; j < uniqueValues.size(); j++) {
        values_counter[uniqueValues[indices[j]]] = j + 1;
    }

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(long i=0;i<poreMatrix->size(); i++) {
        if(poreMatrix->at(i) != 0) {
            poreMatrix->at(i) = values_counter[poreMatrix->at(i)];
        }
    }

    if(print) {
        std::cout << "Number of pores: " << uniqueValues.size() << std::endl;
        double maxValue = MP_VolumeFractionHelper::volumeFraction(poreMatrix,puma::Cutoff((float)1,(float)1), numThreads);
        std::cout << "Maximum void volume fraction: " << maxValue << std::endl;
    }

    return true;
}


bool PoreSpace::errorCheck(std::string *errorMessage) {

    if(voidRange.first < -0.5) {
        (*errorMessage) = "Invalid cutoff range";
        return false;
    }
    if(voidRange.first > voidRange.second) {
        (*errorMessage) = "Invalid cutoff range";
        return false;
    }
    if( ws->size() == 0) {
        (*errorMessage) = "empty workspace";
        return false;
    }


    return true;
}


bool PoreSpace::logInput() {
    puma::Logger *log;
    log = ws->log;

    (*log).appendLogSection("Identify Pore Space:");
    (*log).appendLogItem("Time: ");
    (*log).appendLogItem(puma::Logger::getTime());
    (*log).newLine();
    (*log).appendLogLine(" -- Inputs:");
    (*log).appendLogItem("Workspace size: ");

    (*log).newLine();

    (*log).writeLog();

    return true;

}


bool PoreSpace::logOutput() {
    puma::Logger *log;
    log = ws->log;

    (*log).appendLogItem("Successful Identify Pore Space");
    (*log).newLine();
    (*log).appendLogItem("Time: ");
    (*log).appendLogItem(puma::Logger::getTime());
    (*log).newLine();

    (*log).writeLog();

    return true;
}
