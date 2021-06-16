#include "export_vtk.h"


bool puma::export_vtk(puma::Matrix<double> *matrix, const std::string& fileName, char fileType, bool unstructured, int numThreads) {
    Export_vtk_Matrix<double> exporter(matrix,fileName,fileType,unstructured,numThreads);
    return exporter.execute();
}
bool puma::export_vtk(puma::Matrix<float> *matrix, const std::string& fileName, char fileType, bool unstructured, int numThreads) {
    Export_vtk_Matrix<float> exporter(matrix,fileName,fileType,unstructured,numThreads);
    return exporter.execute();
}
bool puma::export_vtk(puma::Matrix<short> *matrix, const std::string& fileName, char fileType, bool unstructured, int numThreads) {
    Export_vtk_Matrix<short> exporter(matrix,fileName,fileType,unstructured,numThreads);
    return exporter.execute();
}
bool puma::export_vtk(puma::Matrix<int> *matrix, const std::string& fileName, char fileType, bool unstructured, int numThreads) {
    Export_vtk_Matrix<int> exporter(matrix,fileName,fileType,unstructured,numThreads);
    return exporter.execute();
}
bool puma::export_vtk(puma::Matrix<long> *matrix, const std::string& fileName, char fileType, bool unstructured, int numThreads) {
    Export_vtk_Matrix<long> exporter(matrix,fileName,fileType,unstructured,numThreads);
    return exporter.execute();
}

bool puma::export_vtk(puma::MatVec3<double> *matrix, std::string fileName, char fileType, bool unstructured, int numThreads) {
    Export_vtk_MatVec3 exporter(matrix,std::move(fileName),fileType,unstructured,numThreads);
    return exporter.execute();
}

bool puma::export_vtk(Workspace *work, std::string fileName, char fileType, bool unstructured, int numThreads) {
    Export_vtk_Workspace exporter(work,std::move(fileName),fileType,unstructured,numThreads);
    return exporter.execute();
}


// Swap bits so that Paraview can read it
template <typename T>
void SwapEnd(T& var){
    char* varArray = reinterpret_cast<char*>(&var);
    for(long i = 0; i < static_cast<long>(sizeof(var)/2); i++)
        std::swap(varArray[sizeof(var) - 1 - i],varArray[i]);
}


template<class T> Export_vtk_Matrix<T>::Export_vtk_Matrix(puma::Matrix<T> *matrix, const std::string& fileName, char fileType, bool unstructured, int numThreads) {
    this->matrix = matrix;
    this->fileName = fileName;
    this->fileType = fileType;
    this->unstructured = unstructured;
    this->numThreads = numThreads;

    empty = this->matrix->size() == 0;
}

template<class T> bool Export_vtk_Matrix<T>::execute() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    std::string check = ".vtk";
    if(strcmp(fileName.substr(fileName.length()-4, fileName.length()).c_str(), check.c_str())!=0){
        fileName += check;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed
        std::cout << "Error in Export_vtk: " << errorMessage << std::endl;
        return false;
    }

    std::cout << "Outputting VTK file: " << fileName << " ... " << std::flush;
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

