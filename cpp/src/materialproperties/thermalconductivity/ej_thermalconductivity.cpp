#include "ej_thermalconductivity.h"

#include <utility>


puma::Vec3<double> puma::compute_EJThermalConductivity(Workspace *grayWS, puma::Matrix<double> *T, const std::map<int, double>& matCond,
                                                       char dir, double solverTol, int solverMaxIt, bool print, int numThreads) {

    Workspace segWS(grayWS->getShape(), grayWS->log);
    segWS.setPrinter(grayWS->printer);
    std::map<int, double> condPairs;
    int matId = 0;
    int lowCutOff = 0;
    for (auto & it : matCond) {
        if (it.first < -0.5 || it.first > 32767) {
            segWS.printer->error("Explicit Jump Thermal Conductivity Error: Invalid MatCond puma::Cutoff");
            return puma::Vec3<double>(-1,-1,-1);
        }
        segWS.setMaterialID(grayWS,puma::Cutoff(lowCutOff,it.first),matId);
        condPairs[matId] = it.second;
        lowCutOff = it.first+1;
        matId++;
    }

    EJ_ThermalConductivity cond(&segWS,T,condPairs,dir,solverTol,solverMaxIt,print ,numThreads);
    return cond.compute();

}

EJ_ThermalConductivity::EJ_ThermalConductivity(puma::Workspace *segWS, puma::Matrix<double> *T, std::map<int, double> matCond,
                                               char dir, double solverTol, int solverMaxIt, bool print, int numThreads) {
    this->segWS = segWS;
    this->T = T;
    this->matCond = std::move(matCond);
    this->dir = dir;
    this->solverTol = solverTol;
    this->solverMaxIt = solverMaxIt;
    this->print = print;
    this->numThreads = numThreads;


}

puma::Vec3<double> EJ_ThermalConductivity::compute() {

    logInput();
    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        segWS->printer->error(errorMessage);
        return puma::Vec3<double>(-1,-1,-1);
    }

    puma::Matrix<double> kMatrix(segWS->X(),segWS->Y(),segWS->Z());
    EJ_Diffusion::computeKMatrix(segWS,matCond,&kMatrix,numThreads);

    EJ_Diffusion solver(T,&kMatrix,dir,solverTol,solverMaxIt,print, segWS->printer, numThreads);
    thermalConductivity = solver.compute_DiffusionCoefficient();

    logOutput();

    return thermalConductivity;
}

bool EJ_ThermalConductivity::logInput() {
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

bool EJ_ThermalConductivity::logOutput() {
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

bool EJ_ThermalConductivity::errorCheck(std::string *errorMessage) {
    if(segWS->matrix.size() == 0) {
        *errorMessage = "Empty Material Matrix";
        return false;
    }
    if(!(dir == 'x' || dir == 'X' || dir == 'y' || dir == 'Y' || dir == 'z' || dir == 'Z' )) {
        *errorMessage = "Invalid Direction";
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


