#ifndef EXPORT_bin_H
#define EXPORT_bin_H

#include "export.h"
#include "workspace.h"
#include "pstring.h"
#include "vector.h"
#include "matrix.h"

#include <fstream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <utility>


namespace puma {

/** @brief Exports a puma::Workspace to a binary file (with extension .puma)
 *
 *  @param work Pointer to a puma::Workspace to be exported,
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myWorkspace.puma
 *  @return bool True if output was successful, False if an error occured.
 */
bool export_bin(Workspace *work, const std::string& fileName);


/** @brief Exports a puma::Matrix<double> to a binary file (with extension .puma)
 *
 *  @param matrix Pointer to a puma::Workspace to be exported,
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myMatrix.puma
 *  @return bool True if output was successful, False if an error occured.
 */
bool export_bin(Matrix<double> *matrix, const std::string& fileName);

/** @brief Exports a puma::Matrix<float> to a binary file (with extension .puma)
 *
 *  @param matrix Pointer to a puma::Workspace to be exported,
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myMatrix.puma
 *  @return bool True if output was successful, False if an error occured.
 */
bool export_bin(Matrix<float> *matrix, const std::string& fileName);

/** @brief Exports a puma::Matrix<short> to a binary file (with extension .puma)
 *
 *  @param matrix Pointer to a puma::Workspace to be exported,
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myMatrix.puma
 *  @return bool True if output was successful, False if an error occured.
 */
bool export_bin(Matrix<short> *matrix, const std::string& fileName);

/** @brief Exports a puma::Matrix<int> to a binary file (with extension .puma)
 *
 *  @param matrix Pointer to a puma::Workspace to be exported,
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myMatrix.puma
 *  @return bool True if output was successful, False if an error occured.
 */
bool export_bin(Matrix<int> *matrix, const std::string& fileName);

/** @brief Exports a puma::Matrix<long> to a binary file (with extension .puma)
 *
 *  @param matrix Pointer to a puma::Workspace to be exported,
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myMatrix.puma
 *  @return bool True if output was successful, False if an error occured.
 */
bool export_bin(Matrix<long> *matrix, const std::string& fileName);


/** @brief Exports a puma::MatVec3<double> to a binary file (with extension .puma). This data structure looks confusing, but it's simply a 3D matrix with a 3D vector at each location. Example: Velocity vector in (x,y,z) at all locations in 3D Matrix
 *
 *  @param matrix Pointer to a puma::Workspace to be exported,
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myMatrix.puma
 *  @return bool True if output was successful, False if an error occured.
 */
bool export_bin(puma::MatVec3<double> *matrix, std::string fileName);

}


template<class T>
class Export_bin :public Export
{

public:

    Export_bin(puma::Matrix<T> *matrix, const std::string& fileName);

    bool execute() override;

private:

    puma::Matrix<T> *matrix;
    std::string fileName;

    bool empty;

    bool exportHelper();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};


class Export_bin_MatrixVec3 :public Export
{

public:

    Export_bin_MatrixVec3(puma::MatVec3<double> *matrix, std::string fileName);

    bool execute() override;

private:

    puma::MatVec3<double> *matrix;
    std::string fileName;

    bool empty;

    bool exportHelper();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};




#endif // EXPORT_bin_H
