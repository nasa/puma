#include "export_STL.h"


bool puma::export_STL(std::vector< puma::Triangle<float> > *triangles, bool Ascii, std::string fileName) {
    Export_STL out(triangles, Ascii, std::move(fileName));
    return out.execute();
}

bool puma::export_STL(Workspace *work, puma::Cutoff cutoff, bool Ascii, std::string fileName) {
    puma::TriangleVector triangles;

    puma::isosurface_LewinerMarchingCubes(&triangles,work,cutoff,true,1,true,0);


    return puma::export_STL(&triangles,Ascii,std::move(fileName));
}


Export_STL::Export_STL(std::vector< puma::Triangle<float> > *triangles, bool Ascii, std::string fileName) {
    this->triangles = triangles;
    this->fileName = std::move(fileName);
    this->Ascii = Ascii;
}

bool Export_STL::execute() {
    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed
        std::cout << "Error in Export STL: " << errorMessage << std::endl;
        return false;
    }

    bool success;

    std::cout << "Outputting stl file: " << fileName << " ... " << std::flush;

    if( Ascii ) {
        success = Export_STL_helper::writeSTLAscii(triangles,fileName);
    }
    else {
        success = Export_STL_helper::writeSTL(triangles,fileName);
    }

    if(!success) {
        return false;
    }


    if( !logOutput() ) {
        std::cout << "Failed to log Outputs" << std::endl;
        return false;
    }

    std::cout << "Done" << std::endl;

    return true;
}

bool Export_STL::logInput() {
    return true;
}

bool Export_STL::logOutput(){
    return true;
}

bool Export_STL::errorCheck(std::string *errorMessage) {
    if(fileName.empty()) {
        *errorMessage = "Blank File Name";
        return false;
    }

    if( !puma::PString::ends_with(fileName,".stl") ) {
        fileName.append(".stl");
    }

    if( triangles->empty() ) {
        *errorMessage = "Empty Triangle vector";
        return false;
    }

    return true;
}
