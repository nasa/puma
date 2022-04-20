#include <iostream>

#include "puma.h"

using namespace std;


//_____________________________________________________________________________
// main function
int main (int argc, char **argv)
//-----------------------------------------------------------------------------
{

    // To run input 1 as command line argument
    if(argc > 1){

        // Example case
        if(!strcmp(argv[1],"1")){

            puma::Workspace grayWS(100,100,100,1e-6);

            RandomFibersInput input;
            input.straightCircle(200,200,200,5,0,200,0,0,-1,-1,true,0.6,1);

            generateRandomFibers(&grayWS,input);

            double vf = puma::compute_VolumeFraction(&grayWS, puma::Cutoff(128,255));
            std::cout << "Volume Fraction: " << vf << std::endl;

        }

    }
    return 0;
}
