#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

    // tortuosity: compute_particle_isosurface_Tortuosity
    RandomFibersInput input;
    input.straightCircle(200,200,200,5,0,200,0,0,0,0,true,0.6,1);

    puma::Workspace grayWS;
    generateRandomFibers(&grayWS,input);

    /*
    puma::Workspace *grayWS, puma::Cutoff cutoff,
    int numParticles, double meanFreePath, double meanVelocity,
    int randomSeed, double totalLength, int numThreads
    */
    puma::TortuosityReturn tortReturn = compute_particle_isosurface_Tortuosity(&grayWS,puma::Cutoff(0,127),20000,1,500,1,10000,0);

    cout << "Tortuosity: " << tortReturn.tortuosity.toString() << endl;
    cout << "Mean Intercept Length: " << tortReturn.meanInterceptLength << endl;
    cout << "Diffusion Coefficient: " << tortReturn.diffusionCoefficient.toString() << endl;
    
    return 0;
}
