#ifndef artificialflux_Diffusion_H
#define artificialflux_Diffusion_H

#include "artificialflux_AMatrix.h"
#include "workspace.h"
#include "vector.h"
#include "AMatrix.h"
#include "logger.h"
#include "iterativesolvers.h"
#include "fv_boundarycondition.h"
#include "fv_periodicboundary.h"
#include "fv_symmetricboundary.h"
#include "fv_constantvalueboundary.h"

#include <map>


class ArtificialFlux_Diffusion
{
public:
    ArtificialFlux_Diffusion(puma::Matrix<double> *T, puma::MatVec3<double> *q, puma::Matrix<double> *kMat,
                 std::string sideBC, std::string solverType, char dir, double solverTol, int solverMaxIt, bool print, int numThreads);
    puma::Vec3<double> compute_DiffusionCoefficient();

    static bool computeKMatrix(puma::Workspace *segWS, std::map<int, double> matCond, puma::Matrix<double> *kMat, int numThreads);

private:
    puma::Vec3<double> diffusionCoefficient;
    puma::Matrix<double> *T;
    puma::MatVec3<double> *q;
    puma::Matrix<double> *kMat;
    puma::Matrix<double> b;
    int X,Y,Z;

    std::string sideBC;
    std::string solverType;
    char dir;
    double solverTol;
    int solverMaxIt;
    bool print;
    int numThreads;

    std::vector<FV_BoundaryCondition*> boundaries;

    bool setupBoundaries();
    bool setInitialConditions();

    bool runIterativeSolver(ArtificialFlux_AMatrix *A);

    bool addLinearProfile();


};

#endif // artificialflux_Diffusion_H
