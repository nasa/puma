#include "artificialflux_diffusion.h"

#include <utility>


bool ArtificialFlux_Diffusion::computeKMatrix(puma::Workspace *segWS, std::map<int, double> matCond, puma::Matrix<double> *kMat, int numThreads) {
    kMat->resize(segWS->X(),segWS->Y(),segWS->Z(),0);
    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (long i=0; i<kMat->X(); i++) {
        for (long j=0; j<kMat->Y(); j++) {
            for (long k=0; k<kMat->Z(); k++) {
                kMat->operator ()(i,j,k) = matCond[(*segWS)(i,j,k)];
            }
        }
    }
    return true;
}

ArtificialFlux_Diffusion::ArtificialFlux_Diffusion(puma::Matrix<double> *T, puma::MatVec3<double> *q, puma::Matrix<double> *kMat,
                           std::string sideBC, std::string solverType, char dir, double solverTol, int solverMaxIt, bool print, int numThreads)
{
    this->T = T;
    this->q = q;
    this->kMat = kMat;
    this->sideBC = std::move(sideBC);
    this->solverType = std::move(solverType);
    this->dir = dir;
    this->solverTol = solverTol;
    this->solverMaxIt = solverMaxIt;
    this->print = print;
    this->numThreads = numThreads;
    this->X = (int)kMat->X();
    this->Y = (int)kMat->Y();
    this->Z = (int)kMat->Z();
}

puma::Vec3<double> ArtificialFlux_Diffusion::compute_DiffusionCoefficient()
{
    if(!setupBoundaries()) {
        return puma::Vec3<double>(-1,-1,-1);
    }
    if(!setInitialConditions()) {
        return puma::Vec3<double>(-1,-1,-1);
    }

    ArtificialFlux_AMatrix A(q,kMat,&boundaries,numThreads);

    if(!runIterativeSolver(&A)) {
        return puma::Vec3<double>(-1,-1,-1);
    }

    addLinearProfile();

    puma::Vec3<double> fluxes = A.computeFluxes(T, q, dir);
    puma::Vec3<double> length(X, Y, Z);

    diffusionCoefficient.x = fluxes.x * length.x;
    diffusionCoefficient.y = fluxes.y * length.y;
    diffusionCoefficient.z = fluxes.z * length.z;

    return diffusionCoefficient;
}


bool ArtificialFlux_Diffusion::setupBoundaries()
{
    boundaries.resize(6);
    if (sideBC == "periodic" || sideBC == "Periodic" ){
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
            std::cout << "Finite Volume Diffusion Error: Invalid Direction" << std::endl;
            return false;
        }
    }
    else if (sideBC == "symmetric" || sideBC == "Symmetric") {
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
            std::cout << "Finite Volume Diffusion Error: Invalid Direction" << std::endl;
            return false;
        }
    }
    else {
        std::cout << "Finite Volume Diffusion Error: Invalid Boundary Condition" << std::endl;
        return false;
    }
    return true;
}

bool ArtificialFlux_Diffusion::setInitialConditions() {

    T->resize(X,Y,Z,0);
    q->resize(X,Y,Z,puma::Vec3<double>(0,0,0));

    b.resize(X,Y,Z);

    if (dir == 'x' || dir == 'X') {
        double h = 1.0/(double)T->X();
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
        double h = 1./(double)T->Y();
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
        double h = 1./(double)T->Z();
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for (long i=0; i<X; i++) {
            for (long j=0; j<Y; j++) {
                for (long k=0; k<Z; k++) {
                    boundaries[4]->getX_at(i,j,k, kMat);
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
        std::cout << "Finite Volume Diffusion Error: Invalid Direction" << std::endl;
        return false;
    }
    return true;
}


bool ArtificialFlux_Diffusion::runIterativeSolver(ArtificialFlux_AMatrix *A) {
    if (solverType == "bicgstab" || solverType == "BiCGSTAB" || solverType == "Bicgstab"){
        IterativeSolver::BiCGSTAB(A,T,&b,solverTol,solverMaxIt,print,numThreads);
    }
    else if (solverType == "conjugategradient" || solverType == "ConjugateGradient" ||
             solverType == "Conjugategradient" || solverType == "conjugateGradient" ||
             solverType == "conjugate gradient" || solverType == "Conjugate Gradient" ||
             solverType == "Conjugate gradient" || solverType == "conjugate Gradient" ||
             solverType == "conjugate_gradient" || solverType == "Conjugate_Gradient" ||
             solverType == "Conjugate_gradient" || solverType == "conjugate_Gradient" ||
             solverType == "cg") {
        IterativeSolver::ConjugateGradient(A,T,&b,solverTol,solverMaxIt,print,numThreads);
    }

    return true;
}

bool ArtificialFlux_Diffusion::addLinearProfile() {
    if(dir == 'x' || dir == 'X') {
        for(int i=0;i<X;i++) {
            for(int j=0;j<Y;j++) {
                for(int k=0;k<Z;k++) {
                    double h = 1./(double)T->X();
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
                    double h = 1./(double)T->Y();
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
                    double h = 1./(double)T->Z();
                    double T0 = 0.5*h;
                    (*T)(i,j,k) += T0+k*h;
                }
            }
        }
    }

    return true;
}

