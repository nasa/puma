#ifndef FV_ANISTROPIC_MATRIX_H
#define FV_ANISTROPIC_MATRIX_H

#include "vector.h"
#include "AMatrix.h"

#include <vector>
#include <utility>


class FV_anisotropic_AMatrix : public AMatrix
{
public:
    FV_anisotropic_AMatrix(puma::Matrix<std::vector<double>> *E_Matrix, puma::Matrix<std::vector<double>> *flux_Matrix, std::vector<long> startend, std::string sideBC, int numThreads);
    bool A_times_X(puma::Matrix<double> *x, puma::Matrix<double> *r) override;
    bool computeQuadrantFluxes(puma::Matrix<double> *x);

private:
    puma::Matrix<std::vector<double>> *E_Matrix;
    puma::Matrix<std::vector<double>> *flux_Matrix;
    int numThreads;
    int X,Y,Z;
    std::vector<long> startend;
    std::string sideBC;

    long indexAt(long index, long size);
};

#endif // FV_ANISTROPIC_MATRIX_H
