#include "fv_periodicboundary.h"


FV_PeriodicBoundary::FV_PeriodicBoundary( int X, int Y, int Z) {
    this->X = X;
    this->Y = Y;
    this->Z = Z;
}


double FV_PeriodicBoundary::getK_at(long i, long j, long k, puma::Matrix<double> *kMat) {
    if(i==-1) {
        return kMat->at(X-1,j,k);
    }
    if(j==-1) {
        return kMat->at(i,Y-1,k);
    }
    if(k==-1) {
        return kMat->at(i,j,Z-1);
    }
    if(i==X) {
        return kMat->at(0,j,k);
    }
    if(j==Y) {
        return kMat->at(i,0,k);
    }
    if(k==Z) {
        return kMat->at(i,j,0);
    }
    return kMat->at(i,j,k);
}

double FV_PeriodicBoundary::getX_at(long i, long j, long k,puma::Matrix<double> *T) {
    if(i==-1) {
        return T->at(X-1,j,k);
    }
    if(j==-1) {
        return T->at(i,Y-1,k);
    }
    if(k==-1) {
        return T->at(i,j,Z-1);
    }
    if(i==X) {
        return T->at(0,j,k);
    }
    if(j==Y) {
        return T->at(i,0,k);
    }
    if(k==Z) {
        return T->at(i,j,0);
    }
    return T->at(i,j,k);
}
