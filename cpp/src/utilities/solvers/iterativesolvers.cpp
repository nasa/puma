#include "iterativesolvers.h"

#include <sstream>


/*
 * Description: BiConjugate Gradient Stabilized Solver for problems of type: Ax = 0
 * Inputs: A - matrix class
 *         x - unknowns
 *         tol tolerance
 *         maxIt - maximum iterations
 *         print - print of the iteration number, time and residual
 *         numThreads - number of threads to split the for loop
 * Outputs: x - Converged solution
 */
bool IterativeSolver::BiCGSTAB(AMatrix *A, puma::Matrix<double> *x, double tol, int maxIt, bool print, int numThreads){
    puma::Printer printer;
    return BiCGSTAB( A, x, tol, maxIt, print, &printer, numThreads);
}


bool IterativeSolver::BiCGSTAB(AMatrix *A, puma::Matrix<double> *x, double tol, int maxIt, bool print, puma::Printer *printer, int numThreads) {
    puma::Timer t1;
    t1.reset();
    puma::Matrix<double> v(x->X(),x->Y(),x->Z(),0);
    puma::Matrix<double> p(x->X(),x->Y(),x->Z(),0);
    puma::Matrix<double> s(x->X(),x->Y(),x->Z(),0);
    puma::Matrix<double> t(x->X(),x->Y(),x->Z(),0);
    puma::Matrix<double> r(x->X(),x->Y(),x->Z(),0);

    A->A_times_X(x,&r);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (long i=0;i<r.size();i++){
        r(i)*=-1;
    }

    if(sqrt(r.dot(&r)) < tol ) {
        return true;
    }

    puma::Matrix<double> r_hat(&r);
    double rho = 1;
    double alpha = 1;
    double omega = 1;
    double rho_old = rho;
    double beta = 0;
    double tau = 0;

    if(print) {
        printer->print("BiCGSTAB Solver running");
    }

    for(int it=0;it<maxIt;it++){
        rho = r.dot(&r_hat);
        if (rho == 0.) {
            // BiCGSTAB Breakdown
            printer->print("BiCGSTAB Warning:  rho = 0");
            return false;
        }
        beta = (rho/rho_old)*(alpha/omega);

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0;i<r.size();i++){
            p(i)=r(i)+beta*(p(i)-omega*v(i));
        }

        A->A_times_X(&p,&v);
        tau = v.dot(&r_hat);
        if (tau == 0.) {
            // BiCGSTAB Breakdown
            printer->print("BiCGSTAB Warning:  tau = 0");
            return false;
        }
        alpha = rho/tau;

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0;i<r.size();i++){
            s(i)=r(i)-alpha*v(i);
        }
        A->A_times_X(&s,&t);

        tau = t.dot(&t);
        if (s.dot(&s) == 0.) {
            omega = 0;
        }
        else if (tau == 0.) {
            // BiCGSTAB Breakdown
            printer->print("BiCGSTAB Warning:  tau = 0");
            return false;
        }
        else {
            omega = t.dot(&s)/tau;
        }

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0;i<r.size();i++){
            // Update Solution and Residual
            r(i) = s(i)-omega*t(i);
            (*x)(i) += alpha*p(i)+omega*s(i);
        }

        double zeta = sqrt(r.dot(&r));
        if(print) {
            std::stringstream buffer;
            buffer << '\r' << "Iteration: " << it+1 << "  -  " << "Time: " << t1.elapsed() << "  -  " << "Residual: " << zeta;
            printer->print(buffer.str());
        }

        if(zeta < tol){
            return true;
        }
        rho_old = rho;
        if (omega == 0.)
        {
            // BiCGSTAB Breakdown
            printer->print("BiCGSTAB Warning:  omega = 0");
            return false;
        }

    }
    printer->print( "BiCGSTAB Warning: Max Iterations Reached");
    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool IterativeSolver::BiCGSTAB(AMatrix *A, puma::Matrix<double> *x, puma::Matrix<double> *b, double tol, int maxIt, bool print, int numThreads){
    puma::Printer printer;
    return BiCGSTAB(A, x, b, tol, maxIt, print, &printer, numThreads);
}

