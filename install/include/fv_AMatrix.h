#ifndef FV_AMatrix_H
#define FV_AMatrix_H

#include "vector.h"
#include "fv_boundarycondition.h"
#include "AMatrix.h"

#ifdef MAC
#include <numeric>
#elif LINUX
#include <parallel/numeric>
#endif


class FV_AMatrix : public AMatrix
{
public:
    FV_AMatrix(puma::Matrix<double> *kMat, std::vector<FV_BoundaryCondition*> *bcs, int numThreads);
    bool A_times_X(puma::Matrix<double> *x, puma::Matrix<double> *r) override;
    puma::Vec3<double> computeFluxes(puma::Matrix<double> *x, char dir );

private:
    std::vector<FV_BoundaryCondition*> *bcs;
    puma::Matrix<double> *kMat;
    puma::Matrix<double> kX;
    puma::Matrix<double> kY;
    puma::Matrix<double> kZ;
    int numThreads;
    int X,Y,Z;

    bool setup_KMinMax();
    bool setup_KMM_Interior();
    bool setup_KMM_Boundaries();

    void computeInteriorResidual(puma::Matrix<double> *x, puma::Matrix<double> *r);
    void computeBoundaryResidual(puma::Matrix<double> *x, puma::Matrix<double> *r);
};

#endif // FV_AMatrix_H
