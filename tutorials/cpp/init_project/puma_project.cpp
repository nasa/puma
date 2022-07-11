#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

    // primitives: Workspace
    cout << endl << "Creating a grayscale workspace:"<< endl;

    puma::Workspace grayWS(10,10,10,1e-6, false); // automatically initializes it to zeros
    // false prevents it from writing the log, unnecessary for these tutorials

    grayWS.matrix.set(0,-1, 4,4, 4,4, 255); // creating a central solid fiber

    grayWS.matrix.printRange(0,-1, 0,-1, 4,5);
    
    return 0;
}
