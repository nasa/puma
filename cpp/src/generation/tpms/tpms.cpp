#include "tpms.h"


// Generation of triply periodic minimal surface (TPMS)

//if you want the result to be continuous (non-thresholded)
bool puma::generateTPMS(puma::Workspace *tpmsWS, TPMSInput input) {

    Generate_TPMS generator(tpmsWS,input);
    return generator.generate();
}


Generate_TPMS::Generate_TPMS(puma::Workspace *tpmsWS, TPMSInput input) {
    this->tpmsWS = tpmsWS;
    this->input = input;
}


bool Generate_TPMS::generate() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed
        std::cout << "Error in TPMS Generation: " << errorMessage << std::endl;
        return false;
    }

    bool success = TPMSGenerationHelper();
    if(!success){
        return false;
    }

    if( !logOutput() ) {
        std::cout << "Failed to log Outputs" << std::endl;
        return false;
    }

    return true;
}


bool Generate_TPMS::TPMSGenerationHelper() {

    double wMin = input.wMin;
    double wMax = input.wMax;
    double qMin = input.qMin;
    double qMax = input.qMax;

    puma::Matrix<double> mat(input.xSize,input.ySize,input.zSize);
    tpmsWS->matrix.resize(input.xSize,input.ySize,input.zSize);

#pragma omp parallel for
    for(int i=0;i<input.xSize;i++) {
        for(int j=0;j<input.ySize;j++) {
            for(int k=0;k<input.zSize;k++) {


                double q = qMin + ((double)k/(double)(input.zSize-1))*(qMax-qMin);
                double w = wMin + ((double)k/(double)(input.zSize-1))*(wMax-wMin);

                if(input.equation == 0) {
                    mat(i, j, k) =
                            std::sin(w * i) * std::sin(w * j) * std::sin(w * k) +
                            std::sin(w * i) * std::cos(w * j) * std::cos(w * k) +
                            std::cos(w * i) * std::sin(w * j) * std::cos(w * k) +
                            std::cos(w * i) * std::cos(w * j) * std::sin(w * k) + q;
                }
                else if(input.equation == 1) {
                    mat(i, j, k) =
                            std::cos(w * i) * std::sin(w * j) +
                            std::cos(w * j) * std::sin(w * k) +
                            std::cos(w * k) * std::cos(w * i) + q;
                }
                else if(input.equation == 2) {
                    mat(i, j, k) =
                             std::cos(w * i) + std::cos(w * j) + std::cos(w * k) + q;
                }
            }
        }
    }


    std::cout << mat.max() << " " << mat.min() << std::endl;

    for(long i=0;i<mat.size();i++) {
        if(mat(i) < 0.8) mat(i)=0.8;
        if(mat(i) > 1.2) mat(i)=1.2;
    }

     std::cout << mat.max() << " " << mat.min() << std::endl;

    mat.normalize(0,255);


#pragma omp parallel for
    for(int i=0;i<input.xSize;i++) {
        for(int j=0;j<input.ySize;j++) {
            for(int k=0;k<input.zSize;k++) {
                 tpmsWS->matrix(i,j,k)=(short)mat(i,j,k);
            }
        }
    }


    return true;
}


bool Generate_TPMS::errorCheck(std::string *errorMessage) {

    if(input.xSize <= 0) {
        (*errorMessage) = "xSize invalid. Must be >0";
        return false;
    }
    if(input.ySize <= 0) {
        (*errorMessage) = "ySize invalid. Must be >0";
        return false;
    }
    if(input.zSize <= 0) {
        (*errorMessage) = "zSize invalid. Must be >0";
        return false;
    }
    if(input.wMin <= 0) {
        (*errorMessage) = "wMin invalid. Must be >0";
        return false;
    }
    if(input.wMax < input.wMin) {
        (*errorMessage) = "wMin invalid. Must be >=0";
        return false;
    }
    if(input.qMin <= 0) {
        (*errorMessage) = "qMin invalid. Must be >0";
        return false;
    }
    if(input.qMax < input.qMin) {
        (*errorMessage) = "qMax invalid. Must be >=0";
        return false;
    }

    if(input.equation != 0 && input.equation != 1 && input.equation != 2) {
        (*errorMessage) = "Invalid input equation. Must be 0, 1, or 2";
        return false;
    }

    return true;
}


bool Generate_TPMS::logInput() {
    puma::Logger *log;
    log = tpmsWS->log;

    (*log).appendLogSection("Generate TPMS Structure");
    (*log).appendLogItem("Time: ");
    (*log).appendLogItem(puma::Logger::getTime());
    (*log).newLine();
    (*log).appendLogLine(" -- Inputs:");
    (*log).appendLogItem("Domain Size: ");
    (*log).appendLogItem(input.xSize);
    (*log).appendLogItem(", ");
    (*log).appendLogItem(input.ySize);
    (*log).appendLogItem(", ");
    (*log).appendLogItem(input.zSize);
    (*log).newLine();
    (*log).appendLogItem("wMin: ");
    (*log).appendLogItem(input.wMin);
    (*log).newLine();
    (*log).appendLogItem("wMax: ");
    (*log).appendLogItem(input.wMax);
    (*log).newLine();
    (*log).appendLogItem("qMin: ");
    (*log).appendLogItem(input.qMin);
    (*log).newLine();
    (*log).appendLogItem("qMax: ");
    (*log).appendLogItem(input.qMax);
    (*log).newLine();
    (*log).appendLogItem("Equation ID: ");
    (*log).appendLogItem(input.equation);
    (*log).newLine();

    (*log).writeLog();

    return true;

}


bool Generate_TPMS::logOutput() {
    puma::Logger *log;
    log = tpmsWS->log;

    (*log).appendLogItem("Successful TPMS Generation");
    (*log).newLine();
    (*log).appendLogItem("Time: ");
    (*log).appendLogItem(puma::Logger::getTime());
    (*log).newLine();

    (*log).writeLog();

    return true;
}
