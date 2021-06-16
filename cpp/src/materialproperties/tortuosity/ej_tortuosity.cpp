#include "ej_tortuosity.h"


puma::Vec3<double> puma::compute_EJTortuosity(puma::Workspace *grayWS, puma::Matrix<double> *C, puma::Cutoff cutoff,
                                                       char dir, double solverTol, int solverMaxIt, bool print, int numThreads) {
    if (cutoff.first < -0.5 || cutoff.second> 32767 || cutoff.first > cutoff.second) {
        std::cout << "Finite Volume Thermal Conductivity Error: Invalid Cutoff Ranges" << std::endl;
        return puma::Vec3<double>(-1,-1,-1);
    }
    puma::Workspace segWS(grayWS->getShape(),grayWS->log);
    segWS.setPrinter(grayWS->printer);

    segWS.setMaterialID(grayWS,cutoff,1);

    EJ_Tortuosity cond(&segWS,C,1,dir,solverTol,solverMaxIt,print,numThreads);
    return cond.compute();
}

EJ_Tortuosity::EJ_Tortuosity(puma::Workspace *segWS, puma::Matrix<double> *C, int voidSpace,
                                               char dir, double solverTol, int solverMaxIt, bool print, int numThreads) {
    this->segWS = segWS;
    this->T = C;
    this->voidSpace = voidSpace;
    this->dir = dir;
    this->solverTol = solverTol;
    this->solverMaxIt = solverMaxIt;
    this->print = print;
    this->numThreads = numThreads;

}

puma::Vec3<double> EJ_Tortuosity::compute() {
    logInput();
    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        segWS->printer->error(errorMessage);
        return puma::Vec3<double>(-1,-1,-1);
    }

    std::map<int,double> matCond;
    matCond.insert( std::pair<int,double>(voidSpace,1.0) );

    puma::Matrix<double> kMatrix(segWS->X(),segWS->Y(),segWS->Z());
    EJ_Diffusion::computeKMatrix(segWS,matCond,&kMatrix,numThreads);

    segWS->printer->print(puma::StringBuilder() << "Average: " << kMatrix.average() <<  "  -- Min: " << kMatrix.min() << "  --Max: " << kMatrix.max());

    EJ_Diffusion solver(T,&kMatrix,dir,solverTol,solverMaxIt,print,segWS->printer, numThreads);
    Tortuosity = solver.compute_DiffusionCoefficient();

    double porosity = puma::compute_VolumeFraction(segWS,voidSpace,numThreads);


    Tortuosity.x = porosity/Tortuosity.x;
    Tortuosity.y = porosity/Tortuosity.y;
    Tortuosity.z = porosity/Tortuosity.z;

    logOutput();

    return Tortuosity;
}

bool EJ_Tortuosity::logInput() {
    puma::Logger *logger = segWS->log;

    logger->appendLogSection("Compute Conductivity");
    logger->appendLogLine(" -- Inputs:");
    logger->appendLogItem("interior Size: ");

    logger->appendLogItem(segWS->X());
    logger->appendLogItem(", ");
    logger->appendLogItem(segWS->Y());
    logger->appendLogItem(", ");
    logger->appendLogItem(segWS->Z());
    logger->newLine();


    logger->appendLogItem("Void Space Value: ");
    logger->appendLogItem(voidSpace);
    logger->writeLog();

    return true;
}

bool EJ_Tortuosity::logOutput() {
    puma::Logger *logger = segWS->log;

    logger->appendLogLine(" -- Output:");
    logger->appendLogItem("Conductivity: ");
    logger->appendLogItem(Tortuosity.x);
    logger->appendLogItem(",");
    logger->appendLogItem(Tortuosity.y);
    logger->appendLogItem(",");
    logger->appendLogItem(Tortuosity.z);

    logger->newLine();
    logger->writeLog();

    return true;
}

bool EJ_Tortuosity::errorCheck(std::string *errorMessage) {
    if(segWS->matrix.size() == 0) {
        *errorMessage = "Empty Material Matrix";
        return false;
    }
    if(!(dir == 'x' || dir == 'X' || dir == 'y' || dir == 'Y' || dir == 'z' || dir == 'Z' )) {
        *errorMessage = "Invalid Direction";
        return false;
    }
    if(voidSpace < 0 || voidSpace > 32767) {
        *errorMessage = "Invalid Void Space";
        return false;
    }


    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }

    return true;
}


