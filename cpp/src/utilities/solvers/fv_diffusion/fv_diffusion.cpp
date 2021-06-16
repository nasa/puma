#include "fv_diffusion.h"


bool FV_Diffusion::computeKMatrix(puma::Workspace *segWS, std::map<int, double> matCond, puma::Matrix<double> *kMat, int numThreads) {
    kMat->resize(segWS->X(),segWS->Y(),segWS->Z(),0);

// TEMPORARY FIX: removing parallelization to this for loop as it intermittently fails on some linux systems
//    omp_set_num_threads(numThreads);
//#pragma omp parallel for
    for (long i=0; i<kMat->X(); i++) {
        for (long j=0; j<kMat->Y(); j++) {
            for (long k=0; k<kMat->Z(); k++) {
                kMat->at(i,j,k) = matCond[segWS->at(i,j,k)];
            }
        }
    }
    return true;
}

FV_Diffusion::FV_Diffusion(puma::Matrix<double> *T, puma::Matrix<double> *kMat,
                           std::string sideBC, std::string solverType, char dir, double solverTol, int solverMaxIt, bool print, int numThreads)
{
    this->T = T;
    this->kMat = kMat;
    this->sideBC = sideBC;
    this->solverType = solverType;
    this->dir = dir;
    this->solverTol = solverTol;
    this->solverMaxIt = solverMaxIt;
    this->print = print;
    this->numThreads = numThreads;
    this->X = kMat->X();
    this->Y = kMat->Y();
    this->Z = kMat->Z();

    printer = new puma::Printer();
    delPrinter = true;

}

FV_Diffusion::FV_Diffusion(puma::Matrix<double> *T, puma::Matrix<double> *kMat,
                           std::string sideBC, std::string solverType, char dir, double solverTol, int solverMaxIt, bool print, puma::Printer *printer, int numThreads)
{
    this->T = T;
    this->kMat = kMat;
    this->sideBC = sideBC;
    this->solverType = solverType;
    this->dir = dir;
    this->solverTol = solverTol;
    this->solverMaxIt = solverMaxIt;
    this->print = print;
    this->numThreads = numThreads;
    this->X = kMat->X();
    this->Y = kMat->Y();
    this->Z = kMat->Z();

    if(printer) {
        this->printer = printer;
        delPrinter = false;
    } else {
        delPrinter = true;
        printer = new puma::Printer();
    }

}

FV_Diffusion::~FV_Diffusion() {
    if(printer && delPrinter) {
        delete printer;
    }
}

puma::Vec3<double> FV_Diffusion::compute_DiffusionCoefficient()
{
    if(!setupBoundaries()) {
        return puma::Vec3<double>(-1,-1,-1);
    }
    if(!setInitialConditions()) {
        return puma::Vec3<double>(-1,-1,-1);
    }

    FV_AMatrix A(kMat,&boundaries,numThreads);

    if(!runIterativeSolver(&A)) {
        return puma::Vec3<double>(-1,-1,-1);
    }

    addLinearProfile();

    puma::Vec3<double> fluxes = A.computeFluxes(T, dir);
    puma::Vec3<double> length(X, Y, Z);

    diffusionCoefficient.x = fluxes.x * length.x;
    diffusionCoefficient.y = fluxes.y * length.y;
    diffusionCoefficient.z = fluxes.z * length.z;

    return diffusionCoefficient;
}


