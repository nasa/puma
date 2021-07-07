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

            puma::Workspace grayWS(1e-6, false);
            puma::import_3DTiff(&grayWS,"/Users/fsemerar/Documents/PuMA_playground/PuMA_data/part2_mpfa/validation/ff/ff800.tif",0);
            puma::export_STL(&grayWS, puma::Cutoff(90,255), false,"/Users/fsemerar/Downloads/ff800.stl");
        }

    }
    return 0;
}
