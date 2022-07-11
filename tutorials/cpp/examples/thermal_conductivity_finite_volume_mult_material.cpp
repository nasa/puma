#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

    // fvthermalconductivity: Multiple materials in series
    cout << endl << "Computing the thermal conductivity using finite volume considering a locally isotropic sample having several materials in series:"<< endl;

// Initializing Workspace
    puma::Workspace segWS(100,57,62,0,1e-6, false);

// Setting the workspace values of the materials in series
    segWS.matrix.set(10,19,0,56,0,61,1);
    segWS.matrix.set(20,29,0,56,0,61,2);
    segWS.matrix.set(30,39,0,56,0,61,3);
    segWS.matrix.set(40,49,0,56,0,61,4);
    segWS.matrix.set(50,59,0,56,0,61,5);
    segWS.matrix.set(60,69,0,56,0,61,6);
    segWS.matrix.set(70,79,0,56,0,61,7);
    segWS.matrix.set(80,89,0,56,0,61,8);
    segWS.matrix.set(90,99,0,56,0,61,9);

// Initializing Temperature field
    puma::Matrix<double> T;

// Initializing material conductivity
    map<int, double> matCond;
    matCond[0] = 1;
    matCond[1] = 2;
    matCond[2] = 3;
    matCond[3] = 4;
    matCond[4] = 5;
    matCond[5] = 6;
    matCond[6] = 7;
    matCond[7] = 8;
    matCond[8] = 9;
    matCond[9] = 10;

// Running simulation
    puma::Vec3<double> k = puma::compute_FVThermalConductivity(&segWS, &T, matCond,"periodic","bicgstab",'x',1e-3,10000,true);

    cout << endl << "Conductivity: " << endl;
    cout << "kxx " << k.x << " kxy " << k.y << " kxz " << k.z << endl;
    
    return 0;
}
