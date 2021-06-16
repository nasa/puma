#include "fv_tortuosity.h"


puma::Vec3<double> puma::compute_FVTortuosity(puma::Workspace *grayWS, puma::Matrix<double> *C, puma::Cutoff cutoff,
                                              std::string sideBC, std::string solverType, char dir, double solverTol,
                                              int solverMaxIt, bool print, int numThreads) {
    if (cutoff.first < -0.5 || cutoff.second> 32767 || cutoff.first > cutoff.second) {
        grayWS->printer->error("Finite Volume Thermal Conductivity Error: Invalid Cutoff Ranges");
        return puma::Vec3<double>(-1,-1,-1);
    }
    puma::Workspace segWS(grayWS->getShape(),grayWS->log);
    segWS.setPrinter(grayWS->printer);

    segWS.setMaterialID(grayWS,cutoff,1);


    FV_Tortuosity cond(&segWS,C,1,sideBC,solverType,dir,solverTol,solverMaxIt,print,numThreads);
    return cond.compute();
}

FV_Tortuosity::FV_Tortuosity(puma::Workspace *segWS, puma::Matrix<double> *C, int voidSpace,
                             std::string sideBC, std::string solverType, char dir, double solverTol,
                             int solverMaxIt, bool print, int numThreads) {
    this->segWS = segWS;
    this->T = C;
    this->voidSpace = voidSpace;
    this->sideBC = sideBC;
    this->solverType = solverType;
    this->dir = dir;
    this->solverTol = solverTol;
    this->solverMaxIt = solverMaxIt;
    this->print = print;
    this->numThreads = numThreads;

}

puma::Vec3<double> FV_Tortuosity::compute() {
    logInput();
    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        segWS->printer->error(errorMessage);
        return puma::Vec3<double>(-1,-1,-1);
    }

    std::map<int,double> matCond;
    matCond.insert( std::pair<int,double>(voidSpace,1.0) );

    puma::Matrix<double> kMatrix(segWS->X(),segWS->Y(),segWS->Z());
    FV_Diffusion::computeKMatrix(segWS,matCond,&kMatrix,numThreads);


    segWS->printer->print(puma::StringBuilder() << kMatrix.average() <<  "  -- Min: " << kMatrix.min() << "  --Max: " << kMatrix.max() );

    segWS->printer->print(puma::StringBuilder() << sideBC << " " << solverType << " " << dir << " " << solverTol << " " );

    FV_Diffusion solver(T,&kMatrix,sideBC,solverType,dir,solverTol,solverMaxIt,print,segWS->printer,numThreads);
    Tortuosity = solver.compute_DiffusionCoefficient();

    double porosity = puma::compute_VolumeFraction(segWS,voidSpace,numThreads);


    Tortuosity.x = porosity/Tortuosity.x;
    Tortuosity.y = porosity/Tortuosity.y;
    Tortuosity.z = porosity/Tortuosity.z;

    logOutput();

    return Tortuosity;
}

bool FV_Tortuosity::logInput() {
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

bool FV_Tortuosity::logOutput() {
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

bool FV_Tortuosity::errorCheck(std::string *errorMessage) {
    if(segWS->matrix.size() == 0) {
        *errorMessage = "Empty Material Matrix";
        return false;
    }
    if(!(dir == 'x' || dir == 'X' || dir == 'y' || dir == 'Y' || dir == 'z' || dir == 'Z' )) {
        *errorMessage = "Invalid Direction";
        return false;
    }
    if(!(solverType.compare("conjugategradient") == 0 || solverType.compare("ConjugateGradient") == 0 ||
         solverType.compare("Conjugategradient") == 0 || solverType.compare("conjugateGradient") == 0 ||
         solverType.compare("conjugate gradient") == 0 || solverType.compare("Conjugate Gradient") == 0 ||
         solverType.compare("Conjugate gradient") == 0 || solverType.compare("conjugate Gradient") == 0 ||
         solverType.compare("conjugate_gradient") == 0 || solverType.compare("Conjugate_Gradient") == 0 ||
         solverType.compare("Conjugate_gradient") == 0 || solverType.compare("conjugate_Gradient") == 0 ||
         solverType.compare("bicgstab") == 0 || solverType.compare("BiCGSTAB") == 0 ||
         solverType.compare("Bicgstab") == 0)) {
        *errorMessage = "Invalid Iterative Solver";
        return false;
    }
    if(!(sideBC.compare("periodic") == 0 || sideBC.compare("Periodic") == 0 || sideBC.compare("symmetric") == 0 || sideBC.compare("Symmetric") == 0)) {
        *errorMessage = "Invalid Boundary Conditions";
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


