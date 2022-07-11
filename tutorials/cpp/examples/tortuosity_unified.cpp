#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

    // Unified tortuosity will calculate the tortuosity and characteristic length of a material, which gives
    // a description of the tortuosity valid for all Knudsen numbers.
    // See this paper for more information: https://www.sciencedirect.com/science/article/pii/S092702562100714X

        // tortuosity: compute_particle_cuberille_Tortuosity
    RandomFibersInput input;
    input.straightCircle(200,200,200,5,0,200,0,0,0,0,true,0.6,1);

    puma::Workspace grayWS;
    generateRandomFibers(&grayWS,input);

    /*
    puma::Workspace *grayWS, puma::Cutoff cutoff, int numParticles, int randomSeed, double totalLength,
    bool print, int numThreads)
    */
    puma::TortuosityUnifiedReturn tortReturn = compute_TortuosityUnified_particle(&grayWS,puma::Cutoff(0,127),20000,1,10000,true,0);

/*
        puma::Vec3<double> tortuosity;
        puma::Vec3<double> characteristicLength;
        double legacy_meanInterceptLength;  // legacy method
        puma::Vec3<double> legacy_knudsen_tortuosity;  // legacy method
        double porosity;
*/

    cout << "Tortuosity: " << tortReturn.tortuosity.toString() << endl;
    cout << "Characteristic Length: " << tortReturn.characteristicLength.toString() << endl;
    cout << "Legacy Mean Intercept Length: " << tortReturn.legacy_meanInterceptLength << endl;
    cout << "Legacy Knudsen Tortuosity: " << tortReturn.legacy_knudsen_tortuosity.toString() << endl;
    
    return 0;
}
