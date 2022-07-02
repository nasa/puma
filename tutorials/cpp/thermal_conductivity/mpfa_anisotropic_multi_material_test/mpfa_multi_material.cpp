#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

    // fvanisotropicthermalconductivity: Multiple materials in series
    cout << endl << "Computing the thermal conductivity using finite volume considering a locally anisotropic sample having several materials in series:"<< endl;

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

    // Initializing Temperature and Flux field
    puma::Matrix<double> T;
    puma::MatVec3<double> q;

    // Initializing material conductivity
    map<int, vector<double>> matCond;
    matCond[0] = {1,1,1,0,0,0};
    matCond[1] = {2,2,2,0,0,0};
    matCond[2] = {3,3,3,0,0,0};
    matCond[3] = {4,4,4,0,0,0};
    matCond[4] = {5,5,5,0,0,0};
    matCond[5] = {6,6,6,0,0,0};
    matCond[6] = {7,7,7,0,0,0};
    matCond[7] = {8,8,8,0,0,0};
    matCond[8] = {9,9,9,0,0,0};
    matCond[9] = {10,10,10,0,0,0};

    // Running simulation
    puma::Vec3<double> k = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'x',1e-3,10000,true);

    cout << endl << "Conductivity: " << endl;
    cout << "kxx " << k.x << " kxy " << k.y << " kxz " << k.z << endl;
    
    return 0;
}
