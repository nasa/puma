#include "fv_anisotropic_thermalconductivity.h"


/// Grayscale GLOBALLY Anisotropic Materials (no direction matrix)
puma::Vec3<double> puma::compute_FVanisotropicThermalConductivity(Workspace *grayWS, puma::Matrix<double> *T, puma::MatVec3<double> *q, const puma::MatCond& matCond,
                                                                  std::string method, std::string sideBC, std::string solverType, char dir, double solverTol,
                                                                  int solverMaxIt, bool print, int numThreads) {
    Workspace segWS(grayWS->getShape(), grayWS->log);
    puma::MatCond condPairs;
    int matId = 0;
    int lowCutOff = 0;
    for (const auto & it : matCond) {

        // Checking Matcond
        if (it.first < -0.5 || it.first > 32767) {
            std::cout << "Finite Volume Thermal Conductivity Error: Invalid MatCond puma::Cutoff" << std::endl;
            return puma::Vec3<double>(-1,-1,-1);
        }
        if (it.second.size() == 1) {
            if ((it.second[0] < 0)) {
                std::cout << "Finite Volume Thermal Conductivity Error: Diagonal Isotropic Conductivity cannot be negative" << std::endl;
                return puma::Vec3<double>(-1, -1, -1);
            }
        } else if (it.second.size() == 6){
            if ((it.second[0] < 0) || (it.second[1] < 0) || (it.second[2] < 0)) {
                std::cout << "Finite Volume Thermal Conductivity Error: Diagonal Anisotropic Conductivity cannot be negative" << std::endl;
                return puma::Vec3<double>(-1, -1, -1);
            }
        } else {
            std::cout << "Finite Volume Thermal Conductivity Error: Invalid element size in MatCond, there need to be 1 or 6 entries" << std::endl;
            return puma::Vec3<double>(-1,-1,-1);
        }

        segWS.setMaterialID(grayWS,puma::Cutoff(lowCutOff,it.first),matId);
        condPairs[matId] = it.second;
        lowCutOff = it.first+1;
        matId++;
    }

    puma::MatVec3<double> direction(1,1,1,puma::Vec3<double>(0.,0.,0.));
    puma::Matrix<double> prescribedBC(1,1,1);
    FV_anisotropic_ThermalConductivity cond(&segWS,T,q,condPairs,&direction,std::move(method),std::move(sideBC),&prescribedBC,std::move(solverType),dir,solverTol,solverMaxIt,print,numThreads);
    return cond.compute();
}


/// Grayscale GLOBALLY Anisotropic Materials (no direction matrix, with prescribedBC)
puma::Vec3<double> puma::compute_FVanisotropicThermalConductivity(Workspace *grayWS, puma::Matrix<double> *T, puma::MatVec3<double> *q, const puma::MatCond& matCond,
                                                                  std::string method, std::string sideBC, puma::Matrix<double> *prescribedBC, std::string solverType, char dir, double solverTol,
                                                                  int solverMaxIt, bool print, int numThreads) {
    Workspace segWS(grayWS->getShape(), grayWS->log);
    puma::MatCond condPairs;
    int matId = 0;
    int lowCutOff = 0;
    for (const auto & it : matCond) {

        // Checking Matcond
        if (it.first < -0.5 || it.first > 32767) {
            std::cout << "Finite Volume Thermal Conductivity Error: Invalid MatCond puma::Cutoff" << std::endl;
            return puma::Vec3<double>(-1,-1,-1);
        }
        if (it.second.size() == 1) {
            if ((it.second[0] < 0)) {
                std::cout << "Finite Volume Thermal Conductivity Error: Diagonal Isotropic Conductivity cannot be negative" << std::endl;
                return puma::Vec3<double>(-1, -1, -1);
            }
        } else if (it.second.size() == 6){
            if ((it.second[0] < 0) || (it.second[1] < 0) || (it.second[2] < 0)) {
                std::cout << "Finite Volume Thermal Conductivity Error: Diagonal Anisotropic Conductivity cannot be negative" << std::endl;
                return puma::Vec3<double>(-1, -1, -1);
            }
        } else {
            std::cout << "Finite Volume Thermal Conductivity Error: Invalid element size in MatCond, there need to be 1 or 6 entries" << std::endl;
            return puma::Vec3<double>(-1,-1,-1);
        }

        segWS.setMaterialID(grayWS,puma::Cutoff(lowCutOff,it.first),matId);
        condPairs[matId] = it.second;
        lowCutOff = it.first+1;
        matId++;
    }

    puma::MatVec3<double> direction(1,1,1,puma::Vec3<double>(0.,0.,0.));
    FV_anisotropic_ThermalConductivity cond(&segWS,T,q,condPairs,&direction,std::move(method),std::move(sideBC),prescribedBC,std::move(solverType),dir,solverTol,solverMaxIt,print,numThreads);
    return cond.compute();
}


