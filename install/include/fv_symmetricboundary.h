#ifndef FV_SymmetricBoundary_H
#define FV_SymmetricBoundary_H

#include "fv_boundarycondition.h"
#include "matrix.h"


class FV_SymmetricBoundary : public FV_BoundaryCondition
{
public:
    FV_SymmetricBoundary( int X, int Y, int Z);
    double getK_at(long i, long j, long k, puma::Matrix<double> *kMat) override;
    double getX_at(long i, long j, long k,puma::Matrix<double> *T) override;

private:
    int X, Y, Z;

};

#endif // FV_SymmetricBoundary_H
