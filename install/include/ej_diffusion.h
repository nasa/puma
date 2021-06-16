#ifndef EJ_DIFFUSION_H
#define EJ_DIFFUSION_H

#include "workspace.h"
#include "vector.h"
#include "ej_AMatrix.h"
#include "iterativesolvers.h"
#include "Printer.h"

#include <map>


//! A general class for solving diffusion problems using the explicit jump method
/*!
 * Implemented in diffusion based material property modules which use the explicit jump method.
 * \sa EJ_ThermalConductivity, EJ_ElectricalConductivity, EJ_Tortuosity
 * Makes use of EJ_AMatrix class for use with BiCGSTAB.
 */
class EJ_Diffusion
{
public:
    
    //! A constructor which takes in the parameters used for calculating the diffusion coefficient using the explicit jump method.
    /*!
     * \param T a pointer to a puma matrix to store the resulting solution field.
     * \param kMat a point to a puma matrix containing the diffusion coefficient at each point
     * \param dir a char specifying which direction to run the simulation in (should be 'x', 'y', or 'z').
     * \param solverTol a double specifying the convergence criterion for the iterative solver used.
     * \param solverMaxIt an integer specifying the maximum number of iterations the solver may execute before exiting.
     * \param print a boolean which specifies whether the the number of iterations and residual are printed after each iteration of the solver.
     * \param numThreads an integer which specifies the number of threads used for the simulation.
     */
    EJ_Diffusion(puma::Matrix<double> *T, puma::Matrix<double> *kMat, char dir, double solverTol, int solverMaxIt, bool print, puma::Printer *printer, int numThreads);
    EJ_Diffusion(puma::Matrix<double> *T, puma::Matrix<double> *kMat, char dir, double solverTol, int solverMaxIt, bool print, int numThreads);
    ~EJ_Diffusion();
    //! computes the diffusion coefficient using the parameters specified when constructing the object.
    /*!
     * \return a puma vector containing the diffusion coefficient in the x, y, and z directions.
     */
    puma::Vec3<double> compute_DiffusionCoefficient();
    
    //! computes a puma matrix which contains the diffusion coefficient at each cell in the domain.
    /*!
     * Implemented in diffusion based material property modules which use the explicit jump method.
     * \param segWS a pointer to a segmented workspace containing the domain.
     * \param matCond a map containing the ID's for each material and their corresponding diffusion coefficients.
     * \param kMat a pointer to a puma matrix in which to store the diffusion coefficients for each cell in the domain.
     * \return a boolean indicating the function executed without errors.
     * \sa EJ_ThermalConductivity, EJ_ElectricalConductivity, EJ_Tortuosity
     */
    static bool computeKMatrix(puma::Workspace *segWS, std::map<int, double> matCond, puma::Matrix<double> *kMat, int numThreads);

private:
    
    //! A puma vector containing the diffusion coefficient in the x, y, and z directions.
    puma::Vec3<double> diffusionCoefficient;
    
    //! A pointer to a puma vector in which the resulting solution field is stored.
    puma::Matrix<double> *T;

    puma::Printer *printer;
    bool delPrinter;
    
    //! A pointer to a puma matrix which stores the diffusion coefficient at each cell in the domain.
    /*!
     * \sa compute_DiffusionCoefficient()
     */
    puma::Matrix<double> *kMat;
    
    //! A puma matrix of doubles which stores the jumps.
    puma::Matrix<double> J;
    
    //! A puma matrix of doubles which stores the force (right-hand side) vector for the jumps.
    /*!
     * \sa setupF()
     */
    puma::Matrix<double> F;
    
    //! The number of cells in the x-direction.
    long X;

    //! The number of cells in the y-direction.
    long Y;

    //! The number of cells in the z-direction.
    long Z;

    //! Specifies which direction to run the simulation in (should be 'x', 'y', or 'z').
    char dir;
    
    //! The convergence criterion for the iterative solver used.
    double solverTol;
    
    //! The width of each cell.
    double H{};
    
    //! The maximum number of iterations the solver may execute before exiting.
    int solverMaxIt;
    
    //! Specifies whether the number of iterations and residual are printed after each iteration of the solver.
    bool print;
    
    //! Specifies the number of threads used for the simulation.
    int numThreads;

    //! Runs BiCGSTAB
    /*!
     * \param A a pointer to an EJ_AMatrix which represents the linear system to be solved by BiCGSTAB.
     * \return a boolean indicating the function executed without errors.
     */
    bool runIterativeSolver(EJ_AMatrix *A);
    
    //! Adds the inhomogeneous part of the solution back to obtain the true solution.
    /*!
     * \return a boolean indicating the function executed without errors.
     */
    bool addLinearProfile();
    
    //! Sets up the jump and forcing vectors as puma matrices.
    /*!
     * \param A a pointer to an EJ_AMatrix which represents the linear system.
     * \return a boolean indicating the function executed without errors.
     */
    bool setupJAndF(EJ_AMatrix *A);

};

#endif // EJ_DIFFUSION_H