template<class T> bool Export_vtk_Matrix<T>::exportHelper() {

    char s[256];
    FILE *file;
    int n = fileName.length();
    char fileName_char_array[n + 1];
    strcpy(fileName_char_array, (fileName).c_str());
    file = fopen(fileName_char_array,"w");
    if (file != nullptr) {
        strcpy(s, "# vtk DataFile Version 3.0\n"); fwrite(s, sizeof(char), strlen(s), file);
        strcpy(s, "VTK from PuMA\n"); fwrite(s, sizeof(char), strlen(s), file);
        if(fileType == 'b') {
            if (!unstructured){
                strcpy(s, "BINARY\n"); fwrite(s, sizeof(char), strlen(s), file);
            } else {
                std::cout << "When unstructured == 1, only ASCII is available. Defaulting to ASCII output." << std::endl;
                strcpy(s, "ASCII\n"); fwrite(s, sizeof(char), strlen(s), file);
            }
        } else {
            strcpy(s, "ASCII\n"); fwrite(s, sizeof(char), strlen(s), file);
        }
        if (!unstructured) {
            strcpy(s, "DATASET STRUCTURED_POINTS\n");
            fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "DIMENSIONS %d %d %d\n", (int) matrix->X() + 1, (int) matrix->Y() + 1, (int) matrix->Z() + 1);
            fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "SPACING %d %d %d\n", 1, 1, 1);
            fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "ORIGIN %d %d %d\n", 0, 0, 0);
            fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "CELL_DATA %d\n", (int) matrix->size());
            fwrite(s, sizeof(char), strlen(s), file);
            strcpy(s, "SCALARS PuMA_Matrix float\nLOOKUP_TABLE default\n");
            fwrite(s, sizeof(char), strlen(s), file);
            if (fileType == 'b') {  // BINARY
                for (long k = 0; k < matrix->Z(); k++) {
                    for (long j = 0; j < matrix->Y(); j++) {
                        for (long i = 0; i < matrix->X(); i++) {
                            auto temp = (float) matrix->at(i, j, k);
                            SwapEnd(temp);
                            fwrite(&temp, sizeof(float), 1, file);
                        }
                    }
                }
            } else {  // ASCII
                for (long k = 0; k < matrix->Z(); k++) {
                    for (long j = 0; j < matrix->Y(); j++) {
                        for (long i = 0; i < matrix->X(); i++) {
                            auto temp = std::to_string((float) matrix->at(i, j, k)) + ' ';
                            strcpy(s, temp.c_str());
                            fwrite(s, sizeof(char), strlen(s), file);
                        }
                    }
                }
            }

        } else {
            strcpy(s, "DATASET UNSTRUCTURED_GRID\n");
            fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "POINTS %ld float\n", (matrix->X()+1)*(matrix->Y()+1)*(matrix->Z()+1));
            fwrite(s, sizeof(char), strlen(s), file);
            puma::Matrix<long> pointsID(matrix->X()+1, matrix->Y()+1, matrix->Z()+1);
            int counter = 0;
            for (long k = 0; k < matrix->Z() + 1; k++) {
                for (long j = 0; j < matrix->Y() + 1; j++) {
                    for (long i = 0; i < matrix->X() + 1; i++) {
                        sprintf(s, "%ld %ld %ld ", i, j, k);
                        fwrite(s, sizeof(char), strlen(s), file);
                        pointsID.at(i, j, k) = counter;
                        counter++;
                    }
                }
            }
            sprintf(s, "\n\nCELLS %ld %ld \n", matrix->size(), matrix->size() * 9);
            fwrite(s, sizeof(char), strlen(s), file);
            for (long k = 0; k < matrix->Z(); k++) {
                for (long j = 0; j < matrix->Y(); j++) {
                    for (long i = 0; i < matrix->X(); i++) {
                        sprintf(s, "8 %ld %ld %ld %ld %ld %ld %ld %ld \n",
                                pointsID.at(i, j, k), pointsID.at(i + 1, j, k), pointsID.at(i + 1, j + 1, k),
                                pointsID.at(i, j + 1, k),
                                pointsID.at(i, j, k + 1), pointsID.at(i + 1, j, k + 1),
                                pointsID.at(i + 1, j + 1, k + 1), pointsID.at(i, j + 1, k + 1));
                        fwrite(s, sizeof(char), strlen(s), file);
                    }
                }
            }
            sprintf(s, "\nCELL_TYPES %ld \n", matrix->size());
            fwrite(s, sizeof(char), strlen(s), file);
            strcpy(s, "12 ");
            for (long k = 0; k < matrix->Z(); k++) {
                for (long j = 0; j < matrix->Y(); j++) {
                    for (long i = 0; i < matrix->X(); i++) {
                        fwrite(s, sizeof(char), strlen(s), file);
                    }
                }
            }
            sprintf(s, "\nCELL_DATA %ld\n", matrix->size());
            fwrite(s, sizeof(char), strlen(s), file);
            strcpy(s, "SCALARS PuMA_Matrix float\nLOOKUP_TABLE default\n");
            fwrite(s, sizeof(char), strlen(s), file);
            for (long k = 0; k < matrix->Z(); k++) {
                for (long j = 0; j < matrix->Y(); j++) {
                    for (long i = 0; i < matrix->X(); i++) {
                        auto temp = std::to_string((float) matrix->at(i, j, k)) + ' ';
                        strcpy(s, temp.c_str());
                        fwrite(s, sizeof(char), strlen(s), file);
                    }
                }
            }
        }
        fclose(file);
    } else {
        return false;
    }

    return true;
}

