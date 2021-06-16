#ifndef FV_BOUNDARYCONDITION_H
#define FV_BOUNDARYCONDITION_H

#include "matrix.h"


class FV_BoundaryCondition
{
public:
    virtual double getK_at(long i, long j, long k, puma::Matrix<double> *kMat) = 0;
    virtual double getX_at(long i, long j, long k, puma::Matrix<double> *x) = 0;
};

#endif // FV_BOUNDARYCONDITION_H
