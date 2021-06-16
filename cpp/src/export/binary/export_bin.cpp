#include "export_bin.h"


bool puma::export_bin(Workspace *work, const std::string& fileName) {
    Export_bin<short> exporter(&(work->matrix),fileName);
    return exporter.execute();
}

bool puma::export_bin(Matrix<double> *matrix, const std::string& fileName) {
    Export_bin<double> exporter(matrix,fileName);
    return exporter.execute();
}
bool puma::export_bin(Matrix<float> *matrix, const std::string& fileName) {
    Export_bin<float> exporter(matrix,fileName);
    return exporter.execute();
}
bool puma::export_bin(Matrix<short> *matrix, const std::string& fileName) {
    Export_bin<short> exporter(matrix,fileName);
    return exporter.execute();
}
bool puma::export_bin(Matrix<int> *matrix, const std::string& fileName) {
    Export_bin<int> exporter(matrix,fileName);
    return exporter.execute();
}
bool puma::export_bin(Matrix<long> *matrix, const std::string& fileName) {
    Export_bin<long> exporter(matrix,fileName);
    return exporter.execute();
}

bool puma::export_bin(puma::MatVec3<double> *matrix, std::string fileName) {
    Export_bin_MatrixVec3 exporter(matrix,std::move(fileName));
    return exporter.execute();
}



template<class T> Export_bin<T>::Export_bin(puma::Matrix<T> *matrix, const std::string& fileName) {
    this->matrix = matrix;
    this->fileName = fileName;
}


template<class T> bool Export_bin<T>::execute() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed

        std::cout << "Error in Exportbin: " << errorMessage << std::endl;
        return false;
    }

    std::cout << "Outputting binary file: " << fileName+".puma" << " ... " << std::flush;
    bool success = exportHelper();

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

template<class T> bool Export_bin<T>::exportHelper() {

    FILE *file;
    int n = fileName.length();
    char fileName_char_array[n + 1];
    strcpy(fileName_char_array, (fileName+".puma").c_str());
    file = fopen(fileName_char_array,"w");

    if (file != NULL) {
        for (long k=0; k<matrix->Z(); k++) {
            for (long j=0; j<matrix->Y(); j++) {
                for (long i=0; i<matrix->X(); i++) {
                    fwrite(&(matrix->at(i,j,k)), sizeof(T), 1, file);
                }
            }
        }
        T X = matrix->X(), Y = matrix->Y(), Z = matrix->Z();
        fwrite(&X, sizeof(T), 1, file);
        fwrite(&Y, sizeof(T), 1, file);
        fwrite(&Z, sizeof(T), 1, file);

        fclose(file);
    } else {
        std::cerr <<  "Could not create file" << std::endl;
        return false;
    }

    return true;
}


template<class T> bool Export_bin<T>::errorCheck(std::string *errorMessage) {
    if(matrix->size() == 0) {
        *errorMessage = "Empty Matrix";
        return false;
    }

    if(fileName == "") {
        *errorMessage = "Blank File Name";
        return false;
    }

    if(puma::PString::ends_with(fileName,".puma")) {
        fileName = puma::PString::replaceAll(fileName,".puma","");
    }


    return true;
}

template<class T> bool Export_bin<T>::logInput() {

    return true;
}

template<class T> bool Export_bin<T>::logOutput() {

    return true;
}




Export_bin_MatrixVec3::Export_bin_MatrixVec3(puma::MatVec3<double> *matrix, std::string fileName) {
    this->matrix = matrix;
    this->fileName = fileName;

    if(matrix->size()==0) {
        empty = true;
    }
    else {
        empty = false;
    }
}


bool Export_bin_MatrixVec3::execute() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed

        std::cout << "Error in Exportbin: " << errorMessage << std::endl;
        return false;
    }

    std::cout << "Outputting binary file: " << fileName+".puma" << " ... " << std::flush;
    bool success = exportHelper();

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

bool Export_bin_MatrixVec3::exportHelper() {

    FILE *file;
    int n = fileName.length();
    char fileName_char_array[n + 1];
    strcpy(fileName_char_array, (fileName+".puma").c_str());
    file = fopen(fileName_char_array,"w");

    if (file != NULL) {
        for (long k=0; k<matrix->Z(); k++) {
            for (long j=0; j<matrix->Y(); j++) {
                for (long i=0; i<matrix->X(); i++) {
                    fwrite(&((matrix->at(i,j,k)).x), sizeof(double), 1, file);
                }
            }
        }
        for (long k=0; k<matrix->Z(); k++) {
            for (long j=0; j<matrix->Y(); j++) {
                for (long i=0; i<matrix->X(); i++) {
                    fwrite(&((matrix->at(i,j,k)).y), sizeof(double), 1, file);
                }
            }
        }
        for (long k=0; k<matrix->Z(); k++) {
            for (long j=0; j<matrix->Y(); j++) {
                for (long i=0; i<matrix->X(); i++) {
                    fwrite(&((matrix->at(i,j,k)).z), sizeof(double), 1, file);
                }
            }
        }
        double X = matrix->X(), Y = matrix->Y(), Z = matrix->Z();
        fwrite(&X, sizeof(double), 1, file);
        fwrite(&Y, sizeof(double), 1, file);
        fwrite(&Z, sizeof(double), 1, file);

        fclose(file);
    } else {
        std::cerr <<  "Could not create file" << std::endl;
        return false;
    }

    return true;
}


bool Export_bin_MatrixVec3::errorCheck(std::string *errorMessage) {
    if(empty) {
        *errorMessage = "Empty Matrix";
        return false;
    }

    if(fileName == "") {
        *errorMessage = "Blank File Name";
        return false;
    }

    if(puma::PString::ends_with(fileName,".puma")) {
        fileName = puma::PString::replaceAll(fileName,".puma","");
    }

    return true;
}

bool Export_bin_MatrixVec3::logInput() {

    return true;
}

bool Export_bin_MatrixVec3::logOutput() {

    return true;
}
