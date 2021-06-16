#ifndef FV_ElectricalCONDUCTIVITY_H
#define FV_ElectricalCONDUCTIVITY_H

#include "materialproperty.h"
#include "workspace.h"
#include "vector.h"
#include "fv_diffusion.h"
#include "Printer.h"

#include <map>


namespace puma {

//! computes Electrical conductivity from a grayscale workspace using the finite volume method.
/*!
 * \param grayWS a grayscale workspace containing the domain.
 * \param T a pointer to a puma matrix to store the resulting temperature field.
 * \param matCond a map containing the ID's for each material and their corresponding Electrical conductivities.
 * \param sideBC a string specifying the boundary conditions on the sides of the domain (should be 'periodic' or 'symmetric').
 * \param solverType a string specifying the iterative solver used in the simulation (should be 'conjugateGradient' or 'bicgstab').
 * \param dir a char specifying which direction to run the simulation in (should be 'x', 'y', or 'z').
 * \param solverTol a double specifying the convergence criterion for the iterative solver used.
 * \param solverMaxIt an integer specifying the maximum number of iterations the solver may execute before exiting.
 * \param print a boolean which specifies whether the the number of iterations and residual are printed after each iteration of the solver.
 * \param numThreads an integer which specifies the number of threads used for the simulation.
 * \return a puma vector containing the Electrical conductivity in the x, y, and z directions.
 */
    puma::Vec3<double> compute_FVElectricalConductivity(Workspace *grayWS, puma::Matrix<double> *T, const std::map<int, double>& matCond,
                                                     std::string sideBC, std::string solverType, char dir, double solverTol,
                                                     int solverMaxIt, bool print, int numThreads = 0);

}

//! A class for computing Electrical conductivity using the finite volume method.
/*!
 *   A FV_ElectricalConductivity object is created and used by the compute_FVElectricalConductivity() functions.
 *   \sa compute_FVElectricalConductivity()
 */
class FV_ElectricalConductivity : MaterialProperty
{
public:

    //! A constructor which takes in the parameters used in calculating Electrical conductivity from the finite volume method.
    /*!
     * \param segWS a segmented workspace containing the domain.
     * \param T a pointer to a puma matrix to store the resulting temperature field.
     * \param matCond a map containing the ID's for each material and their corresponding Electrical conductivities.
     * \param sideBC a string specifying the boundary conditions on the sides of the domain (should be 'periodic' or 'symmetric').
     * \param solverType a string specifying the iterative solver used in the simulation (should be 'conjugateGradient' or 'bicgstab').
     * \param dir a char specifying which direction to run the simulation in (should be 'x', 'y', or 'z').
     * \param solverTol a double specifying the convergence criterion for the iterative solver used.
     * \param solverMaxIt an integer specifying the maximum number of iterations the solver may execute before exiting.
     * \param print a boolean which specifies whether the the number of iterations and residual are printed after each iteration of the solver.
     * \param numThreads an integer which specifies the number of threads used for the simulation.
     */
    FV_ElectricalConductivity(puma::Workspace *segWS, puma::Matrix<double> *T, std::map<int, double> matCond,
                           std::string sideBC, std::string solverType, char dir, double solverTol,
                           int solverMaxIt, bool print,  int numThreads);

    //! computes the Electrical conductivity using the parameters specified when constructing the object.
    /*!
     * \return a puma vector containing the Electrical conductivity in the x, y, and z directions.
     */
    puma::Vec3<double> compute();


private:

    //! A pointer to a segmented workspace containing the domain.
    puma::Workspace *segWS;

    //! A pointer to a puma matrix to store the resulting temperature field.
    puma::Matrix<double> *T;

    //! A map containing the ID's for each material and their corresponding electrical conductivities.
    std::map<int, double> matCond;

    //! Specifies the boundary conditions on the sides of the domain (should be 'periodic' or 'symmetric').
    std::string sideBC;

    //! Specifies the iterative solver used in the simulation (should be 'conjugateGradient' or 'bicgstab').
    std::string solverType;

    //! Specifies the direction in of the applied temperature drop.
    char dir;

    //! The convergence criterion for the iterative solver used.
    double solverTol;

    //! The maximum number of iterations the solver may execute before exiting.
    int solverMaxIt;

    //! Specifies whether the the number of iterations and residual are printed after each iteration of the solver.
    bool print;

    //! Specifies the number of threads used for the simulation.
    int numThreads;

    //! A puma vector which stores the Electrical conductivity in the x, y, and z directions.
    puma::Vec3<double> ElectricalConductivity;

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

    puma::Printer *printer;
    bool delPrinter;

};


#endif // FV_ElectricalCONDUCTIVITY_H
