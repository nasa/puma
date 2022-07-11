#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

    // fvthermalconductivity: FiberForm 100^3
    cout << endl << "Computing the thermal conductivity using finite volume considering a locally isotropic FiberForm 100^3 sample:"<< endl;

    // Initializing Workspace
    puma::Workspace ws(1e-6, false);

    // Importing 3D tiff
    puma::import_3DTiff(&ws,puma::path_to_example_file("100_fiberform.tif"));

    // Segmenting workspace by thresholding
    ws.setMaterialID(&ws,puma::Cutoff(0,89),0);
    ws.setMaterialID(&ws,puma::Cutoff(90,255),1);

    // Initializing Temperature field and material conductivity
    puma::Matrix<double> T;
    map<int, double> matCond;
    matCond[0] = 0.0257; // air conductivity
    matCond[1] = 12; // approximation to fiber conductivity

    // Running simulation in three directions
    puma::Vec3<double> kx = puma::compute_FVThermalConductivity(&ws, &T, matCond,"symmetric","cg",'x',1e-3,10000,true);
    puma::Vec3<double> ky = puma::compute_FVThermalConductivity(&ws, &T, matCond,"symmetric","cg",'y',1e-3,10000,true);
    puma::Vec3<double> kz = puma::compute_FVThermalConductivity(&ws, &T, matCond,"symmetric","cg",'z',1e-3,10000,true);

    cout << endl << "Conductivity: " << endl;
    cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << endl;
    cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << endl;
    cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << endl;
    
    return 0;
}
