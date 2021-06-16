#ifndef FV_CONSTANTVALUEBOUNDARY_H
#define FV_CONSTANTVALUEBOUNDARY_H

#include "fv_boundarycondition.h"
#include "matrix.h"


class FV_ConstantValueBoundary : public FV_BoundaryCondition
{
public:
    FV_ConstantValueBoundary( double value, int X, int Y, int Z);
    double getK_at(long i, long j, long k, puma::Matrix<double> *kMat) override;
    double getX_at(long i, long j, long k,puma::Matrix<double> *T) override;

private:
    double value;
    int X, Y, Z;
  //  int constantIndex;

};


#endif // FV_CONSTANTVALUEBOUNDARY_H
