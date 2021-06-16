#ifndef FV_ANISOTROPIC_THERMALCONDUCTIVITY_H
#define FV_ANISOTROPIC_THERMALCONDUCTIVITY_H

#include "materialproperty.h"
#include "workspace.h"
#include "vector.h"
#include "logger.h"
#include "fv_anisotropic_diffusion.h"

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <utility>


namespace puma {
    // no orientation matrix
    puma::Vec3<double> compute_FVanisotropicThermalConductivity(Workspace *grayWS, puma::Matrix<double> *T, puma::MatVec3<double> *q, const std::map<int, std::vector<double> >& matCond,
                                                                std::string method, std::string sideBC, std::string solverType, char dir, double solverTol,
                                                                int solverMaxIt, bool print = true, int numThreads = 0);

    // with orientation matrix
    puma::Vec3<double> compute_FVanisotropicThermalConductivity(Workspace *grayWS, puma::Matrix<double> *T, puma::MatVec3<double> *q, const std::map<int, std::vector<double> >& matCond,
                                                                puma::MatVec3<double> *direction, std::string method, std::string sideBC, std::string solverType, char dir, double solverTol,
                                                                int solverMaxIt, bool print = true, int numThreads = 0);

    // no orientation matrix with prescribedBC
    puma::Vec3<double> compute_FVanisotropicThermalConductivity(Workspace *grayWS, puma::Matrix<double> *T, puma::MatVec3<double> *q, const std::map<int, std::vector<double> >& matCond,
                                                                std::string method, std::string sideBC, puma::Matrix<double> *prescribedBC, std::string solverType, char dir, double solverTol,
                                                                int solverMaxIt, bool print = true, int numThreads = 0);

    // with orientation matrix and prescribedBC
    puma::Vec3<double> compute_FVanisotropicThermalConductivity(Workspace *grayWS, puma::Matrix<double> *T, puma::MatVec3<double> *q, const std::map<int, std::vector<double>>& matCond,
                                                                puma::MatVec3<double> *direction, std::string method, std::string sideBC, puma::Matrix<double> *prescribedBC,
                                                                std::string solverType, char dir, double solverTol, int solverMaxIt, bool print = true, int numThreads = 0);
}


class FV_anisotropic_ThermalConductivity : MaterialProperty
{
public:

    FV_anisotropic_ThermalConductivity(puma::Workspace *segWS, puma::Matrix<double> *T, puma::MatVec3<double> *q, std::map<int, std::vector<double> > matCond,
                                       puma::MatVec3<double> *direction, std::string method, std::string sideBC, puma::Matrix<double> *prescribedBC, std::string solverType, char dir, double solverTol,
                                       int solverMaxIt, bool print, int numThreads);
    puma::Vec3<double> compute();


private:
    puma::Workspace *segWS;
    puma::Matrix<double> *T;
    puma::MatVec3<double> *q;
    std::map<int, std::vector<double> > matCond;
    std::string sideBC;
    puma::Matrix<double> *prescribedBC;
    std::string solverType;
    char dir;
    double solverTol;
    int solverMaxIt;
    bool print;
    std::string method;
    puma::MatVec3<double> *direction;
    int numThreads;

    puma::Vec3<double> anisotropicthermalConductivity;

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};


#endif // FV_ANISOTROPIC_THERMALCONDUCTIVITY_H
