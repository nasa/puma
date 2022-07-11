#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

    cout << endl << "Finding the mean intercept length of a FiberForm 200^3 sample:"<< endl;

// Creating empty workspace
    puma::Workspace segWS(1e-6, false);

// Importing FiberForm 200^3 tiff, selecting only subsection of it
    puma::import_3DTiff(&segWS,puma::path_to_example_file("200_fiberform_segmented.tif"));

// Computing the Mean Intercept Length
    puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(0,0));

    cout << "Mean Intercept Length: " << mil.x << ' ' << mil.y << ' ' << mil.z << endl;
    
    return 0;
}
