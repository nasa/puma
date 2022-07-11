#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

// tortuosity: compute_FVTortuosity
RandomFibersInput input;
input.straightCircle(200,200,200,5,0,200,0,0,0,0,true,0.6,1);

puma::Workspace grayWS;
generateRandomFibers(&grayWS,input);

puma::Matrix<double> C;
puma::Vec3<double> tx = compute_FVTortuosity(&grayWS, &C, puma::Cutoff(0,127), "symmetric", "conjugategradient", 'x', 1e-3, 10000, true, 0);
puma::Vec3<double> ty = compute_FVTortuosity(&grayWS, &C, puma::Cutoff(0,127), "symmetric", "conjugategradient", 'y', 1e-3, 10000, true, 0);
puma::Vec3<double> tz = compute_FVTortuosity(&grayWS, &C, puma::Cutoff(0,127), "symmetric", "conjugategradient", 'z', 1e-3, 10000, true, 0);


cout << "tx: " << tx.x << ' ' << tx.y << ' ' << tx.z << endl;
cout << "tx: " << ty.x << ' ' << ty.y << ' ' << ty.z << endl;
cout << "tx: " << tz.x << ' ' << tz.y << ' ' << tz.z << endl;
    
    return 0;
}