template<class T> bool Export_vtk_Matrix<T>::errorCheck(std::string *errorMessage) {

    if(empty) {
        *errorMessage = "Empty Matrix";
        return false;
    }

    if(fileName.empty()) {
        *errorMessage = "Blank File Name";
        return false;
    }

    if(!(fileType == 'a' || fileType == 'b')) {
        *errorMessage = "Invalid fileType";
        return false;
    }

    return true;
}


template<class T> bool Export_vtk_Matrix<T>::logInput() {

    return true;
}


template<class T> bool Export_vtk_Matrix<T>::logOutput() {

    return true;
}




Export_vtk_MatVec3::Export_vtk_MatVec3(puma::MatVec3<double> *matrix, std::string fileName, char fileType, bool unstructured, int numThreads) {
    this->matrix = matrix;
    this->fileName = std::move(fileName);
    this->fileType = fileType;
    this->unstructured = unstructured;
    this->numThreads = numThreads;

    empty = this->matrix->size() == 0;
}

bool Export_vtk_MatVec3::execute() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    std::string check = ".vtk";
    if(strcmp(fileName.substr(fileName.length()-4, fileName.length()).c_str(), check.c_str())!=0){
        fileName += check;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed
        std::cout << "Error in Export_vtk: " << errorMessage << std::endl;
        return false;
    }

    std::cout << "Outputting VTK file: " << fileName << " ... " << std::flush;
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

