#include "artificialflux.h"


////// ISOTROPIC ARTIFICIAL FLUX METHOD //////

bool puma::compute_orientationAF(puma::Workspace *grayWS, puma::MatVec3<double> *qDirs, puma::Cutoff cutoff,
                                 double solverTol, int solverMaxIt, bool print, int numThreads) {

    if(cutoff.first < -0.5 || cutoff.second > 32767 || cutoff.first > cutoff.second){  // maximum signed short value 32767
        std::cout << "Orientation Artificial Flux Error: Invalid Cutoff Ranges" << std::endl;
        return false;
    }

    Workspace segWS(grayWS->getShape(), grayWS->log);
    segWS.setMaterialID(grayWS,cutoff,1);

    ArtificialFlux cond(&segWS,qDirs,solverTol,solverMaxIt,print,numThreads);
    return cond.compute();
}



ArtificialFlux::ArtificialFlux(puma::Workspace *segWS, puma::MatVec3<double> *qDirs,
                               double solverTol, int solverMaxIt, bool print, int numThreads) {
    this->segWS = segWS;
    this->qDirs = qDirs;
    this->solverTol = solverTol;
    this->solverMaxIt = solverMaxIt;
    this->print = print;
    this->numThreads = numThreads;
}

bool ArtificialFlux::compute() {
    logInput();
    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        std::cout << "Orientation Artificial Flux Error: " <<  errorMessage << std::endl;
        return false;
    }

    std::map<int,double> matCond{{0, 0}, {1, 1}};
    puma::Matrix<double> kMatrix;
    ArtificialFlux_Diffusion::computeKMatrix(segWS,matCond,&kMatrix, numThreads);

    std::string sideBC = "symmetric";
    std::string solverType = "cg";
    puma::MatVec3<double> qDirs_tmp;
    puma::Matrix<double> T;

    ArtificialFlux_Diffusion Xsolver(&T,qDirs,&kMatrix,sideBC,solverType,'x',solverTol,solverMaxIt,print,numThreads);
    Xsolver.compute_DiffusionCoefficient();

    ArtificialFlux_Diffusion Ysolver(&T,&qDirs_tmp,&kMatrix,sideBC,solverType,'y',solverTol,solverMaxIt,print,numThreads);
    Ysolver.compute_DiffusionCoefficient();

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(long i=0;i<qDirs->size();i++){
        if(segWS->at(i) > 0) {
            qDirs->at(i).x = qDirs->at(i).x + qDirs_tmp.at(i).x;
            qDirs->at(i).y = qDirs->at(i).y + qDirs_tmp.at(i).y;
            qDirs->at(i).z = qDirs->at(i).z + qDirs_tmp.at(i).z;
        }
    }

    ArtificialFlux_Diffusion Zsolver(&T,&qDirs_tmp,&kMatrix,sideBC,solverType,'z',solverTol,solverMaxIt,print,numThreads);
    Zsolver.compute_DiffusionCoefficient();

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for(long i=0;i<qDirs->X();i++){
        for(long j=0;j<qDirs->Y();j++){
            for(long k=0;k<qDirs->Z();k++){
                if(segWS->at(i,j,k) > 0) {
                    qDirs->at(i,j,k).x = qDirs->at(i,j,k).x + qDirs_tmp.at(i,j,k).x;
                    qDirs->at(i,j,k).y = qDirs->at(i,j,k).y + qDirs_tmp.at(i,j,k).y;
                    qDirs->at(i,j,k).z = qDirs->at(i,j,k).z + qDirs_tmp.at(i,j,k).z;

                    double magn = qDirs->at(i,j,k).magnitude();
                    if(magn>0){
                        qDirs->at(i,j,k).x = qDirs->at(i,j,k).x/magn;
                        qDirs->at(i,j,k).y = qDirs->at(i,j,k).y/magn;
                        qDirs->at(i,j,k).z = qDirs->at(i,j,k).z/magn;
                    } else { // pick random direction
                        qDirs->at(i,j,k) = qDirs_tmp.at(i,j,k).norm();
                    }
                }
            }
        }
    }

    logOutput();

    return true;
}

bool ArtificialFlux::logInput() {
    puma::Logger *logger = segWS->log;

    logger->appendLogSection("Compute Orientation using Artificial Flux");
    logger->appendLogLine(" -- Inputs:");
    logger->appendLogItem("interior Size: ");

    logger->appendLogItem(segWS->X());
    logger->appendLogItem(", ");
    logger->appendLogItem(segWS->Y());
    logger->appendLogItem(", ");
    logger->appendLogItem(segWS->Z());

    logger->appendLogItem("Solver Tolerance: ");
    logger->appendLogItem(solverTol);
    logger->newLine();


    logger->writeLog();

    return true;
}

bool ArtificialFlux::logOutput() {
    puma::Logger *logger = segWS->log;

    logger->appendLogItem("Completed Orientation computation using the Artificial Flux (AF) method");

    logger->newLine();
    logger->writeLog();

    return true;
}

bool ArtificialFlux::errorCheck(std::string *errorMessage) {
    if(segWS->matrix.size() == 0) {
        *errorMessage = "Empty Material Matrix";
        return false;
    }

    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }

    if(solverTol<=0) {
        *errorMessage = "Solver Tolerance has to be > 0";
        return false;
    }

    if(solverMaxIt<=0) {
        *errorMessage = "Solver Maximum Iteration has to be > 0";
        return false;
    }

    return true;
}