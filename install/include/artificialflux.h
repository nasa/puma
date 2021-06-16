#ifndef artificialflux_H
#define artificialflux_H

#include "materialproperty.h"
#include "orientation.h"
#include "workspace.h"
#include "vector.h"
#include "artificialflux_diffusion.h"

#include <map>


namespace puma {

bool compute_orientationAF(puma::Workspace *grayWS, puma::MatVec3<double> *q, puma::Cutoff cutoff, double solverTol, int solverMaxIt, bool print = true, int numThreads = 0);

}


class ArtificialFlux : public MaterialProperty
{
public:

    ArtificialFlux(puma::Workspace *segWS, puma::MatVec3<double> *qDirs,
                   double solverTol, int solverMaxIt, bool print, int numThreads);

    bool compute();


private:

    //! A pointer to a segmented workspace containing the domain.
    puma::Workspace *segWS;

    //! A pointer to a puma matrix to store the resulting temperature field.
    puma::MatVec3<double> *qDirs;

    //! The convergence criterion for the iterative solver used.
    double solverTol;

    //! The maximum number of iterations the solver may execute before exiting.
    int solverMaxIt;

    //! Specifies whether the the number of iterations and residual are printed after each iteration of the solver.
    bool print;

    //! Specifies the number of threads used for the simulation.
    int numThreads;

    //! Logs the inputs into a file.
    /*!
     * \return a boolean indicating the function executed without errors.
     */
    bool logInput() override;

    //! Logs the output into a file.
    /*!
     * \return a boolean indicating the function executed without errors.
     */
    bool logOutput() override;

    //! Checks the input for errors.
    /*!
     * \param errorMessage a pointer to string where the error message, if any, is stored.
     * \return a boolean indicating the function executed without errors.
     */
    bool errorCheck(std::string *errorMessage) override;

};


#endif // artificialflux_H
