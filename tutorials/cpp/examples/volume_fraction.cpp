#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){
    
    // volumefraction
    cout << endl << "Computing Porosity and Volume Fraction of a segmented Fiberform sample:"<< endl;

    puma::Workspace workspace(1.3, false);
    puma::import_3DTiff(&workspace, puma::path_to_example_file("200_fiberform_segmented.tif"),0);

    cout << endl << "Porosity: " << puma::compute_VolumeFraction(&workspace,0,0) << endl;
    cout << "Fiber Volume Fraction: " << puma::compute_VolumeFraction(&workspace,1,0) << endl;
    
    return 0;
}
