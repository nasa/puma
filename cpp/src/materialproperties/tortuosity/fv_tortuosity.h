#ifndef FV_Tortuosity_H
#define FV_Tortuosity_H

#include "materialproperty.h"
#include "workspace.h"
#include "vector.h"
#include "fv_diffusion.h"
#include "mp_volumefraction.h"

#include <map>


namespace puma {

//! computes tortuosity from a grayscale workspace using the explicit jump method.
/*!
 * \param grayWS a grayscale workspace containing the domain.
 * \param C a pointer to a puma matrix to store the resulting Concentration field.
 * \param lowVoid is the lowest grayscale value in the void phase.
 * \param highVoid is the lowest grayscale value in the void phase.
 * \param dir a char specifying which direction to run the simulation in (should be 'x', 'y', or 'z').
 * \param solverTol a double specifying the convergence criterion for the iterative solver used.
 * \param solverMaxIt an integer specifying the maximum number of iterations the solver may execute before exiting.
 * \param print a boolean which specifies whether the the number of iterations and residual are printed after each iteration of the solver.
 * \param numThreads an integer which specifies the number of threads used for the simulation.
 * \return a puma vector containing the tortuosity in the x, y, and z directions.
 */
puma::Vec3<double> compute_FVTortuosity(Workspace *grayWS, puma::Matrix<double> *C, puma::Cutoff cutoff,
                                        std::string sideBC, std::string solverType, char dir, double solverTol,
                                        int solverMaxIt, bool print, int numThreads = 0);
}

//! A class for computing tortuosity using the explicit jump method.
/*!
 *   An FV_Tortuosity object is created and used by the compute_FVTortuosity() functions.
 *   \sa compute_FVTortuosity()
 */
class FV_Tortuosity : MaterialProperty
{
public:

    //! A constructor which takes in the parameters used in calculating tortuosity from the explicit jump method.
    /*!
     * \param segWS a segmented workspace containing the domain.
     * \param C a pointer to a puma matrix to store the resulting Concentration field.
     * \param voidLabel is an int which specified the void phase in the Segmented Workspace (usually 0).
     * \param dir a char specifying which direction to run the simulation in (should be 'x', 'y', or 'z').
     * \param solverTol a double specifying the convergence criterion for the iterative solver used.
     * \param solverMaxIt an integer specifying the maximum number of iterations the solver may execute before exiting.
     * \param print a boolean which specifies whether the the number of iterations and residual are printed after each iteration of the solver.
     * \param numThreads an integer which specifies the number of threads used for the simulation.
     */
    FV_Tortuosity(puma::Workspace *segWS, puma::Matrix<double> *C, int voidSpace,
                  std::string sideBC, std::string solverType, char dir, double solverTol,
                  int solverMaxIt, bool print, int numThreads);

    //! computes the tortuosity using the parameters specified when constructing the object.
    /*!
     * \return a puma vector containing the tortuosity in the x, y, and z directions.
     */
    puma::Vec3<double> compute();


private:

    //! A pointer to a segmented workspace containing the domain.
    puma::Workspace *segWS;

    //! A pointer to a puma matrix to store the resulting temperature field.
    puma::Matrix<double> *T;

    //! The segmented workspace value for the void region
    int voidSpace;

    //! Specifies the boundary conditions on the sides of the domain (should be 'periodic' or 'symmetric').
    std::string sideBC;

    //! Specifies the iterative solver used in the simulation (should be 'conjugateGradient' or 'bicgstab').
    std::string solverType;

    //! Specifies the direction in of the applied voltage drop.
    char dir;

    //! The convergence criterion for the iterative solver used.
    double solverTol;

    //! The maximum number of iterations the solver may execute before exiting.
    int solverMaxIt;

    //! Specifies whether the the number of iterations and residual are printed after each iteration of the solver.
    bool print;

    //! Specifies the number of threads used for the simulation.
    int numThreads;

    //! A puma vector which stores the tortuosity in the x, y, and z directions.
    puma::Vec3<double> Tortuosity;

    //! Logs the inputs into a file.
    /*!
     * \return a boolean indicating the function executed without errors.
     */
    bool logInput();

    //! Logs the output into a file.
    /*!
     * \return a boolean indicating the function executed without errors.
     */
    bool logOutput();

    //! Checks the input for errors.
    /*!
     * \param errorMessage a pointer to string where the error message, if any, is stored.
     * \return a boolean indicating the function executed without errors.
     */
    bool errorCheck(std::string *errorMessage);

};


#endif // FV_Tortuosity_H
