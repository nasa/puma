#ifndef EJ_AMATRIX_H
#define EJ_AMATRIX_H

#include "vector.h"
#include "AMatrix.h"
#include "fftw3.h"


//! A class which represents the linear system which must be solved to obtain the jumps in the explicit jump method.
class EJ_AMatrix : public AMatrix
{
public:
    //! A constructor which takes in the parameters used in solving for the jumps.
    /*!
     * \param kMat a pointer to a puma matrix containing the conductivity at each cell.
     * \param dir a char indicating the direction of the simulation.
     * \param numThreads an integer indicating the number of threads used.
     */
    EJ_AMatrix(puma::Matrix<double> *kMat, char dir, int numThreads);

    //! multiplies the linear system by a vector.
    /*!
     * \param x a pointer to a puma matrix that is multiplied by the linear system.
     * \param r a pointer to a puma matrix where the solution is stored.
     * \return a boolean which is true if no errors are encountered.
     */
    bool A_times_X(puma::Matrix<double> *x, puma::Matrix<double> *r);

    //! computes the effective conductivity from the solution.
    /*!
     * \param T a pointer to a puma matrix containing the solution.
     * \param J a pointer to a puma matrix containing the jumps.
     * \return a puma vector containing the thermal conductivity in the x, y, and z directions.
     */
    puma::Vec3<double> computeConductivity(puma::Matrix<double> *T, puma::Matrix<double> *J);

    //! preallocates and sets up matrices for explicit jump method.
    /*!
     * \return a boolean which is true if no errors are encountered.
     */
    bool setup();

    bool free();

    //! finds the total number of interfaces at which jumps occur.
    /*!
     * \return a long containing the total number of interfaces.
     */
    long getNumInterfaces();

    //! returns F in a puma matrix.
    /*!
     * \param FOut a pointer to a puma matrix into which the values of F will be placed.
     */
    void getF(puma::Matrix<double> *FOut);

    //! computes the homogenized solution and stores it in a puma matrix.
    /*!
     * \param T a pointer to a puma matrix which stores the solution.
     * \param J a pointer to a puma matrix containing the jumps.
     */
    void computeHomogenizedSolution(puma::Matrix<double> *T, puma::Matrix<double> *J);

private:
    //! A pointer to a puma matrix containing the conductivity at every cell.
    puma::Matrix<double> *kMat;

    double *inData;
    double *outData;
    fftw_complex *fourierData;
    fftw_plan pf, pb;

    //! A char indicating the direction in which the simulation is run.
    char dir;

    //! A double indicating the width of a cell.
    double H;

    //! A long indicating the total number of cells.
    long N;

    //! An integer indicating the number of threads used.
    int numThreads;

    //! A long indicating the number of interfaces in the x-direction.
    long numXInterfaces;

    //! A long indicating the number of interfaces in the y-direction.
    long numYInterfaces;

    //! A long indicating the number of interfaces in the z-direction.
    long numZInterfaces;

    //! A long indicating the total number of interfaces.
    long numInterfacesTotal;

    //! A vector of longs containing indices indicating the cell behind each of the interfaces facing in the x-direction.
    std::vector<long> indexXInterfacesBack;

    //! A vector of longs containing indices indicating the cell in front of each of the interfaces facing in the x-direction.
    std::vector<long> indexXInterfacesFront;

    //! A vector of longs containing indices indicating the cell behind each of the interfaces facing in the y-direction.
    std::vector<long> indexYInterfacesBack;

    //! A vector of longs containing indices indicating the cell in front of each of the interfaces facing in the y-direction.
    std::vector<long> indexYInterfacesFront;

    //! A vector of longs containing indices indicating the cell behind each of the interfaces facing in the z-direction.
    std::vector<long> indexZInterfacesBack;

    //! A vector of longs containing indices indicating the cell in front of each of the interfaces facing in the z-direction.
    std::vector<long> indexZInterfacesFront;

    //! A vector of longs containing IDs for each of the jumps in the x-direction.
    std::vector<long> jumpCountsX;

    //! A vector of longs containing IDs for each of the jumps in the y-direction.
    std::vector<long> jumpCountsY;

    //! A vector of longs containing IDs for each of the jumps in the z-direction.
    std::vector<long> jumpCountsZ;

    //! A vector of doubles containing coefficients associated with the x-interfaces used in creating the D matrix
    std::vector<double> BI;

    //! A vector of doubles containing coefficients associated with the y-interfaces used in creating the D matrix
    std::vector<double> BJ;

    //! A vector of doubles containing coefficients associated with the z-interfaces used in creating the D matrix
    std::vector<double> BK;

    //! A vector of doubles containing the elements of BI multiplied by negative one.
    std::vector<double> BINegative;

