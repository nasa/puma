#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

    // tortuosity: compute_EJTortuosity
RandomFibersInput input;
input.straightCircle(200,200,200,5,0,200,0,0,0,0,true,0.6,1);

puma::Workspace grayWS;
generateRandomFibers(&grayWS,input);

puma::Matrix<double> Cx, Cy, Cz;
puma::Vec3<double> tx = compute_EJTortuosity(&grayWS, &Cx, puma::Cutoff(0,127), 'x', 1e-3, 10000, true, 0);
puma::Vec3<double> ty = compute_EJTortuosity(&grayWS, &Cy, puma::Cutoff(0,127), 'y', 1e-3, 10000, true, 0);
puma::Vec3<double> tz = compute_EJTortuosity(&grayWS, &Cz, puma::Cutoff(0,127), 'z', 1e-3, 10000, true, 0);

cout << "tx: " << tx.x << ' ' << tx.y << ' ' << tx.z << endl;
cout << "ty: " << ty.x << ' ' << ty.y << ' ' << ty.z << endl;
cout << "tz: " << ty.x << ' ' << tz.y << ' ' << tz.z << endl;
    
return 0;

}