bool Export_vtk_MatVec3::exportHelper() {

    char s[256];
    FILE *file;
    int n = fileName.length();
    char fileName_char_array[n + 1];
    strcpy(fileName_char_array, (fileName).c_str());
    file = fopen(fileName_char_array,"w");
    if (file != nullptr) {
        strcpy(s, "# vtk DataFile Version 3.0\n"); fwrite(s, sizeof(char), strlen(s), file);
        strcpy(s, "VTK from PuMA\n"); fwrite(s, sizeof(char), strlen(s), file);
        if(fileType == 'b') {
            if (!unstructured){
                strcpy(s, "BINARY\n"); fwrite(s, sizeof(char), strlen(s), file);
            } else {
                std::cout << "When unstructured == 1, only ASCII is available. Defaulting to ASCII output." << std::endl;
                strcpy(s, "ASCII\n"); fwrite(s, sizeof(char), strlen(s), file);
            }
        } else {
            strcpy(s, "ASCII\n"); fwrite(s, sizeof(char), strlen(s), file);
        }
        if (!unstructured) {
            strcpy(s, "DATASET STRUCTURED_POINTS\n"); fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "DIMENSIONS %d %d %d\n", (int)matrix->X()+1, (int)matrix->Y()+1, (int)matrix->Z()+1); fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "SPACING %d %d %d\n", 1, 1, 1); fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "ORIGIN %d %d %d\n", 0, 0, 0); fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "CELL_DATA %d\n", (int)matrix->size()); fwrite(s, sizeof(char), strlen(s), file);
            strcpy(s, "VECTORS PuMA_Vec3 float\n"); fwrite(s, sizeof(char), strlen(s), file);

            if(fileType == 'b') {
                for (long k=0; k<matrix->Z(); k++) {
                    for (long j=0; j<matrix->Y(); j++) {
                        for (long i=0; i<matrix->X(); i++) {
                            auto tempX = (float)(matrix->at(i,j,k)).x;
                            auto tempY = (float)(matrix->at(i,j,k)).y;
                            auto tempZ = (float)(matrix->at(i,j,k)).z;
                            SwapEnd(tempX), SwapEnd(tempY), SwapEnd(tempZ);
                            fwrite(&tempX, sizeof(float), 1, file);
                            fwrite(&tempY, sizeof(float), 1, file);
                            fwrite(&tempZ, sizeof(float), 1, file);
                        }
                    }
                }
            } else {
                for (long k=0; k<matrix->Z(); k++) {
                    for (long j=0; j<matrix->Y(); j++) {
                        for (long i=0; i<matrix->X(); i++) {
                            auto tempX = std::to_string((float)(matrix->at(i,j,k)).x) + ' ';
                            auto tempY = std::to_string((float)(matrix->at(i,j,k)).y) + ' ';
                            auto tempZ = std::to_string((float)(matrix->at(i,j,k)).z) + ' ';
                            strcpy(s, tempX.c_str()); fwrite(s, sizeof(char), strlen(s), file);
                            strcpy(s, tempY.c_str()); fwrite(s, sizeof(char), strlen(s), file);
                            strcpy(s, tempZ.c_str()); fwrite(s, sizeof(char), strlen(s), file);
                        }
                    }
                }
            }
        } else {
            strcpy(s, "DATASET UNSTRUCTURED_GRID\n");
            fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "POINTS %ld float\n", (matrix->X()+1)*(matrix->Y()+1)*(matrix->Z()+1));
            fwrite(s, sizeof(char), strlen(s), file);
            puma::Matrix<long> pointsID(matrix->X()+1, matrix->Y()+1, matrix->Z()+1);
            int counter = 0;
            for (long k = 0; k < matrix->Z() + 1; k++) {
                for (long j = 0; j < matrix->Y() + 1; j++) {
                    for (long i = 0; i < matrix->X() + 1; i++) {
                        sprintf(s, "%ld %ld %ld ", i, j, k);
                        fwrite(s, sizeof(char), strlen(s), file);
                        pointsID.at(i, j, k) = counter;
                        counter++;
                    }
                }
            }
            sprintf(s, "\n\nCELLS %ld %ld \n", matrix->size(), matrix->size() * 9);
            fwrite(s, sizeof(char), strlen(s), file);
            for (long k = 0; k < matrix->Z(); k++) {
                for (long j = 0; j < matrix->Y(); j++) {
                    for (long i = 0; i < matrix->X(); i++) {
                        sprintf(s, "8 %ld %ld %ld %ld %ld %ld %ld %ld \n",
                                pointsID.at(i, j, k), pointsID.at(i + 1, j, k), pointsID.at(i + 1, j + 1, k),
                                pointsID.at(i, j + 1, k),
                                pointsID.at(i, j, k + 1), pointsID.at(i + 1, j, k + 1),
                                pointsID.at(i + 1, j + 1, k + 1), pointsID.at(i, j + 1, k + 1));
                        fwrite(s, sizeof(char), strlen(s), file);
                    }
                }
            }
            sprintf(s, "\nCELL_TYPES %ld \n", matrix->size());
            fwrite(s, sizeof(char), strlen(s), file);
            strcpy(s, "12 ");
            for (long k = 0; k < matrix->Z(); k++) {
                for (long j = 0; j < matrix->Y(); j++) {
                    for (long i = 0; i < matrix->X(); i++) {
                        fwrite(s, sizeof(char), strlen(s), file);
                    }
                }
            }
            sprintf(s, "\nCELL_DATA %ld\n", matrix->size());
            fwrite(s, sizeof(char), strlen(s), file);
            strcpy(s, "VECTORS PuMA_Vec3 float\n");
            fwrite(s, sizeof(char), strlen(s), file);
            for (long k = 0; k < matrix->Z(); k++) {
                for (long j = 0; j < matrix->Y(); j++) {
                    for (long i = 0; i < matrix->X(); i++) {
                        auto tempX = std::to_string((float)(matrix->at(i,j,k)).x) + ' ';
                        auto tempY = std::to_string((float)(matrix->at(i,j,k)).y) + ' ';
                        auto tempZ = std::to_string((float)(matrix->at(i,j,k)).z) + ' ';
                        strcpy(s, tempX.c_str()); fwrite(s, sizeof(char), strlen(s), file);
                        strcpy(s, tempY.c_str()); fwrite(s, sizeof(char), strlen(s), file);
                        strcpy(s, tempZ.c_str()); fwrite(s, sizeof(char), strlen(s), file);
                    }
                }
            }
        }
        fclose(file);
    } else {
        return false;
    }

    return true;
}

