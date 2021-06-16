#ifndef MATERIALPROPERTYMIL_H
#define MATERIALPROPERTYMIL_H

#include "materialproperty.h"
#include "workspace.h"
#include "vector.h"


namespace puma {

    //! computes the mean intercept length on a grayscale workspace.
    /*!
     * \param segWS a segmented workspace containing the domain.
     * \param voidLowpuma::Cutoff an integer corresponding to the lowest grayscale value which is considered void.
     * \param voidHighpuma::Cutoff an integer corresponding to the highest grayscale value which is considered void.
     * \return a puma vector containing the mean intercept length in the x, y, and z directions.
     */
    puma::Vec3<double> compute_MeanInterceptLength(puma::Workspace *grayWS, puma::Cutoff cutoff, int numThreads = 0);
}

//! A class for computing mean intercept length
/*!
 *   A MeanInterceptLength object is created and used by the compute_MeanInterceptLength() functions.
 *   \sa compute_MeanInterceptLength()
 */
class MeanInterceptLength : MaterialProperty
{
public:

    //! A constructor which takes in the parameters used in calculating the mean intercept length.
    /*!
     * \param segWS a segmented workspace containing the domain.
     * \param voidValue an integer corresponding to the material ID for the void.
     */
    MeanInterceptLength(puma::Workspace *segWS, puma::Cutoff cutoff, int numThreads);

    //! computes the mean intercept length using the parameters specified when constructing the object.
    /*!
     * \return a puma vector containing the mean intercept length in the x, y, and z directions.
     */
    puma::Vec3<double> compute();


private:

    //! A puma vector which stores the mean intercept length in the x, y, and z directions.
    puma::Vec3<double> meanInterceptLength;

    //! A pointer to a segmented workspace containing the domain.
    puma::Workspace *segWS;

    //! Corresponds to the cutoff values, low and high
    puma::Cutoff cutoff;

    //! The number of threads to use for the computation.
    int numThreads;

    //! A helper function which calculates the mean intercept length and stores it in a puma vector.
    /*!
     * \sa meanInterceptLength
     */
    void meanInterceptLengthHelper();

    //! Gets the number of transitions from void to solid (i.e. collisions) in the x direction.
    /*!
     * This only counts interfaces which transition from void to solid when moving in the
     * positive x direction and not those which transition from void to solid.
     * \return number of collisions in the x direction.
     */
    long getNumCollisionsX();

    //! Gets the number of transitions from void to solid (i.e. collisions) in the y direction.
    /*!
     * This only counts interfaces which transition from void to solid when moving in the
     * positive y direction and not those which transition from void to solid.
     * \return number of collisions in the y direction.
     */
    long getNumCollisionsY();

    //! Gets the number of transitions from void to solid (i.e. collisions) in the z direction.
    /*!
     * This only counts interfaces which transition from void to solid when moving in the
     * positive z direction and not those which transition from void to solid.
     * \return number of collisions in the z direction.
     */
    long getNumCollisionsZ();

    //! Gets the number of void cells.
    /*!
     * \return number of void cells.
     */
    long getNumVoidCells();


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


#endif // MATERIALPROPERTYMIL_H