/// LOCALLY Anisotropic Materials (with direction matrix)
puma::Vec3<double> puma::compute_FVanisotropicThermalConductivity(Workspace *grayWS, puma::Matrix<double> *T, puma::MatVec3<double> *q, const std::map<int, std::vector<double>>& matCond,
                                                                  puma::MatVec3<double> *direction, std::string method, std::string sideBC, std::string solverType, char dir, double solverTol,
                                                                  int solverMaxIt, bool print, int numThreads) {

    if(!(direction->X() == grayWS->X() && direction->Y() == grayWS->Y() && direction->Z() == grayWS->Z())) {
        std::cout << "Orientation MatVec has wrong size" << std::endl;
        return puma::Vec3<double>(-1,-1,-1);
    }

    Workspace segWS(grayWS->getShape(), grayWS->log);
    std::map<int, std::vector<double>> condPairs;
    int matId = 0;
    int lowCutOff = 0;
    for (auto & it : matCond) {

        // Checking Matcond
        if (it.first < -0.5 || it.first > 32767) {
            std::cout << "Finite Volume Thermal Conductivity Error: Invalid MatCond puma::Cutoff" << std::endl;
            return puma::Vec3<double>(-1,-1,-1);
        }
        if (it.second.size() == 1) {
            if ((it.second[0] < 0)) {
                std::cout << "Finite Volume Thermal Conductivity Error: Diagonal Isotropic Conductivity cannot be negative" << std::endl;
                return puma::Vec3<double>(-1, -1, -1);
            }
        } else if (it.second.size() == 2){
            if ((it.second[0] < 0) || (it.second[1] < 0)) {
                std::cout << "Finite Volume Thermal Conductivity Error: Diagonal Anisotropic Conductivity cannot be negative" << std::endl;
                return puma::Vec3<double>(-1, -1, -1);
            }
        } else if (it.second.size() == 6){
            if ((it.second[0] < 0) || (it.second[1] < 0) || (it.second[2] < 0)) {
                std::cout << "Finite Volume Thermal Conductivity Error: Diagonal Anisotropic Conductivity cannot be negative" << std::endl;
                return puma::Vec3<double>(-1, -1, -1);
            }
        } else {
            std::cout << "Finite Volume Thermal Conductivity Error: Invalid element size in MatCond, there need to be 1, 2 or 6 entries" << std::endl;
            return puma::Vec3<double>(-1,-1,-1);
        }

        segWS.setMaterialID(grayWS,puma::Cutoff(lowCutOff,it.first),matId);
        condPairs[matId] = it.second;
        lowCutOff = it.first+1;
        matId++;
    }

    puma::Matrix<double> prescribedBC(1,1,1);
    FV_anisotropic_ThermalConductivity cond(&segWS,T,q,condPairs,direction,std::move(method),std::move(sideBC),&prescribedBC,std::move(solverType),dir,solverTol,solverMaxIt,print,numThreads);
    return cond.compute();
}


