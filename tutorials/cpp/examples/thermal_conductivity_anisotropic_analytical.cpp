#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

    // fvanisotropicthermalconductivity: homogeneous analytical solution
    cout << endl << "Computing the thermal conductivity of an anisotropic slab with a prescribed boundary temperature:"<< endl;

// Domain size
    int X = 22, Y = 102, Z = 3;

// Initializing datastructures
    puma::Workspace segWS(X, Y, Z, 0, 1e-6, false);
    puma::Matrix<double> T;
    puma::MatVec3<double> q;
    std::map<int, std::vector<double>> matCond;

// Assigning conductivity
    matCond[0] = {1, 1, 1, 0.75, 0.75, 0.75}; // kxx,kyy,kzz,kxy,kxz,kyz

// Initialize prescribed boundary temperature
    puma::Matrix<double> prescribedBC(2, Y, Z, 0);

// Assigning point temperature in the middle of the top surface
    for (int k = 0; k < Z; k++) {
        for (int j = Y/2-1; j < Y/2+1; j++) {
            prescribedBC.at(0, j, k) = 1;
        }
    }

// Running anisotropic conductivity solver
    puma::Vec3<double> kx = compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "symmetric", &prescribedBC,"bicgstab", 'x', 1e-8, 10000, false);

// Printing slice of the temperature field
    T.printSlice('z', 2, 12);
    
    return 0;
}
