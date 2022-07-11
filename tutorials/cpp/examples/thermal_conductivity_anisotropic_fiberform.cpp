#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

// fvanisotropicthermalconductivity: FiberForm 100^3
    cout << endl << "Computing the thermal conductivity using finite volume considering a locally anisotropic FiberForm 100^3 sample:"<< endl;

// Initializing Workspace
    puma::Workspace ws(1e-6, false);

// Importing 3D tiff
    puma::import_3DTiff(&ws,puma::path_to_example_file("100_fiberform.tif"));

// Computing orientations using Structure Tensor (ST) method
    puma::MatVec3< double> tangents;
    puma::compute_orientationST(&ws, 0.7, 0.7, puma::Cutoff(89,255), &tangents);

// Initializing Temperature, flux field and material conductivity
    puma::Matrix<double> T;
    puma::MatVec3<double> q;
    map<int, vector<double>> matCond;
    matCond[89] = {0.0257}; // air conductivity
    matCond[255] = {15, 8}; // approximation to fiber conductivity, here considered isotropic as an example

// Running simulation in three directions
    puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond, &tangents, "mpfa", "symmetric","bicgstab",'x',1e-3,10000,true);
    puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond, &tangents, "mpfa", "symmetric","bicgstab",'y',1e-3,10000,true);
    puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond, &tangents, "mpfa", "symmetric","bicgstab",'z',1e-3,10000,true);

    cout << endl << "Conductivity: " << endl;
    cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << endl;
    cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << endl;
    cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << endl;
    
    return 0;
}
