#include "import_bin.h"

#include <utility>


bool puma::import_bin(Workspace *work, const std::string& fileName, int numThreads) {
    Import_bin<short> importer(&(work->matrix),fileName,numThreads);
    return importer.import();
}

bool puma::import_bin(Matrix<double> *matrix, const std::string& fileName, int numThreads) {
    Import_bin<double> importer(matrix,fileName,numThreads);
    return importer.import();
}
bool puma::import_bin(Matrix<float> *matrix, const std::string& fileName, int numThreads) {
    Import_bin<float> importer(matrix,fileName,numThreads);
    return importer.import();
}
bool puma::import_bin(Matrix<short> *matrix, const std::string& fileName, int numThreads) {
    Import_bin<short> importer(matrix,fileName,numThreads);
    return importer.import();
}
bool puma::import_bin(Matrix<int> *matrix, const std::string& fileName, int numThreads) {
    Import_bin<int> importer(matrix,fileName,numThreads);
    return importer.import();
}

bool puma::import_bin(puma::MatVec3<double> *matrix, std::string fileName, int numThreads) {
    Import_bin_MatrixVec3 importer(matrix,std::move(fileName),numThreads);
    return importer.import();
}



template<class T> Import_bin<T>::Import_bin(puma::Matrix<T> *matrix, const std::string& fileName, int numThreads) {
    this->matrix = matrix;
    this->fileName = fileName;
    this->numThreads = numThreads;
}


template<class T> bool Import_bin<T>::import() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed

        std::cout << "Error in importbin: " << errorMessage << std::endl;
        return false;
    }

    std::cout << "Reading bin file: " << fileName << " ... " << std::flush;
    bool success = importHelper();

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

template<class T> bool Import_bin<T>::importHelper() {

    FILE *pfile;
    int n = fileName.length();
    char fileName_char_array[n + 1];
    strcpy(fileName_char_array, fileName.c_str());
    pfile = fopen(fileName_char_array,"rb");

    if (pfile!=nullptr) {

        // obtain number of elements
        unsigned long count, lSize;
        fseek (pfile , 0 , SEEK_END);

        lSize = ftell (pfile);
        count = lSize/sizeof(T);
        rewind (pfile);

        // copy the file into the temporary vector:
        T *tempVec;
        tempVec = (T*) malloc (sizeof(char)*lSize);

        size_t result = fread (tempVec, sizeof(T), count, pfile);

        if (result == count) {

            int X = (int)tempVec[count-3];
            int Y = (int)tempVec[count-2];
            int Z = (int)tempVec[count-1];
            matrix->resize(X,Y,Z);

            omp_set_num_threads(numThreads);
#pragma omp parallel for
            for(int k=0;k<Z;k++) {
                for(int j=0;j<Y;j++) {
                    for(int i=0;i<X;i++) {
                        matrix->at(i,j,k) = tempVec[X*Y*k+X*j+i];
                    }
                }
            }

            fclose (pfile);
            free(tempVec);

        } else {
            std::cerr << "Could not read file" << std::endl;
            return false;
        }
    } else {
        std::cerr << "Could not open file" << std::endl;
        return false;
    }

    return true;
}


template<class T> bool Import_bin<T>::errorCheck(std::string *errorMessage) {

    if(fileName.empty()) {
        *errorMessage = "Blank File Name";
        return false;
    }

    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }
    return true;
}

template<class T> bool Import_bin<T>::logInput() {

    return true;
}

template<class T> bool Import_bin<T>::logOutput() {

    return true;
}








Import_bin_MatrixVec3::Import_bin_MatrixVec3(puma::MatVec3<double> *matrix, std::string fileName, int numThreads) {
    this->matrix = matrix;
    this->fileName = std::move(fileName);
    this->numThreads = numThreads;
}


bool Import_bin_MatrixVec3::import() {

    if( !logInput() ) {
        std::cout << "Failed to log Inputs" << std::endl;
        return false;
    }

    std::string errorMessage;
    if( !errorCheck(&errorMessage) ) { //Error Check Failed

        std::cout << "Error in Exportbin: " << errorMessage << std::endl;
        return false;
    }

    std::cout << "Reading bin file: " << fileName << " ... " << std::flush;
    bool success = importHelper();

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

bool Import_bin_MatrixVec3::importHelper() {

    FILE *pfile;
    int n = fileName.length();
    char fileName_char_array[n + 1];
    strcpy(fileName_char_array, fileName.c_str());
    pfile = fopen(fileName_char_array,"rb");

    if (pfile!=nullptr) {

        // obtain number of elements
        unsigned long count, lSize;
        fseek (pfile , 0 , SEEK_END);

        lSize = ftell (pfile);
        count = lSize/sizeof(double);
        rewind (pfile);

        // copy the file into the temporary vector:
        double *tempVec;
        tempVec = (double*) malloc (sizeof(char)*lSize);

        size_t result = fread (tempVec, sizeof(double), count, pfile);

        if (result == count) {

            int X = (int)tempVec[count-3];
            int Y = (int)tempVec[count-2];
            int Z = (int)tempVec[count-1];
            int startIndex = X*Y*Z;
            matrix->resize(X,Y,Z);

            omp_set_num_threads(numThreads);
#pragma omp parallel for
            for(int k=0;k<Z;k++) {
                for(int j=0;j<Y;j++) {
                    for(int i=0;i<X;i++) {
                        matrix->at(i,j,k).x = tempVec[X*Y*k+X*j+i];
                        matrix->at(i,j,k).y = tempVec[startIndex + X*Y*k+X*j+i];
                        matrix->at(i,j,k).z = tempVec[2*startIndex + X*Y*k+X*j+i];
                    }
                }
            }

            fclose (pfile);
            free(tempVec);

        } else {
            std::cerr << "Could not read file" << std::endl;
            return false;
        }
    } else {
        std::cerr << "Could not open file" << std::endl;
        return false;
    }

    return true;
}


bool Import_bin_MatrixVec3::errorCheck(std::string *errorMessage) {

    if(fileName.empty()) {
        *errorMessage = "Blank File Name";
        return false;
    }
    if(numThreads<=0 || numThreads>1000) {
        numThreads = omp_get_num_procs();
    }
    return true;
}

bool Import_bin_MatrixVec3::logInput() {

    return true;
}

bool Import_bin_MatrixVec3::logOutput() {

    return true;
}