bool Export_vtk_MatVec3::errorCheck(std::string *errorMessage) {

    if(empty) {
        *errorMessage = "Empty MatVec3";
        return false;
    }

    if(fileName.empty()) {
        *errorMessage = "Blank File Name";
        return false;
    }

    if(!(fileType == 'a' || fileType == 'b')) {
        *errorMessage = "Invalid fileType";
        return false;
    }

    return true;
}


bool Export_vtk_MatVec3::logInput() {

    return true;
}


bool Export_vtk_MatVec3::logOutput() {

    return true;
}




Export_vtk_Workspace::Export_vtk_Workspace(puma::Workspace *work, std::string fileName, char fileType, bool unstructured, int numThreads) {
    this->work = work;
    this->fileName = std::move(fileName);
    this->fileType = fileType;
    this->unstructured = unstructured;
    this->numThreads = numThreads;

    empty = work->matrix.size() == 0;
}


bool Export_vtk_Workspace::execute() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    std::string check = ".vtk";
    if(strcmp(fileName.substr(fileName.length()-4, fileName.length()).c_str(), check.c_str())!=0){
        fileName += check;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed

        std::cout << "Error in Exportvtk: " << errorMessage << std::endl;
        return false;
    }

    std::cout << "Outputting VTK file: " << fileName << " ... " << std::flush;
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

