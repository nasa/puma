#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

    double p = 0.2;
    double q = 0.5045;
    bool exportSTL = true;
    int equationNumber = 0; // equation 0, 1 or 2

    std::string outputFolder = "out/";
    std::string outputLabel = "equation0";

    int voxelsX = 400;
    int voxelsY = 400;
    int voxelsZ = 400;

    puma::Workspace ws(1e-6,false);

//  triply periodic minimal surface (TPMS)
    TPMSInput input(voxelsX,voxelsY,voxelsZ,p,p,q,q, equationNumber);
    bool success = puma::generateTPMS(&ws, input);


    if (success) {
        std::cout << "Success" << std::endl;
        if(exportSTL){
            puma::export_STL(&ws, puma::Cutoff(128, 255), false, outputFolder + outputLabel +"_tpms_test.stl");
        }

    }
    
    return 0;
}
