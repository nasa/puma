#ifndef ITERATIVESOLVER_H
#define ITERATIVESOLVER_H

#include "AMatrix.h"
#include "matrix.h"
#include "timer.h"
#include "Printer.h"

#include <cmath>


namespace IterativeSolver {

    //! solves a homogeneous (right-hand side equals zero) linear system of equations using the biconjugate gradient stabilized method.
    /*!
     * \param A a pointer to an AMatrix representing the linear system being solved.
     * \param x a pointer to a puma matrix which stores the solution. The matrix should contain an initial guess for the solution when passed in.
     * \param tol a double specifying the convergence criterion.
     * \param maxIt an integer specifying the maximum number of iterations the solver may execute before exiting.
     * \param print a boolean which specifies whether the the number of iterations and residual are printed after each iteration of the solver.
     * \return a boolean indicating whether or not convergence was achieved.
     */
    bool BiCGSTAB(AMatrix *A, puma::Matrix<double> *x, double tol, int maxIt, bool print, int numThreads);
    bool BiCGSTAB(AMatrix *A, puma::Matrix<double> *x, double tol, int maxIt, bool print, puma::Printer *printer, int numThreads);

    //! solves a linear system of equations using the biconjugate gradient stabilized method.
    /*!
     * \param A a pointer to an AMatrix representing the linear system being solved.
     * \param x a pointer to a puma matrix which stores the solution. The matrix should contain an initial guess for the solution when passed in.
     * \param b a pointer to a puma matrix which stores the right-hand side of the system of equations.
     * \param tol a double specifying the convergence criterion.
     * \param maxIt an integer specifying the maximum number of iterations the solver may execute before exiting.
     * \param print a boolean which specifies whether the the number of iterations and residual are printed after each iteration of the solver.
     * \return a boolean indicating whether or not convergence was achieved.
     */
    bool BiCGSTAB(AMatrix *A, puma::Matrix<double> *x, puma::Matrix<double> *b, double tol, int maxIt, bool print, int numThreads);
    bool BiCGSTAB(AMatrix *A, puma::Matrix<double> *x, puma::Matrix<double> *b, double tol, int maxIt, bool print, puma::Printer *printer, int numThreads);

    //! solves a homogeneous (right-hand side equals zero) linear system of equations using the conjugate gradient method.
    /*!
     * \param A a pointer to an AMatrix representing the linear system being solved.
     * \param x a pointer to a puma matrix which stores the solution. The matrix should contain an initial guess for the solution when passed in.
     * \param tol a double specifying the convergence criterion.
     * \param maxIt an integer specifying the maximum number of iterations the solver may execute before exiting.
     * \param print a boolean which specifies whether the the number of iterations and residual are printed after each iteration of the solver.
     * \return a boolean indicating whether or not convergence was achieved.
     */
    bool ConjugateGradient(AMatrix *A, puma::Matrix<double> *x, double tol, int maxIt, bool print, int numThreads);
    bool ConjugateGradient(AMatrix *A, puma::Matrix<double> *x, double tol, int maxIt, bool print, puma::Printer *printer, int numThreads);

    //! solves a linear system of equations using the conjugate gradient method.
    /*!
     * \param A a pointer to an AMatrix representing the linear system being solved.
     * \param x a pointer to a puma matrix which stores the solution. The matrix should contain an initial guess for the solution when passed in.
     * \param b a pointer to a puma matrix which stores the right-hand side of the system of equations.
     * \param tol a double specifying the convergence criterion.
     * \param maxIt an integer specifying the maximum number of iterations the solver may execute before exiting.
     * \param print a boolean which specifies whether the the number of iterations and residual are printed after each iteration of the solver.
     * \return a boolean indicating whether or not convergence was achieved.
     */
    bool ConjugateGradient(AMatrix *A, puma::Matrix<double> *x, puma::Matrix<double> *b, double tol, int maxIt, bool print, int numThreads);
    bool ConjugateGradient(AMatrix *A, puma::Matrix<double> *x, puma::Matrix<double> *b, double tol, int maxIt, bool print, puma::Printer *printer, int numThreads);
}
#endif // ITERATIVESOLVER_H
