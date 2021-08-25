#ifndef IMPORT_bin_H
#define IMPORT_bin_H

#include "importdata.h"
#include "workspace.h"
#include "pstring.h"
#include "vector.h"
#include "matrix.h"
#include "pstring.h"

#include <fstream>
#include <vector>
#include <cstdio>
#include <cstring>


namespace puma {

/*! Imports a binary file (.puma) into the passed Workspace.
 * \param work is a pointer to a Workspace
 * \param fileName is a string that has the path to the tiff: ex: "/home/jsmith/Desktop/myWorkspace.puma"
 * \param numThreads is the number of threads used in the input. 0 defaults to max number of threads
 */
bool import_bin(Workspace *work, const std::string& fileName, int numThreads = 0);

/*! Imports a binary file (.puma) into the passed matrix.
 * \param matrix is a pointer to a matrix
 * \param fileName is a string that has the path to the tiff: ex: "/home/jsmith/Desktop/myMatrix.puma"
 * \param numThreads is the number of threads used in the input. 0 defaults to max number of threads
 */
bool import_bin(Matrix<double> *matrix, const std::string& fileName, int numThreads = 0);

/*! Imports a binary file (.puma) into the passed matrix.
 * \param matrix is a pointer to a matrix
 * \param fileName is a string that has the path to the tiff: ex: "/home/jsmith/Desktop/myMatrix.puma"
 * \param numThreads is the number of threads used in the input. 0 defaults to max number of threads
 */
bool import_bin(Matrix<float> *matrix, const std::string& fileName, int numThreads = 0);

/*! Imports a binary file (.puma) into the passed matrix.
 * \param matrix is a pointer to a matrix
 * \param fileName is a string that has the path to the tiff: ex: "/home/jsmith/Desktop/myMatrix.puma"
 * \param numThreads is the number of threads used in the input. 0 defaults to max number of threads
 */
bool import_bin(Matrix<short> *matrix, const std::string& fileName, int numThreads = 0);

/*! Imports a binary file (.puma) into the passed matrix.
 * \param matrix is a pointer to a matrix
 * \param fileName is a string that has the path to the tiff: ex: "/home/jsmith/Desktop/myMatrix.puma"
 * \param numThreads is the number of threads used in the input. 0 defaults to max number of threads
 */
bool import_bin(Matrix<int> *matrix, const std::string& fileName, int numThreads = 0);

/*! Imports a binary file (.puma) into the passed matrix.
 * \param matrix is a pointer to a matrix
 * \param fileName is a string that has the path to the tiff: ex: "/home/jsmith/Desktop/myMatrix.puma"
 * \param numThreads is the number of threads used in the input. 0 defaults to max number of threads
 */
bool import_bin(puma::MatVec3<double> *matrix, std::string fileName, int numThreads = 0);

}


template<class T>
class Import_bin :public Import
{

public:

    Import_bin(puma::Matrix<T> *matrix, const std::string& fileName, int numThreads);

    bool import() override;

private:

    puma::Matrix<T> *matrix;
    int numThreads{};
    std::string fileName;

    bool empty{};

    bool importHelper();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};


class Import_bin_MatrixVec3 :public Import
{

public:

    Import_bin_MatrixVec3(puma::MatVec3<double> *matrix, std::string fileName, int numThreads);

    bool import() override;

private:

    puma::MatVec3<double> *matrix;
    int numThreads;
    std::string fileName;

    bool empty{};

    bool importHelper();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};




#endif // import_bin_H