    //! A vector of doubles containing the elements of BJ multiplied by negative one.
    std::vector<double> BJNegative;

    //! A vector of doubles containing the elements of BI multiplied by negative one.
    std::vector<double> BKNegative;

    //! A vector of doubles representing the forcing vector (right-hand side) for the jump equations.
    std::vector<double> F;

    //! A vector of doubles representing a Poisson equation in Fourier space.
    std::vector<double> PDM;

    //! A vector of doubles containing indices used to iterate through columns of the Psi matrix or rows of the D matrix.
    std::vector<long> PsiColDRowInds;

    //! A vector of doubles containing indices used to iterate through rows of the Psi matrix.
    std::vector<long> PsiRowInds;

    //! A vector of doubles containing indices used to iterate through columns of the D matrix.
    std::vector<long> DColInds;

    //! A vector containing coefficients used with the D matrix.
    std::vector<double> BForDMatrix;

    //! An N length vector of doubles which stores intermediate solutions when computing the residual.
    puma::Matrix<double> rN;

    //! A size for the z-direction used when in Fourier space by FFTW.
    int ZFourierSpace;

    //! A function which finds the value of a matrix at a certain position and accounts for periodic boundaries.
    /*!
     * \param i an integer representing the position index in the x-direction.
     * \param j an integer representing the position index in the y-direction.
     * \param k an integer representing the position index in the z-direction.
     * \param x a pointer to a puma matrix which must be the same size as the domain.
     * \return a double representing a value of x at a position given by i, j, and k.
     */
    double getValueAt(int i, int j, int k, puma::Matrix<double> *x);

    //! The number of cells in the x-direction.
    int X;

    //! The number of cells in the y-direction.
    int Y;

    //! The number of cells in the z-direction.
    int Z;

    //! Represents pi to machine accuracy.
    double pi;

    //! Finds the cells preceding and following each interface and stores their indices in vectors.
    /*!
     * \sa IndexXInterfacesBack, IndexXInterfacesFront, IndexYInterfacesBack, IndexYInterfacesFront, IndexZInterfacesBack, IndexZInterfacesFront
     */
    void findInterfaceFrontsAndBacks();

    //! Finds the number of interfaces in each direction and in total and stores them in variables.
    /*!
     * \sa numXInterfaces, numYInterfaces, numZInterfaces, numInterfacesTotal
     */
    void findNumInterfaces();

    //! Sets up the jump count vectors.
    /*!
     * \sa jumpCountsX, jumpCountsY, jumpCountsZ
     */
    void setupJumpCounts();

    //! Sets up the B vectors.
    /*!
     * \sa BI, BJ, BK, BINegative, BJNegative, BKNegative
     */
    void setupBMatrices();

    //! Sets up the forcing vector for the jumps.
    /*!
     * \sa F
     */
    void setupF();

    //! Sets up the PDM vector.
    /*!
     * \sa PDM
     */
    void setupPDM();

    //! Sets up the vectors used to index the Psi and D matrices.
    /*!
     * \sa PsiColDRowInds, PsiRowInds, DColInds, BForDMatrix
     */
    void setupSparseMatrixIndexing();

    //! Preallocates vectors for performance.
    void preallocateMatrices();

    //! Multiplies the Psi matrix by a puma matrix treated as a vector in linear algebra.
    /*!
     * \param x a pointer to a puma matrix which should be 1D and have a size equal to the number of interfaces.
     * \param r a pointer to a puma matrix where the solution is stored. This should have the same dimensions as the discretized domain.
     */
    void Psi_times_X(puma::Matrix<double> *x, puma::Matrix<double> *r);

    //! Multiplies the pseudo-inverse of the discrete Laplacian matrix by a puma matrix using FFTs.
    /*!
     * \param x a pointer to a puma matrix which should have the same dimensions as the discretized domain. This puma matrix is overwritten by the solution.
     */
    void AInv_times_X(puma::Matrix<double> *x);

    //! Multiplies the D matrix by a puma matrix treated as a vector in linear algebra.
    /*!
     * \param x a pointer to a puma matrix which should have the same dimensions as the discretized domain.
     * \param r a pointer ta a puma matrix where the solution is stored. This should be 1D and have a size equal to the number of interfaces.
     */
    void D_times_X(puma::Matrix<double> *x, puma::Matrix<double> *r);

    //! Subtracts the elements of r from the elements of x and stores the result in r.
    /*!
     * x and r must have a size equal to the total number of jumps.
     * \param x a pointer to a puma matrix.
     * \param r a pointer to a puma matrix.
     */
    void X_minus_R(puma::Matrix<double> *x, puma::Matrix<double> *r);


};

#endif // EJ_AMATRIX_H