bool FV_Diffusion::setupBoundaries()
{
    boundaries.resize(6);
    if (sideBC.compare("periodic") == 0 || sideBC.compare("Periodic") == 0 ){
        if (dir == 'x' || dir == 'X'){
            boundaries[0] = new FV_ConstantValueBoundary(0, X, Y, Z);
            boundaries[1] = new FV_ConstantValueBoundary(1, X, Y, Z);
            boundaries[2] = new FV_PeriodicBoundary( X, Y, Z);
            boundaries[3] = new FV_PeriodicBoundary( X, Y, Z);
            boundaries[4] = new FV_PeriodicBoundary( X, Y, Z);
            boundaries[5] = new FV_PeriodicBoundary( X, Y, Z);
        }
        else if (dir == 'y' || dir == 'Y') {
            boundaries[0] = new FV_PeriodicBoundary(X, Y, Z);
            boundaries[1] = new FV_PeriodicBoundary( X, Y, Z);
            boundaries[2] = new FV_ConstantValueBoundary(0, X, Y, Z);
            boundaries[3] = new FV_ConstantValueBoundary(1, X, Y, Z);
            boundaries[4] = new FV_PeriodicBoundary( X, Y, Z);
            boundaries[5] = new FV_PeriodicBoundary(X, Y, Z);
        }
        else if (dir == 'z' || dir == 'Z') {
            boundaries[0] = new FV_PeriodicBoundary(X, Y, Z);
            boundaries[1] = new FV_PeriodicBoundary(X, Y, Z);
            boundaries[2] = new FV_PeriodicBoundary(X, Y, Z);
            boundaries[3] = new FV_PeriodicBoundary(X, Y, Z);
            boundaries[4] = new FV_ConstantValueBoundary(0, X, Y, Z);
            boundaries[5] = new FV_ConstantValueBoundary(1, X, Y, Z);
        }
        else {
            printer->print("Finite Volume Diffusion Error: Invalid Direction" );
            return false;
        }
    }
    else if (sideBC.compare("symmetric") == 0 || sideBC.compare("Symmetric") == 0) {
        if (dir == 'x' || dir == 'X'){
            boundaries[0] = new FV_ConstantValueBoundary(0, X, Y, Z);
            boundaries[1] = new FV_ConstantValueBoundary(1, X, Y, Z);
            boundaries[2] = new FV_SymmetricBoundary( X, Y, Z);
            boundaries[3] = new FV_SymmetricBoundary( X, Y, Z);
            boundaries[4] = new FV_SymmetricBoundary( X, Y, Z);
            boundaries[5] = new FV_SymmetricBoundary( X, Y, Z);
        }
        else if (dir == 'y' || dir == 'Y') {
            boundaries[0] = new FV_SymmetricBoundary( X, Y, Z);
            boundaries[1] = new FV_SymmetricBoundary( X, Y, Z);
            boundaries[2] = new FV_ConstantValueBoundary(0, X, Y, Z);
            boundaries[3] = new FV_ConstantValueBoundary(1, X, Y, Z);
            boundaries[4] = new FV_SymmetricBoundary( X, Y, Z);
            boundaries[5] = new FV_SymmetricBoundary( X, Y, Z);
        }
        else if (dir == 'z' || dir == 'Z') {
            boundaries[0] = new FV_SymmetricBoundary( X, Y, Z);
            boundaries[1] = new FV_SymmetricBoundary( X, Y, Z);
            boundaries[2] = new FV_SymmetricBoundary( X, Y, Z);
            boundaries[3] = new FV_SymmetricBoundary( X, Y, Z);
            boundaries[4] = new FV_ConstantValueBoundary(0, X, Y, Z);
            boundaries[5] = new FV_ConstantValueBoundary(1, X, Y, Z);
        }
        else {
            printer->print("Finite Volume Diffusion Error: Invalid Direction" );
            return false;
        }
    }
    else {
        printer->print("Finite Volume Diffusion Error: Invalid Boundary Condition" );
        return false;
    }
    return true;
}

