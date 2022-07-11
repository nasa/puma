#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

    // Generating a fibrous material to use for pore space identification
    puma::Workspace grayWS;
    RandomFibersInput input;
    input.straightFlower_Hollow(200,200,1, 30, 0, 1, 0, 2, 0, 2, true, 0.6, 2, 1, 0, 0, 0, 0, 1, 20, 0);
    generateRandomFibers(&grayWS,input);

    puma::Matrix<long> poreMatrix;
    puma::identifyPoreSpace(&grayWS,&poreMatrix,puma::Cutoff(0,127));

    double average =  poreMatrix.average();

    cout << "Average porespace: " << average << endl;

    puma::export_3DTiff(&grayWS,"out/pore_space_before_fill",false);

    puma::fillClosedPores(&grayWS, puma::Cutoff(0, 127), 255);

    puma::export_3DTiff(&grayWS,"out/pore_space_after_fill",false);
    
    return 0;
}
