#ifndef AMATRIX_H
#define AMATRIX_H

#include "matrix.h"


//! An abstract class representing a linear system.
class AMatrix
{
public:

    //! multiplies the linear system by a vector.
    /*!
     * \param x a pointer to a puma matrix that is multiplied by the linear system.
     * \param r a pointer to a puma matrix where the solution is stored.
     * \return
     */
    virtual bool A_times_X(puma::Matrix<double> *x, puma::Matrix<double> *r) = 0;
};

#endif // AMATRIX_H