/*
 * Description: BiConjugate Gradient Stabilized Solver for problems of type: Ax = b
 * Inputs: A - matrix class
 *         x - unknowns
 *         b - vector
 *         tol tolerance
 *         maxIt - maximum iterations
 *         print - print of the iteration number, time and residual
 *         numThreads - number of threads to split the for loop
 * Outputs: x - Converged solution
 */
bool IterativeSolver::BiCGSTAB(AMatrix *A, puma::Matrix<double> *x, puma::Matrix<double> *b, double tol, int maxIt, bool print, puma::Printer *printer, int numThreads) {
    puma::Timer t1;
    t1.reset();

    puma::Matrix<double> v(x->X(),x->Y(),x->Z(),0);
    puma::Matrix<double> p(x->X(),x->Y(),x->Z(),0);
    puma::Matrix<double> s(x->X(),x->Y(),x->Z(),0);
    puma::Matrix<double> t(x->X(),x->Y(),x->Z(),0);
    puma::Matrix<double> r(x->X(),x->Y(),x->Z(),0);

    A->A_times_X(x,&r);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (long i=0;i<r.size();i++){
        r(i)=(*b)(i)-r(i);
    }

    if(sqrt(r.dot(&r)) < tol ) {
        return true;
    }

    puma::Matrix<double> r_hat(&r);
    double rho = 1;
    double alpha = 1;
    double omega = 1;
    double rho_old = rho;
    double beta = 0;
    double tau = 0;

    if(print) {
        printer->print("BiCGSTAB Solver running");
    }

    for(int it=0;it<maxIt;it++){
        rho = r.dot(&r_hat);
        if (rho == 0.) {
            // BiCGSTAB Breakdown
            printer->print("BiCGSTAB Warning:  rho = 0");
            return false;
        }
        beta = (rho/rho_old)*(alpha/omega);

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0;i<r.size();i++){
            p(i)=r(i)+beta*(p(i)-omega*v(i));
        }

        A->A_times_X(&p,&v);
        tau = v.dot(&r_hat);
        if (tau == 0.) {
            // BiCGSTAB Breakdown
            printer->print("BiCGSTAB Warning:  tau = 0");
            return false;
        }
        alpha = rho/tau;

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0;i<r.size();i++){
            s(i)=r(i)-alpha*v(i);
        }
        A->A_times_X(&s,&t);

        tau = t.dot(&t);
        if (s.dot(&s) == 0.) {
            printer->print("BiCGSTAB Warning:  omega = 0");
            omega = 0;
        }
        else if (tau == 0.) {
            // BiCGSTAB Breakdown
            printer->print("BiCGSTAB Warning:  tau = 0");
            return false;
        }
        else {
            omega = t.dot(&s)/tau;
        }

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0;i<r.size();i++){
            // Update Solution and Residual
            r(i) = s(i)-omega*t(i);
            (*x)(i) += alpha*p(i)+omega*s(i);
        }

        double zeta = sqrt(r.dot(&r));
        if(print) {
            std::stringstream buffer;
            buffer << '\r' << "Iteration: " << it+1 << "  -  " << "Time: " << t1.elapsed() << "  -  " << "Residual: " << zeta;
            printer->print(buffer.str());
        }

        if(zeta < tol){
            return true;
        }
        rho_old = rho;
        if (omega == 0.)
        {
            // BiCGSTAB Breakdown
            printer->print("BiCGSTAB Warning:  omega = 0");
            return false;
        }

    }
    printer->print("BiCGSTAB Warning: Max Iterations Reached");
    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool IterativeSolver::ConjugateGradient(AMatrix *A, puma::Matrix<double> *x, double tol, int maxIt, bool print, int numThreads) {
    puma::Printer printer;
    return ConjugateGradient(A, x, tol, maxIt, print, &printer, numThreads);
}

/*
 * Description: Conjugate Gradient Solver for problems of type: Ax = 0
 * Inputs: A - class
 *         x - unknowns
 *         tol tolerance
 *         maxIt - maximum iterations
 *         print - print of the iteration number, time and residual
 *         numThreads - number of threads to split the for loop
 * Outputs: x - Converged solution
 */