bool Export_vtk_Workspace::exportHelper() {

    char s[256];
    FILE *file;
    int n = fileName.length();
    char fileName_char_array[n + 1];
    strcpy(fileName_char_array, (fileName).c_str());
    file = fopen(fileName_char_array,"w");
    if (file != nullptr) {
        strcpy(s, "# vtk DataFile Version 3.0\n"); fwrite(s, sizeof(char), strlen(s), file);
        strcpy(s, "VTK from PuMA\n"); fwrite(s, sizeof(char), strlen(s), file);
        if(fileType == 'b') {
            if (!unstructured){
                strcpy(s, "BINARY\n"); fwrite(s, sizeof(char), strlen(s), file);
            } else {
                std::cout << "When unstructured == 1, only ASCII is available. Defaulting to ASCII output." << std::endl;
                strcpy(s, "ASCII\n"); fwrite(s, sizeof(char), strlen(s), file);
            }
        } else {
            strcpy(s, "ASCII\n"); fwrite(s, sizeof(char), strlen(s), file);
        }
        if (!unstructured) {
            strcpy(s, "DATASET STRUCTURED_POINTS\n"); fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "DIMENSIONS %d %d %d\n", (int)work->X()+1, (int)work->Y()+1, (int)work->Z()+1); fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "SPACING %d %d %d\n", 1, 1, 1); fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "ORIGIN %d %d %d\n", 0, 0, 0); fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "CELL_DATA %d\n", (int)work->size()); fwrite(s, sizeof(char), strlen(s), file);
            strcpy(s, "SCALARS PuMA_Workspace short\nLOOKUP_TABLE default\n"); fwrite(s, sizeof(char), strlen(s), file);
            if(fileType == 'b') {
                for (long k=0; k<work->Z(); k++) {
                    for (long j=0; j<work->Y(); j++) {
                        for (long i=0; i<work->X(); i++) {
                            auto temp = work->at(i,j,k);
                            SwapEnd(temp);
                            fwrite(&temp, sizeof(short), 1, file);
                        }
                    }
                }
            } else {
                for (long k=0; k<work->Z(); k++) {
                    for (long j=0; j<work->Y(); j++) {
                        for (long i=0; i<work->X(); i++) {
                            auto temp = std::to_string(work->at(i,j,k)) + ' ';
                            strcpy(s, temp.c_str());
                            fwrite(s, sizeof(char), strlen(s), file);
                        }
                    }
                }
            }

        } else {
            strcpy(s, "DATASET UNSTRUCTURED_GRID\n");
            fwrite(s, sizeof(char), strlen(s), file);
            sprintf(s, "POINTS %ld float\n", (work->X()+1)*(work->Y()+1)*(work->Z()+1));
            fwrite(s, sizeof(char), strlen(s), file);
            puma::Matrix<long> pointsID(work->X()+1, work->Y()+1, work->Z()+1);
            int counter = 0;
            for (long k = 0; k < work->Z() + 1; k++) {
                for (long j = 0; j < work->Y() + 1; j++) {
                    for (long i = 0; i < work->X() + 1; i++) {
                        sprintf(s, "%ld %ld %ld ", i, j, k);
                        fwrite(s, sizeof(char), strlen(s), file);
                        pointsID.at(i, j, k) = counter;
                        counter++;
                    }
                }
            }
            sprintf(s, "\n\nCELLS %ld %ld \n", work->size(), work->size() * 9);
            fwrite(s, sizeof(char), strlen(s), file);
            for (long k = 0; k < work->Z(); k++) {
                for (long j = 0; j < work->Y(); j++) {
                    for (long i = 0; i < work->X(); i++) {
                        sprintf(s, "8 %ld %ld %ld %ld %ld %ld %ld %ld \n",
                                pointsID.at(i, j, k), pointsID.at(i + 1, j, k), pointsID.at(i + 1, j + 1, k),
                                pointsID.at(i, j + 1, k),
                                pointsID.at(i, j, k + 1), pointsID.at(i + 1, j, k + 1),
                                pointsID.at(i + 1, j + 1, k + 1), pointsID.at(i, j + 1, k + 1));
                        fwrite(s, sizeof(char), strlen(s), file);
                    }
                }
            }
            sprintf(s, "\nCELL_TYPES %ld \n", work->size());
            fwrite(s, sizeof(char), strlen(s), file);
            strcpy(s, "12 ");
            for (long k = 0; k < work->Z(); k++) {
                for (long j = 0; j < work->Y(); j++) {
                    for (long i = 0; i < work->X(); i++) {
                        fwrite(s, sizeof(char), strlen(s), file);
                    }
                }
            }
            sprintf(s, "\nCELL_DATA %ld\n", work->size());
            fwrite(s, sizeof(char), strlen(s), file);
            strcpy(s, "SCALARS PuMA_Workspace short\nLOOKUP_TABLE default\n");
            fwrite(s, sizeof(char), strlen(s), file);
            for (long k = 0; k < work->Z(); k++) {
                for (long j = 0; j < work->Y(); j++) {
                    for (long i = 0; i < work->X(); i++) {
                        auto temp = std::to_string(work->at(i, j, k)) + ' ';
                        strcpy(s, temp.c_str());
                        fwrite(s, sizeof(char), strlen(s), file);
                    }
                }
            }
        }
        fclose(file);
    } else {
        return false;
    }

    return true;
}


bool Export_vtk_Workspace::errorCheck(std::string *errorMessage) {
    if(empty) {
        *errorMessage = "Empty Workspace";
        return false;
    }

    if(fileName.empty()) {
        *errorMessage = "Blank File Name";
        return false;
    }

    if(!(fileType == 'a' || fileType == 'b')) {
        *errorMessage = "Invalid fileType";
        return false;
    }

    return true;
}

bool Export_vtk_Workspace::logInput() {

    work->log->appendLogSection("Export vtk");
    work->log->appendLogLine(" -- Inputs:");
    work->log->appendLogItem("File Name: ");
    work->log->appendLogItem(fileName);
    work->log->newLine();
    work->log->appendLogItem("Domain Size: ");
    work->log->appendLogItem(work->matrix.X());
    work->log->appendLogItem(", ");
    work->log->appendLogItem(work->matrix.Y());
    work->log->appendLogItem(", ");
    work->log->appendLogItem(work->matrix.Z());
    work->log->newLine();
    work->log->appendLogItem("Header: ");
    work->log->newLine();
    work->log->writeLog();
    return true;
}

bool Export_vtk_Workspace::logOutput() {

    work->log->appendLogItem("Successful Export");
    work->log->newLine();
    work->log->writeLog();

    return true;
}