bool FV_Diffusion::setInitialConditions() {

    T->resize(X,Y,Z,0);

    b.resize(X,Y,Z);

    if (dir == 'x' || dir == 'X') {
        double h = 1.0/T->X();
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (long i=0; i<X; i++) {
            for (long j=0; j<Y; j++) {
                for (long k=0; k<Z; k++) {
                    double kMinus = boundaries[0]->getK_at(i-1,j,k, kMat);
                    double kPlus = boundaries[1]->getK_at(i+1,j,k, kMat);
                    double kCenter = kMat->at(i,j,k);
                    b.at(i,j,k) = - kCenter * h/2.0 * ( ( (kPlus - kCenter)/(kPlus + kCenter) ) + ( (kCenter - kMinus )/(kCenter + kMinus) ) );
                    if(b.at(i,j,k) != b.at(i,j,k)) b.at(i,j,k) = 0;
                }
            }
        }
    }
    else if (dir == 'y' || dir == 'Y'){
        double h = 1./T->Y();
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (long i=0; i<X; i++) {
            for (long j=0; j<Y; j++) {
                for (long k=0; k<Z; k++) {
                    double kMinus = boundaries[2]->getK_at(i,j-1,k, kMat);
                    double kPlus = boundaries[3]->getK_at(i,j+1,k, kMat);
                    double kCenter = kMat->at(i,j,k);
                    b.at(i,j,k) = - kCenter * h/2.0 * ( ( (kPlus - kCenter)/(kPlus + kCenter) ) + ( (kCenter - kMinus )/(kCenter + kMinus) ) );
                    if(b.at(i,j,k) != b.at(i,j,k)) b.at(i,j,k) = 0;
                }
            }
        }
    }
    else if (dir == 'z' || dir == 'Z'){
        double h = 1./T->Z();
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (long i=0; i<X; i++) {
            for (long j=0; j<Y; j++) {
                for (long k=0; k<Z; k++) {
                    double kMinus = boundaries[4]->getK_at(i,j,k-1, kMat);
                    double kPlus = boundaries[5]->getK_at(i,j,k+1, kMat);
                    double kCenter = kMat->at(i,j,k);
                    b.at(i,j,k) = - kCenter * h/2.0 * ( ( (kPlus - kCenter)/(kPlus + kCenter) ) + ( (kCenter - kMinus )/(kCenter + kMinus) ) );
                    if(b.at(i,j,k) != b.at(i,j,k)) b.at(i,j,k) = 0;
                }
            }
        }
    }
    else {
        printer->print("Finite Volume Diffusion Error: Invalid Direction" );
        return false;
    }
    return true;
}


bool FV_Diffusion::runIterativeSolver(FV_AMatrix *A) {
    if (solverType.compare("bicgstab") == 0 || solverType.compare("BiCGSTAB") == 0 || solverType.compare("Bicgstab") == 0){
        IterativeSolver::BiCGSTAB(A,T,&b,solverTol,solverMaxIt,print, printer, numThreads);
    }
    else if (solverType.compare("conjugategradient") == 0 || solverType.compare("ConjugateGradient") == 0 ||
             solverType.compare("Conjugategradient") == 0 || solverType.compare("conjugateGradient") == 0 ||
             solverType.compare("conjugate gradient") == 0 || solverType.compare("Conjugate Gradient") == 0 ||
             solverType.compare("Conjugate gradient") == 0 || solverType.compare("conjugate Gradient") == 0 ||
             solverType.compare("conjugate_gradient") == 0 || solverType.compare("Conjugate_Gradient") == 0 ||
             solverType.compare("Conjugate_gradient") == 0 || solverType.compare("conjugate_Gradient") == 0 ||
             solverType.compare("cg") == 0) {
        IterativeSolver::ConjugateGradient(A,T,&b,solverTol,solverMaxIt,print,printer, numThreads);
    }

    return true;
}

bool FV_Diffusion::addLinearProfile() {
    if(dir == 'x' || dir == 'X') {
        for(int i=0;i<X;i++) {
            for(int j=0;j<Y;j++) {
                for(int k=0;k<Z;k++) {
                    double h = 1./T->X();
                    double T0 = 0.5*h;
                    (*T)(i,j,k) += T0+i*h;
                }
            }
        }
    }

    if(dir == 'y' || dir == 'Y') {
        for(int i=0;i<X;i++) {
            for(int j=0;j<Y;j++) {
                for(int k=0;k<Z;k++) {
                    double h = 1./T->Y();
                    double T0 = 0.5*h;
                    (*T)(i,j,k) += T0+j*h;
                }
            }
        }
    }

    if(dir == 'z' || dir == 'Z') {
        for(int i=0;i<X;i++) {
            for(int j=0;j<Y;j++) {
                for(int k=0;k<Z;k++) {
                    double h = 1./T->Z();
                    double T0 = 0.5*h;
                    (*T)(i,j,k) += T0+k*h;
                }
            }
        }
    }

    return true;
}

