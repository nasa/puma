#ifndef EJ_THERMALCONDUCTIVITY_H
#define EJ_THERMALCONDUCTIVITY_H

#include "materialproperty.h"
#include "workspace.h"
#include "vector.h"
#include "ej_diffusion.h"
#include "Printer.h"

#include <iostream>
#include <map>


namespace puma {

//! computes thermal conductivity from a grayscale workspace using the explicit jump method.
/*!
 * \param grayWS a grayscale workspace containing the domain.
 * \param T a pointer to a puma matrix to store the resulting temperature field.
 * \param matCond a map containing the ID's for each material and their corresponding thermal conductivities.
 * \param dir a char specifying which direction to run the simulation in (should be 'x', 'y', or 'z').
 * \param solverTol a double specifying the convergence criterion for the iterative solver used.
 * \param solverMaxIt an integer specifying the maximum number of iterations the solver may execute before exiting.
 * \param print a boolean which specifies whether the the number of iterations and residual are printed after each iteration of the solver.
 * \param numThreads an integer which specifies the number of threads used for the simulation.
 * \return a puma vector containing the thermal conductivity in the x, y, and z directions.
 */
puma::Vec3<double> compute_EJThermalConductivity(Workspace *grayWS, puma::Matrix<double> *T, const std::map<int, double>& matCond,
                                                 char dir, double solverTol, int solverMaxIt, bool print, int numThreads = 0);
}

//! A class for computing thermal conductivity using the explicit jump method.
/*!
 *   An EJ_ThermalConductivity object is created and used by the compute_EJThermalConductivity() functions.
 *   \sa compute_EJThermalConductivity()
 */
class EJ_ThermalConductivity : MaterialProperty
{
public:

    //! A constructor which takes in the parameters used in calculating thermal conductivity from the explicit jump method.
    /*!
     * \param segWS a segmented workspace containing the domain.
     * \param T a pointer to a puma matrix to store the resulting temperature field.
     * \param matCond a map containing the ID's for each material and their corresponding thermal conductivities.
     * \param dir a char specifying which direction to run the simulation in (should be 'x', 'y', or 'z').
     * \param solverTol a double specifying the convergence criterion for the iterative solver used.
     * \param solverMaxIt an integer specifying the maximum number of iterations the solver may execute before exiting.
     * \param print a boolean which specifies whether the the number of iterations and residual are printed after each iteration of the solver.
     * \param numThreads an integer which specifies the number of threads used for the simulation.
     */
    EJ_ThermalConductivity(puma::Workspace *segWS, puma::Matrix<double> *T, std::map<int, double> matCond,
                           char dir, double solverTol, int solverMaxIt, bool print, int numThreads);


    //! computes the thermal conductivity using the parameters specified when constructing the object.
    /*!
     * \return a puma vector containing the thermal conductivity in the x, y, and z directions.
     */
    puma::Vec3<double> compute();


private:

    //! A pointer to a segmented workspace containing the domain.
    puma::Workspace *segWS;

    //! A pointer to a puma matrix to store the resulting temperature field.
    puma::Matrix<double> *T;

    //! A map containing the ID's for each material and their corresponding electrical conductivities.
    std::map<int, double> matCond;

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

    //! A puma vector which stores the thermal conductivity in the x, y, and z directions.
    puma::Vec3<double> thermalConductivity;

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


#endif // EJ_THERMALCONDUCTIVITY_H
