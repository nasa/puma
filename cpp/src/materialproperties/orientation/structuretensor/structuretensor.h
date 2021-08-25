#ifndef STRUCTURETENSOR_H
#define STRUCTURETENSOR_H

#include "materialproperty.h"
#include "orientation.h"
#include "workspace.h"
#include "vector.h"

#include "Eigenvalues"


namespace puma {

/*! Function to compute the orientation using the Structure Tensor (i.e. find the direction of least gradient change)
\param grayWS the input workspace to serve as a mask according to the cutoff (Note: the ST filter is applied on the whole image, the cutoffs are only applied when computing the Eigenvectors to put in the direction MatVec3)
\param dogsigmaArr kernel size parameter for Gaussian derivatives (should be smaller than rho)
\param gausigmaArr kernel size parameter for Gaussian filter (should be bigger than sigma)
\param cutoff the grayscale range to consider, according to the input ws
\param direction the output orientation field
\param print whether to print progress, optional
\param numThreads number of threads to use, optional
\return true if successful, false if it failing
*/
bool compute_orientationST(Workspace *grayWS, double dogsigmaArr, double gausigmaArr, puma::Cutoff cutoff, puma::MatVec3<double> *direction, bool print = true, int numThreads = 0);

}


class StructureTensor : public MaterialProperty
{
public:
    StructureTensor(puma::Workspace *WS, double dogSigma, double gausRho, puma::Cutoff cutoff, puma::MatVec3<double> *direction, bool print, int numThreads);
    bool computeGradient();

private:
    puma::Workspace *WS;
    double dogSigma, gausRho;
    puma::Cutoff cutoff;
    int numThreads;
    bool print;
    int X,Y,Z;
    puma::MatVec3<double> *direction;

    void kerngen(puma::Matrix<double> *filter, int derivative); // derivative: 1 for Gaussian, 2 for derivative
    void convn(puma::Matrix<double> *img, puma::Matrix<double> *filter, puma::Matrix<double> *img_out, char bc);

    static long indexAt(int index, int size);

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};

#endif // STRUCTURETENSOR_H
