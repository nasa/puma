#ifndef FV_ANISOTROPIC_DIFFUSION_H
#define FV_ANISOTROPIC_DIFFUSION_H

#include "workspace.h"
#include "vector.h"
#include "logger.h"
#include "iterativesolvers.h"
#include "fv_anisotropic_AMatrix.h"

#include <Dense>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <cmath>
#include <utility>


class FV_anisotropic_Diffusion
{
public:
    FV_anisotropic_Diffusion(puma::Matrix<double> *T, puma::MatVec3<double> *q, puma::Matrix< std::vector<double> > *kMat,
                             std::string sideBC, puma::Matrix<double> *prescribedBC, std::string solverType, char dir, double voxelLength,
                             double solverTol, int solverMaxIt, bool print, std::string method, int numThreads);
    puma::Vec3<double> compute_DiffusionCoefficient();

    static bool computeKMatrix(puma::Workspace *segWS, std::map<int,std::vector<double>> matCond,
                               puma::Matrix< std::vector<double> > *kMat, puma::MatVec3<double> *direction, bool print, int numThreads);

    static std::vector<double> matrixMultiply(std::vector<double> *matrix1, int X1, int Y1, std::vector<double> *matrix2, int X2);

private:
    puma::Vec3<double> diffusionCoefficient;
    puma::Matrix<double> *T;
    puma::MatVec3<double> *q;
    puma::Matrix < std::vector < double > > *kMat;
    puma::Matrix<double> b;
    int X,Y,Z;

    puma::Matrix< std::vector<double> > flux_Matrix;
    puma::Matrix< std::vector<double> > E_Matrix;

    std::string sideBC;
    puma::Matrix<double> *prescribedBC;
    std::string solverType;
    char dir;
    double voxelLength;
    double solverTol;
    int solverMaxIt;
    bool print;
    std::string method;
    int numThreads;

    bool initializeLinearProfile();

    std::vector<long> startend;

    bool computeEMatrix();

    bool runIterativeSolver(FV_anisotropic_AMatrix *A);

    bool computeLengthFluxes(puma::Vec3<double> *fluxes, puma::Vec3<double> *length);

    long indexAt(long index, long size);
};

#endif // FV_ANISOTROPIC_DIFFUSION_H
