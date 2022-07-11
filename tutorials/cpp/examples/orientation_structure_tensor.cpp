#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

// orientation: Structure Tensor on Generated Random Fibers

    cout << endl << "Creating a random fiber domain and computing the Orientation via the structure tensor method:"<< endl;

// Generating random fiber sample (see generateRandomFibers example for step-by-step description)
    int scale = 50;
    puma::Workspace grayWS(1e-5, false);
    puma::MatVec3<double> tangents;
    RandomFibersInput input;
    input.straightCircle(scale,scale,scale,2,0,scale,0,0,0,0,false,0.9,1);
    input.print = false; puma::generateRandomFibers(&grayWS,&tangents,input);

    puma::Workspace segWS(grayWS.shape(), false);
    segWS.setMaterialID(&grayWS, puma::Cutoff(0,127),0);
    segWS.setMaterialID(&grayWS, puma::Cutoff(128,255),1);

    puma::MatVec3<double> dirs;

// Computing orientation using Structure Tensor (ST)
    puma::compute_orientationST(&segWS, 0.7, 1.1, puma::Cutoff(1, 1), &dirs,false);

    puma::Matrix<double> error;
    pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

    cout << "Mean: " << MeanSD.first << endl;
    cout << "Standard Deviation: " << MeanSD.second << endl;

    return 0;
}
