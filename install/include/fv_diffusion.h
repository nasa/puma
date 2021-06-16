#ifndef FV_DIFFUSION_H
#define FV_DIFFUSION_H

#include "workspace.h"
#include "vector.h"
#include "fv_AMatrix.h"
#include "iterativesolvers.h"
#include "fv_periodicboundary.h"
#include "fv_symmetricboundary.h"
#include "fv_constantvalueboundary.h"
#include "fv_boundarycondition.h"
#include "Printer.h"

#include <map>


class FV_Diffusion
{
public:
    FV_Diffusion(puma::Matrix<double> *T, puma::Matrix<double> *kMat,
                 std::string sideBC, std::string solverType, char dir, double solverTol, int solverMaxIt, bool print, int numThreads);
    FV_Diffusion(puma::Matrix<double> *T, puma::Matrix<double> *kMat,
                 std::string sideBC, std::string solverType, char dir, double solverTol, int solverMaxIt, bool print, puma::Printer *printer, int numThreads);
    ~FV_Diffusion();
    puma::Vec3<double> compute_DiffusionCoefficient();

    static bool computeKMatrix(puma::Workspace *segWS, std::map<int, double> matCond, puma::Matrix<double> *kMat, int numThreads);

private:
    puma::Vec3<double> diffusionCoefficient;
    puma::Matrix<double> *T;
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

    puma::Printer *printer;
    bool delPrinter;

    std::vector<FV_BoundaryCondition*> boundaries;

    bool setupBoundaries();
    bool setInitialConditions();

    bool runIterativeSolver(FV_AMatrix *A);

    bool addLinearProfile();


};

#endif // FV_DIFFUSION_H
