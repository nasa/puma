#include "ej_diffusion.h"



EJ_Diffusion::EJ_Diffusion(puma::Matrix<double> *T, puma::Matrix<double> *kMat, char dir, double solverTol, int solverMaxIt, bool print, int numThreads)
{
    this->T = T;
    this->kMat = kMat;
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


EJ_Diffusion::EJ_Diffusion(puma::Matrix<double> *T, puma::Matrix<double> *kMat, char dir, double solverTol, int solverMaxIt, bool print, puma::Printer *printer, int numThreads)
{
    this->T = T;
    this->kMat = kMat;
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

EJ_Diffusion::~EJ_Diffusion() {
    if(printer && delPrinter) {
        delete printer;
    }
}

bool EJ_Diffusion::computeKMatrix(puma::Workspace *segWS, std::map<int, double> matCond, puma::Matrix<double> *kMat, int numThreads) {
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

puma::Vec3<double> EJ_Diffusion::compute_DiffusionCoefficient()
{

    EJ_AMatrix A(kMat,dir,numThreads);
    if(!A.setup()) {
        return puma::Vec3<double>(-1,-1,-1);
    }

    if(!setupJAndF(&A)) {
        return puma::Vec3<double>(-1,-1,-1);
    }

    if(!runIterativeSolver(&A)) {
        return puma::Vec3<double>(-1,-1,-1);
    }
    A.computeHomogenizedSolution(T,&J);
    puma::Vec3<double> K = A.computeConductivity(T,&J);
    addLinearProfile();
    A.free();
    return K;
}

bool EJ_Diffusion::setupJAndF(EJ_AMatrix *A) {
    J.resize(A->getNumInterfaces(),1,1,0);
    A->getF(&F);
    return true;
}

bool EJ_Diffusion::runIterativeSolver(EJ_AMatrix *A) {
    IterativeSolver::BiCGSTAB(A,&J,&F,solverTol,solverMaxIt,print, printer,numThreads);
    return true;
}

bool EJ_Diffusion::addLinearProfile() {
    if(dir == 'x' || dir == 'X') {
        for(long i=0;i<X;i++) {
            for(long j=0;j<Y;j++) {
                for(long k=0;k<Z;k++) {
                    double h = 1./(double)T->X();
                    double T0 = 0.5*h;
                    (*T)(i,j,k) += T0+(double)i*h;
                }
            }
        }
    }

    if(dir == 'y' || dir == 'Y') {
        for(long i=0;i<X;i++) {
            for(long j=0;j<Y;j++) {
                for(long k=0;k<Z;k++) {
                    double h = 1./(double)T->Y();
                    double T0 = 0.5*h;
                    (*T)(i,j,k) += T0+(double)j*h;
                }
            }
        }
    }

    if(dir == 'z' || dir == 'Z') {
        for(long i=0;i<X;i++) {
            for(long j=0;j<Y;j++) {
                for(long k=0;k<Z;k++) {
                    double h = 1./(double)T->Z();
                    double T0 = 0.5*h;
                    (*T)(i,j,k) += T0+(double)k*h;
                }
            }
        }
    }

    return true;
}
