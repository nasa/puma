#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){
    
    // medianfilter3d
    cout << endl << "Applying a Median filter to a FiberForm sample and printing a slice of it:"<< endl;

    puma::Workspace grayWS(1e-6, false);

    puma::import_3DTiff(&grayWS, puma::path_to_example_file("200_fiberform.tif"),0);

    cout << "Before Median filter:"<< endl;
    grayWS.matrix.printRange(50,-1, 50, -1, 50,51);

    //in this case, the return should be true since all values are valid
    puma::filter_Median3D(&grayWS,2);

    cout << endl << "After Median filter:"<< endl;
    grayWS.matrix.printRange(50,-1, 50, -1, 50,51);
    
    
    // meanfilter3d
    cout << endl << "Applying a Mean filter to a FiberForm sample and printing a slice of it:"<< endl;

    puma::import_3DTiff(&grayWS, puma::path_to_example_file("200_fiberform.tif"),0);

    cout << "Before Mean filter:"<< endl;
    grayWS.matrix.printRange(50,-1, 50, -1, 50,51);

    //in this case, the return should be true since all values are valid
    puma::filter_Mean3D(&grayWS,2);

    cout << endl << "After Mean filter:"<< endl;
    grayWS.matrix.printRange(50,-1, 50, -1, 50,51);
    
    
    // bilateralfilter
    cout << endl << "Applying a Bilateral filter to a FiberForm sample and printing a slice of it:"<< endl;

    puma::import_3DTiff(&grayWS, puma::path_to_example_file("200_fiberform.tif"),0);

    cout << "Before Bilateral filter:"<< endl;
    grayWS.matrix.printRange(50,-1, 50, -1, 50,51);

    //in this case, the return should be true since all values are valid
    puma::filter_Bilateral(&grayWS,2,2,50);

    cout << endl << "After Bilateral filter:"<< endl;
    grayWS.matrix.printRange(50,-1, 50, -1, 50,51);
    
    
    return 0;
}