/// Grayscale LOCALLY Anisotropic Materials (with direction matrix, with prescribedBC)
puma::Vec3<double> puma::compute_FVanisotropicThermalConductivity(Workspace *grayWS, puma::Matrix<double> *T, puma::MatVec3<double> *q, const std::map<int, std::vector<double>>& matCond,
                                                                  puma::MatVec3<double> *direction, std::string method, std::string sideBC, puma::Matrix<double> *prescribedBC,
                                                                  std::string solverType, char dir, double solverTol, int solverMaxIt, bool print, int numThreads) {

    if(!(direction->X() == grayWS->X() && direction->Y() == grayWS->Y() && direction->Z() == grayWS->Z())) {
        std::cout << "Orientation MatVec has wrong size" << std::endl;
        return puma::Vec3<double>(-1,-1,-1);
    }

    Workspace segWS(grayWS->getShape(), grayWS->log);
    std::map<int, std::vector<double>> condPairs;
    int matId = 0;
    int lowCutOff = 0;
    for (auto & it : matCond) {

        // Checking Matcond
        if (it.first < -0.5 || it.first > 32767) {
            std::cout << "Finite Volume Thermal Conductivity Error: Invalid MatCond puma::Cutoff" << std::endl;
            return puma::Vec3<double>(-1,-1,-1);
        }
        if (it.second.size() == 1) {
            if ((it.second[0] < 0)) {
                std::cout << "Finite Volume Thermal Conductivity Error: Diagonal Isotropic Conductivity cannot be negative" << std::endl;
                return puma::Vec3<double>(-1, -1, -1);
            }
        } else if (it.second.size() == 2){
            if ((it.second[0] < 0) || (it.second[1] < 0)) {
                std::cout << "Finite Volume Thermal Conductivity Error: Diagonal Anisotropic Conductivity cannot be negative" << std::endl;
                return puma::Vec3<double>(-1, -1, -1);
            }
        } else if (it.second.size() == 6){
            if ((it.second[0] < 0) || (it.second[1] < 0) || (it.second[2] < 0)) {
                std::cout << "Finite Volume Thermal Conductivity Error: Diagonal Anisotropic Conductivity cannot be negative" << std::endl;
                return puma::Vec3<double>(-1, -1, -1);
            }
        } else {
            std::cout << "Finite Volume Thermal Conductivity Error: Invalid element size in MatCond, there need to be 1, 2 or 6 entries" << std::endl;
            return puma::Vec3<double>(-1,-1,-1);
        }

        segWS.setMaterialID(grayWS,puma::Cutoff(lowCutOff,it.first),matId);
        condPairs[matId] = it.second;
        lowCutOff = it.first+1;
        matId++;
    }

    FV_anisotropic_ThermalConductivity cond(&segWS,T,q,condPairs,direction,std::move(method),std::move(sideBC),prescribedBC,std::move(solverType),dir,solverTol,solverMaxIt,print,numThreads);
    return cond.compute();
}


FV_anisotropic_ThermalConductivity::FV_anisotropic_ThermalConductivity(puma::Workspace *segWS, puma::Matrix<double> *T, puma::MatVec3<double> *q, std::map<int, std::vector<double>> matCond,
                                                                       puma::MatVec3<double> *direction, std::string method,std::string sideBC, puma::Matrix<double> *prescribedBC, std::string solverType, char dir, double solverTol,
                                                                       int solverMaxIt, bool print, int numThreads) {
    this->segWS = segWS;
    this->T = T;
    this->q = q;
    this->matCond = std::move(matCond);
    this->sideBC = std::move(sideBC);
    this->prescribedBC = prescribedBC;
    this->solverType = std::move(solverType);
    this->dir = dir;
    this->solverTol = solverTol;
    this->solverMaxIt = solverMaxIt;
    this->print = print;
    this->method = std::move(method);
    this->direction = direction;
    this->numThreads = numThreads;
}


puma::Vec3<double> FV_anisotropic_ThermalConductivity::compute() {
    logInput();
    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) {
        std::cout << "Thermal Conductivity Error: " <<  errorMessage << std::endl;
        return puma::Vec3<double>(-1,-1,-1);
    }

    puma::Matrix<std::vector<double>> kMatrix(segWS->X(),segWS->Y(),segWS->Z(),std::vector<double> (6, 0));
    FV_anisotropic_Diffusion::computeKMatrix(segWS,matCond, &kMatrix,direction,print,numThreads);

    FV_anisotropic_Diffusion solver(T,q,&kMatrix,sideBC,prescribedBC,solverType,dir,segWS->voxelLength,solverTol,solverMaxIt,print,method,numThreads);
    anisotropicthermalConductivity = solver.compute_DiffusionCoefficient();

    logOutput();

    return anisotropicthermalConductivity;
}


bool FV_anisotropic_ThermalConductivity::logInput() {
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
    logger->newLine();
    for (auto & it : matCond) {
        logger->appendLogItem("Material ");
        logger->appendLogItem(it.first);
        logger->appendLogItem(" Conductivity: ");
        logger->appendLogItem(it.second[0]);
        logger->appendLogItem(" ");
        logger->appendLogItem(it.second[1]);
        logger->appendLogItem(" ");
        logger->appendLogItem(it.second[2]);
        logger->appendLogItem(" ");
        logger->appendLogItem(it.second[3]);
        logger->appendLogItem(" ");
        logger->appendLogItem(it.second[4]);
        logger->appendLogItem(" ");
        logger->appendLogItem(it.second[5]);
        logger->newLine();
    }
    logger->writeLog();

    return true;
}


bool FV_anisotropic_ThermalConductivity::logOutput() {
    puma::Logger *logger = segWS->log;

    logger->appendLogLine(" -- Output:");
    logger->appendLogItem("Conductivity: ");
    logger->appendLogItem(anisotropicthermalConductivity.x);
    logger->appendLogItem(",");
    logger->appendLogItem(anisotropicthermalConductivity.y);
    logger->appendLogItem(",");
    logger->appendLogItem(anisotropicthermalConductivity.z);

    logger->newLine();
    logger->writeLog();

    return true;
}


