#ifndef FV_PERIODICBOUNDARY_H
#define FV_PERIODICBOUNDARY_H

#include "fv_boundarycondition.h"
#include "matrix.h"


class FV_PeriodicBoundary : public FV_BoundaryCondition
{
public:
    FV_PeriodicBoundary(int X, int Y, int Z);
    double getK_at(long i, long j, long k, puma::Matrix<double> *kMat) override;
    double getX_at(long i, long j, long k,puma::Matrix<double> *T) override;

private:
    int X, Y, Z;
    //  int constantIndex;

};

#endif // FV_PERIODICBOUNDARY_H
