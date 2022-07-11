#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){
    
    // surfacearea
    cout << endl << "Computing the surface area of a sample of FiberForm:"<< endl;

    puma::Workspace grayWS(1e-6, false);
    puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

    pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&grayWS, puma::Cutoff(128, 255), true, 0);

    cout << endl << "Raw Surface Area: " << sa.first << " m^2" << endl;
    cout << "Specific Surface Area: " << sa.second << " 1/m" << endl;
    
    return 0;
}
