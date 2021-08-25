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

    /*! Compute the effective thermal conductivity with anisotropic local phases (GLOBALLY Anisotropic Materials - no direction matrix)
    \param grayWS input workspace
    \param T Temperature field
    \param q Flux field
    \param matCond Local conductivity of the phases
    \param method which discretization method to use ('mpfa', 'empfa')
    \param sideBC what side boundary conditions ('p'eriodic, 's'ymmetric)
    \param solverType which iterative solver to use ('cg', 'bicgstab')
    \param dir in what direction the conductivity has to be homogenized ('x', 'y', 'z')
    \param solverTol the solver tolerance
    \param solverMaxIt maximum solver iteration
    \param print whether to print progress, optional
    \param numThreads number to threads to use, optional
    \return the row of conductivity in the effective conductivity tensor, depending on the dir ('x' --> first row, and so on)
    */
    puma::Vec3<double> compute_FVanisotropicThermalConductivity(Workspace *grayWS, puma::Matrix<double> *T, puma::MatVec3<double> *q, const std::map<int, std::vector<double> >& matCond,
                                                                std::string method, std::string sideBC, std::string solverType, char dir, double solverTol,
                                                                int solverMaxIt, bool print = true, int numThreads = 0);

    /*! Compute the effective thermal conductivity with anisotropic local phases (GLOBALLY Anisotropic Materials - no direction matrix, with prescribedBC)
    \param grayWS input workspace
    \param T Temperature field
    \param q Flux field
    \param matCond Local conductivity of the phases
    \param method which discretization method to use ('mpfa', 'empfa')
    \param sideBC what side boundary conditions ('p'eriodic, 's'ymmetric)
    \param prescribedBC impose custom dirichlet BC impose on the sides. It has to be of size 2 in the simulation direction, e.g. prescribedBC(2, Y, Z, 0) for dirichlet on X sides when dir='x'
    \param solverType which iterative solver to use ('cg', 'bicgstab')
    \param dir in what direction the conductivity has to be homogenized ('x', 'y', 'z')
    \param solverTol the solver tolerance
    \param solverMaxIt maximum solver iteration
    \param print whether to print progress, optional
    \param numThreads number to threads to use, optional
    \return the row of conductivity in the effective conductivity tensor, depending on the dir ('x' --> first row, and so on)
    */
    puma::Vec3<double> compute_FVanisotropicThermalConductivity(Workspace *grayWS, puma::Matrix<double> *T, puma::MatVec3<double> *q, const std::map<int, std::vector<double> >& matCond,
                                                                std::string method, std::string sideBC, puma::Matrix<double> *prescribedBC, std::string solverType, char dir, double solverTol,
                                                                int solverMaxIt, bool print = true, int numThreads = 0);

    /*! Compute the effective thermal conductivity with anisotropic local phases (LOCALLY Anisotropic Materials - with direction matrix)
    \param grayWS input workspace
    \param T Temperature field
    \param q Flux field
    \param matCond Local conductivity of the phases
    \param direction the local orientation field throughout the domain
    \param method which discretization method to use ('mpfa', 'empfa')
    \param sideBC what side boundary conditions ('p'eriodic, 's'ymmetric)
    \param solverType which iterative solver to use ('cg', 'bicgstab')
    \param dir in what direction the conductivity has to be homogenized ('x', 'y', 'z')
    \param solverTol the solver tolerance
    \param solverMaxIt maximum solver iteration
    \param print whether to print progress, optional
    \param numThreads number to threads to use, optional
    \return the row of conductivity in the effective conductivity tensor, depending on the dir ('x' --> first row, and so on)
    */
    puma::Vec3<double> compute_FVanisotropicThermalConductivity(Workspace *grayWS, puma::Matrix<double> *T, puma::MatVec3<double> *q, const std::map<int, std::vector<double> >& matCond,
                                                                puma::MatVec3<double> *direction, std::string method, std::string sideBC, std::string solverType, char dir, double solverTol,
                                                                int solverMaxIt, bool print = true, int numThreads = 0);

    /*! Compute the effective thermal conductivity with anisotropic local phases (GLOBALLY Anisotropic Materials - with direction matrix, with prescribedBC)
    \param grayWS input workspace
    \param T Temperature field
    \param q Flux field
    \param matCond Local conductivity of the phases
    \param direction the local orientation field throughout the domain
    \param method which discretization method to use ('mpfa', 'empfa')
    \param sideBC what side boundary conditions ('p'eriodic, 's'ymmetric)
    \param prescribedBC impose custom dirichlet BC impose on the sides. It has to be of size 2 in the simulation direction, e.g. prescribedBC(2, Y, Z, 0) for dirichlet on X sides when dir='x'
    \param solverType which iterative solver to use ('cg', 'bicgstab')
    \param dir in what direction the conductivity has to be homogenized ('x', 'y', 'z')
    \param solverTol the solver tolerance
    \param solverMaxIt maximum solver iteration
    \param print whether to print progress, optional
    \param numThreads number to threads to use, optional
    \return the row of conductivity in the effective conductivity tensor, depending on the dir ('x' --> first row, and so on)
    */
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
