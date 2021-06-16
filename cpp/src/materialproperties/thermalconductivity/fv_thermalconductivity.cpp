#include "fv_thermalconductivity.h"

#include <utility>


puma::Vec3<double> puma::compute_FVThermalConductivity(Workspace *grayWS, puma::Matrix<double> *T, const std::map<int, double>& matCond,
                                                          std::string sideBC, std::string solverType, char dir, double solverTol,
                                                          int solverMaxIt, bool print, int numThreads) {

    Workspace segWS(grayWS->getShape(), grayWS->log);
    segWS.setPrinter(grayWS->printer);
    std::map<int, double> condPairs;
    int matId = 0;
    int lowCutOff = 0;
    for (auto & it : matCond) {
        if (it.first < -0.5 || it.first > 32767) {
            segWS.printer->error("Finite Volume Thermal Conductivity Error: Invalid MatCond puma::Cutoff");
            return puma::Vec3<double>(-1,-1,-1);
        }
        segWS.setMaterialID(grayWS,puma::Cutoff(lowCutOff,it.first),matId);
        condPairs[matId] = it.second;
        lowCutOff = it.first+1;
        matId++;
    }

    FV_ThermalConductivity cond(&segWS,T,condPairs,std::move(sideBC),std::move(solverType),dir,solverTol,solverMaxIt, print,numThreads);
    return cond.compute();
}


FV_ThermalConductivity::FV_ThermalConductivity(puma::Workspace *segWS, puma::Matrix<double> *T, std::map<int, double> matCond,
                                               std::string sideBC, std::string solverType, char dir, double solverTol,
                                               int solverMaxIt, bool print, int numThreads) {
    this->segWS = segWS;
    this->T = T;
    this->matCond = std::move(matCond);
    this->sideBC = std::move(sideBC);
    this->solverType = std::move(solverType);
    this->dir = dir;
    this->solverTol = solverTol;
    this->solverMaxIt = solverMaxIt;
    this->print = print;
    this->numThreads = numThreads;


}


puma::Vec3<double> FV_ThermalConductivity::compute() {
    logInput();
    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        std::cout << "Thermal Conductivity Error: " <<  errorMessage << std::endl;
        return puma::Vec3<double>(-1,-1,-1);
    }

    puma::Matrix<double> kMatrix(segWS->X(),segWS->Y(),segWS->Z());
    FV_Diffusion::computeKMatrix(segWS,matCond,&kMatrix, numThreads);

    FV_Diffusion solver(T,&kMatrix,sideBC,solverType,dir,solverTol,solverMaxIt,print, segWS->printer, numThreads);
    thermalConductivity = solver.compute_DiffusionCoefficient();

    logOutput();

    return thermalConductivity;
}

bool FV_ThermalConductivity::logInput() {
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


    logger->appendLogItem("Constituent Conductivities: ");
    for (auto & it : matCond) {
        logger->appendLogItem("Material ");
        logger->appendLogItem(it.first);
        logger->appendLogItem(" Conductivity: ");
        logger->appendLogItem(it.second);
        logger->newLine();
    }
    logger->writeLog();

    return true;
}

bool FV_ThermalConductivity::logOutput() {
    puma::Logger *logger = segWS->log;

    logger->appendLogLine(" -- Output:");
    logger->appendLogItem("Conductivity: ");
    logger->appendLogItem(thermalConductivity.x);
    logger->appendLogItem(",");
    logger->appendLogItem(thermalConductivity.y);
    logger->appendLogItem(",");
    logger->appendLogItem(thermalConductivity.z);

    logger->newLine();
    logger->writeLog();

    return true;
}

bool FV_ThermalConductivity::errorCheck(std::string *errorMessage) {
    if(segWS->matrix.size() == 0) {
        *errorMessage = "Empty Material Matrix";
        return false;
    }
    if(!(dir == 'x' || dir == 'X' || dir == 'y' || dir == 'Y' || dir == 'z' || dir == 'Z' )) {
        *errorMessage = "Invalid Direction";
        return false;
    }
    if(!(solverType == "conjugategradient" || solverType == "ConjugateGradient" ||
         solverType == "Conjugategradient" || solverType == "conjugateGradient" ||
         solverType == "conjugate gradient" || solverType == "Conjugate Gradient" ||
         solverType == "Conjugate gradient" || solverType == "conjugate Gradient" ||
         solverType == "conjugate_gradient" || solverType == "Conjugate_Gradient" ||
         solverType == "Conjugate_gradient" || solverType == "conjugate_Gradient" ||
         solverType == "cg" || solverType == "bicgstab" ||
         solverType == "BiCGSTAB" || solverType == "Bicgstab")) {
        *errorMessage = "Invalid Iterative Solver";
        return false;
    }
    if(!(sideBC == "periodic" || sideBC == "Periodic" || sideBC == "symmetric" || sideBC == "Symmetric")) {
        *errorMessage = "Invalid Boundary Conditions";
        return false;
    }

    for (auto & it : matCond) {
        if (it.second < 0) {
            *errorMessage = "Negative Conductivity";
            return false;
        }
    }

    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }

    return true;
}


