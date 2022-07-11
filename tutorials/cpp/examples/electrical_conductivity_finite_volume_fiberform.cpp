#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

// fvelectricalconductivity: FiberForm 100^3
    cout << endl << "Computing the electrical conductivity using finite volume considering a locally isotropic FiberForm 100^3 sample:"<< endl;

// Initializing Workspace
    puma::Workspace ws(1e-6, false);

    // Importing 3D tiff
    puma::import_3DTiff(&ws,puma::path_to_example_file("100_fiberform.tif"));

    // Segmenting workspace by thresholding
    ws.setMaterialID(&ws,puma::Cutoff(0,89),0);
    ws.setMaterialID(&ws,puma::Cutoff(90,255),1);

// Initializing Voltage Potential field and material conductivity
    puma::Matrix<double> V;
    map<int, double> matCond;
    matCond[0] = 1e-12; // air conductivity
    matCond[1] = 8e5; // approximation to fiber conductivity

// Running simulation in three directions
    puma::Vec3<double> sigma_x = puma::compute_FVElectricalConductivity(&ws, &V, matCond,"symmetric","cg",'x',1e-3,10000,true);
    puma::Vec3<double> sigma_y = puma::compute_FVElectricalConductivity(&ws, &V, matCond,"symmetric","cg",'y',1e-3,10000,true);
    puma::Vec3<double> sigma_z = puma::compute_FVElectricalConductivity(&ws, &V, matCond,"symmetric","cg",'z',1e-3,10000,true);

    cout << endl << "Conductivity: " << endl;
    cout << "sigma_xx " << sigma_x.x << " sigma_xy " << sigma_x.y << " sigma_xz " << sigma_x.z << endl;
    cout << "sigma_yx " << sigma_y.x << " sigma_yy " << sigma_y.y << " sigma_yz " << sigma_y.z << endl;
    cout << "sigma_zx " << sigma_z.x << " sigma_zy " << sigma_z.y << " sigma_zz " << sigma_z.z << endl;
    
    return 0;
}