bool FV_anisotropic_ThermalConductivity::errorCheck(std::string *errorMessage) {

    // Workspace check
    if(segWS->matrix.size() == 0) {
        *errorMessage = "Empty Material Matrix";
        return false;
    }
    else if (segWS->X() < 3 || segWS->Y() < 3 || segWS->Z() < 3) {
        *errorMessage = "Domain for anisotropic calculation must be at least 3 voxels in each direction";
        return false;
    }

    // Direction check
    if (dir == 'x' || dir == 'X') {
        dir = 'x';
    }
    else if (dir == 'y' || dir == 'Y') {
        dir = 'y';
    }
    else if (dir == 'z' || dir == 'Z') {
        dir = 'z';
    }
    else {
        *errorMessage = "Invalid Direction";
        return false;
    }

    // Solver check
    if (solverType == "conjugategradient" || solverType == "ConjugateGradient" ||
        solverType == "Conjugategradient" || solverType == "conjugateGradient" ||
        solverType == "conjugate gradient" || solverType == "Conjugate Gradient" ||
        solverType == "Conjugate gradient" || solverType == "conjugate Gradient" ||
        solverType == "conjugate_gradient" || solverType == "Conjugate_Gradient" ||
        solverType == "Conjugate_gradient" || solverType == "conjugate_Gradient" ||
        solverType == "cg") {
        solverType = "cg";
    }
    else if (solverType == "bicgstab" || solverType == "BiCGSTAB" ||
             solverType == "Bicgstab") {
        solverType = "bicgstab";
    }
    else {
        *errorMessage = "Invalid Iterative Solver";
        return false;
    }

    // Boundary conditions check
    if (sideBC == "periodic" || sideBC == "Periodic" || sideBC == "p") {
        sideBC = "p";
    }
    else if (sideBC == "symmetric" || sideBC == "Symmetric" || sideBC == "s") {
        sideBC = "s";
    }
    else {
        *errorMessage = "Invalid Boundary Conditions";
        return false;
    }

    // Boundary conditions check
    if (method == "mpfa" || method == "MPFA" || method == "m") {
        method = "m";
    }
    else if (method == "eMPFA" || method == "empfa" || method == "EMPFA" || method == "e") {
        method = "e";
    }
    else {
        *errorMessage = "Invalid Method";
        return false;
    }

    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }

    // Checking that, for all the places where we have 2 entries in matCond
    // we have a valid direction matrix with magnitude==1, necessary for the tensor rotation
    bool check = true;
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (long i=0; i<segWS->X(); i++) {
        for (long j = 0; j < segWS->Y(); j++) {
            for (long k = 0; k < segWS->Z(); k++) {
                if(matCond[segWS->at(i, j, k)].size() == 2) {
                    if(direction->at(i, j, k).magnitude() < 0.9){
                        check = false;
                    }
                }
            }
        }
    }
    if(!check){
        *errorMessage = "Missing parts of the orientation field";
        return false;
    }

    if(prescribedBC->size() > 1){
        if(dir == 'x'){
            if(prescribedBC->X() != 2){
                *errorMessage = "PrescribedBC needs to be of thickness 2 in the simulation X direction";
                return false;
            }
            if(prescribedBC->Y() != segWS->Y()){
                *errorMessage = "PrescribedBC Y dimension is not the same as the Workspace";
                return false;
            }
            if(prescribedBC->Z() != segWS->Z()){
                *errorMessage = "PrescribedBC Z dimension is not the same as the Workspace";
                return false;
            }
        } else if (dir == 'y'){
            if(prescribedBC->Y() != 2){
                *errorMessage = "PrescribedBC needs to be of thickness 2 in the simulation Y direction";
                return false;
            }
            if(prescribedBC->X() != segWS->X()){
                *errorMessage = "PrescribedBC X dimension is not the same as the Workspace";
                return false;
            }
            if(prescribedBC->Z() != segWS->Z()){
                *errorMessage = "PrescribedBC Z dimension is not the same as the Workspace";
                return false;
            }
        } else {
            if(prescribedBC->Z() != 2){
                *errorMessage = "PrescribedBC needs to be of thickness 2 in the simulation Z direction";
                return false;
            }
            if(prescribedBC->X() != segWS->X()){
                *errorMessage = "PrescribedBC X dimension is not the same as the Workspace";
                return false;
            }
            if(prescribedBC->Y() != segWS->Y()){
                *errorMessage = "PrescribedBC Y dimension is not the same as the Workspace";
                return false;
            }
        }
    }

    return true;
}
