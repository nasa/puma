#include <iostream>
#include <cstdlib>

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
            input.straightCircle(200,200,200,5,0,200,0,90,90,90,true,0.6,1);

            generateRandomFibers(&grayWS,input);

            puma::TortuosityReturn tortReturn = compute_particle_isosurface_Tortuosity(&grayWS,puma::Cutoff(0,127),20000,20000,500,1,10000,0);
            cout << "Tortuosity: " << tortReturn.tortuosity.toString() << endl;
            cout << "Mean Intercept Length: " << tortReturn.meanInterceptLength << endl;
            cout << "Diffusion Coefficient: " << tortReturn.diffusionCoefficient.toString() << endl;
        }

        // Example case
        if(!strcmp(argv[1],"2")){

            //// YOUR CODE HERE ////
        }

    }
    return 0 ;
}
