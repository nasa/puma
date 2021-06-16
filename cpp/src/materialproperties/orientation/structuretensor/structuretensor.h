#ifndef STRUCTURETENSOR_H
#define STRUCTURETENSOR_H

#include "materialproperty.h"
#include "orientation.h"
#include "workspace.h"
#include "vector.h"

#include "Eigenvalues"


namespace puma {

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