bool IterativeSolver::ConjugateGradient(AMatrix *A, puma::Matrix<double> *x, double tol, int maxIt, bool print, puma::Printer *printer, int numThreads) {
    puma::Timer t1;
    t1.reset();

    puma::Matrix<double> r(x->X(),x->Y(),x->Z(),0);
    puma::Matrix<double> Ap(x->X(),x->Y(),x->Z(),0);
    double psold,alpha,rsnew;

    A->A_times_X(x,&r);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (long i=0;i<r.size();i++){
        r(i)*=-1;
    }
    puma::Matrix<double> p(&r);

    if(sqrt(r.dot(&r)) < tol ) {
        return true;
    }
    if(print) printer->print("Conjugate Gradient Solver running");

    // Start of Iterations
    double rsold = r.dot(&r);
    for(int it=0;it<maxIt;it++){

        A->A_times_X(&p,&Ap);
        psold = p.dot(&Ap);
        alpha = rsold/psold;
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0;i<r.size();i++){
            (*x)(i) += alpha*p(i);
            r(i) += -alpha*Ap(i);
        }
        rsnew = r.dot(&r);

        if(sqrt(rsnew)<tol) {
            return true;
        }
        if(print) {
            std::stringstream buffer;
            buffer << '\r' << "Iteration: " << it+1 << "  -  " << "Time: " << t1.elapsed() << "  -  " << "Residual: " << sqrt(rsnew);
            printer->print(buffer.str());
        }


        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0;i<p.size();i++){
            p(i)=r(i)+rsnew/rsold*p(i);
        }
        rsold=rsnew;
    }
    printer->print("Conjugate Gradient Warning: Max Iterations Reached");

    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * Description: Conjugate Gradient Solver for problems of type: Ax = b
 * Inputs: A - matrix class
 *         x - unknowns
 *         b - vector
 *         tol tolerance
 *         maxIt - maximum iterations
 *         print - print of the iteration number, time and residual
 *         numThreads - number of threads to split the for loop
 * Outputs: x - Converged solution
 */
bool IterativeSolver::ConjugateGradient(AMatrix *A, puma::Matrix<double> *x, puma::Matrix<double> *b, double tol, int maxIt, bool print, int numThreads) {
    puma::Printer printer;
    return ConjugateGradient(A, x, b, tol, maxIt, print, &printer, numThreads);

}


bool IterativeSolver::ConjugateGradient(AMatrix *A, puma::Matrix<double> *x, puma::Matrix<double> *b, double tol, int maxIt, bool print , puma::Printer *printer, int numThreads) {
    puma::Timer t1;
    t1.reset();

    puma::Matrix<double> r(x->X(),x->Y(),x->Z(),0);
    puma::Matrix<double> Ap(x->X(),x->Y(),x->Z(),0);
    double psold,alpha,rsnew;

    A->A_times_X(x,&r);

    omp_set_num_threads(numThreads);
#pragma omp parallel for
    for (long i=0;i<r.size();i++){
        r(i)=(*b)(i)-r(i);
    }
    puma::Matrix<double> p(&r);

    if(sqrt(r.dot(&r)) < tol ) {
        return true;
    }
    if(print) {
        printer->print("Conjugate Gradient Solver running");
    }

    // Start of Iterations
    double rsold = r.dot(&r);
    for(int it=0;it<maxIt;it++){

        A->A_times_X(&p,&Ap);
        psold = p.dot(&Ap);
        alpha = rsold/psold;
        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0;i<r.size();i++){
            (*x)(i) += alpha*p(i);
            r(i) += -alpha*Ap(i);
        }
        rsnew = r.dot(&r);

        if(sqrt(rsnew)<tol) {
            return true;
        }
        if(print) {
            std::stringstream buffer;
            buffer << '\r' << "Iteration: " << it+1 << "  -  " << "Time: " << t1.elapsed() << "  -  " << "Residual: " << sqrt(rsnew);
            printer->print(buffer.str());
        }

        omp_set_num_threads(numThreads);
#pragma omp parallel for
        for(long i=0;i<p.size();i++){
            p(i)=r(i)+rsnew/rsold*p(i);
        }
        rsold=rsnew;
    }
    printer->print("Conjugate Gradient Warning: Max Iterations Reached");

    return false;
}
