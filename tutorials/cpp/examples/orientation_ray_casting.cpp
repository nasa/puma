#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

// orientation: Ray Casting on Generated Random Fibers

    cout << endl << "Creating a random fiber domain and computing the Orientation via the ray casting method:"<< endl;

// Generating random fiber sample (see generateRandomFibers example for step-by-step description)
    int scale = 50;
    puma::Workspace grayWS(1e-5, false);
    puma::MatVec3<double> tangents;
    RandomFibersInput input;
    input.straightCircle(scale,scale,scale,2,0,scale,0,0,0,0,false,0.9,1);
    input.print = false; puma::generateRandomFibers(&grayWS,&tangents,input);

// Segmenting sample
    puma::Workspace segWS(grayWS.shape(), false);
    segWS.setMaterialID(&grayWS, puma::Cutoff(0,127),0);
    segWS.setMaterialID(&grayWS, puma::Cutoff(128,255),1);

// Initializing orientation matrix
    puma::MatVec3<double> dirs;

// Computing orientation using Cay Casting (RC)
    puma::compute_orientationRC(&segWS, puma::Cutoff(1, 1), 15,5, &dirs, false);

// Initializing orientation error matrix
    puma::Matrix<double> error;

// Computing orientation error between the estimated orientation (dirs) and the true orientation (tangents)
    pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

    cout << "Mean: " << MeanSD.first << endl;
    cout << "Standard Deviation: " << MeanSD.second << endl;
    
    return 0;
}
