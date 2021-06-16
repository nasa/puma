#ifndef EXPORT_vtk_H
#define EXPORT_vtk_H

#include "export.h"
#include "workspace.h"
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

/** @brief Exports a puma::Matrix to a vtk file, a common visualization file (works in Paraview)
 *
 *  @param matrix Pointer to a puma::Matrix to be exported,
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myMatrix.vtk
 *  @param fileType char specifying if output should be binary, 'b', or ASCII, 'a'. Defaulted to binary.
 *  @param unstructured boolean specifying if we want an unstructured grid (for export to Code Aster), Defaulted to 0, meaning structured points
 *  @param numThreads Integer value, number of threads to be used in calculation. If 0 is put, it defaults to the max number of processors
 *  @return bool True if output was successful, False if an error occured.
 */
bool export_vtk(puma::Matrix<double> *matrix, const std::string& fileName, char fileType = 'b', bool unstructured = false, int numThreads = 0);
bool export_vtk(puma::Matrix<float> *matrix, const std::string& fileName, char fileType = 'b', bool unstructured = false, int numThreads = 0);
bool export_vtk(puma::Matrix<short> *matrix, const std::string& fileName, char fileType = 'b', bool unstructured = false, int numThreads = 0);
bool export_vtk(puma::Matrix<int> *matrix, const std::string& fileName, char fileType = 'b', bool unstructured = false, int numThreads = 0);
bool export_vtk(puma::Matrix<long> *matrix, const std::string& fileName, char fileType = 'b', bool unstructured = false, int numThreads = 0);

/** @brief Exports a puma::Matrix of vector values to a vtk file, a common visualization file (works in Paraview)
 *
 *  @param matrix Pointer to a puma::Matrix to be exported,
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myMatrix.vtk
 *  @param fileType char specifying if output should be binary, 'b', or ASCII, 'a'. Defaulted to binary.
 *  @param unstructured boolean specifying if we want an unstructured grid (for export to Code Aster), Defaulted to 0, meaning structured points
 *  @param numThreads Integer value, number of threads to be used in calculation. If 0 is put, it defaults to the max number of processors
 *  @return bool True if output was successful, False if an error occured.
 */
bool export_vtk(puma::MatVec3<double> *matrix, std::string fileName, char fileType = 'b', bool unstructured = false, int numThreads = 0);

/** @brief Exports a puma::Workspace to a vtk file, a common visualization file (works in Paraview)
 *
 *  @param work Pointer to a puma::Workspace to be exported,
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myWorkspace.vtk
 *  @param fileType char specifying if output should be binary, 'b', or ASCII, 'a'. Defaulted to binary.
 *  @param unstructured boolean specifying if we want an unstructured grid (for export to Code Aster), Defaulted to 0, meaning structured points
 *  @param numThreads Integer value, number of threads to be used in calculation. If 0 is put, it defaults to the max number of processors
 *  @return bool True if output was successful, False if an error occured.
 */
bool export_vtk(Workspace *work, std::string fileName, char fileType = 'b', bool unstructured = false, int numThreads = 0);

}

template<class T>
class Export_vtk_Matrix :public Export
{

public:

    Export_vtk_Matrix(puma::Matrix<T> *matrix, const std::string& fileName, char fileType, bool unstructured, int numThreads);

    bool execute() override;

private:

    puma::Matrix<T> *matrix;
    int numThreads{};
    std::string fileName;
    char fileType{};
    bool unstructured{};
    bool empty;

    bool exportHelper();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};


class Export_vtk_MatVec3 :public Export
{

public:

    Export_vtk_MatVec3(puma::MatVec3<double> *matrix, std::string fileName, char fileType, bool unstructured, int numThreads);

    bool execute() override;

private:

    puma::MatVec3<double> *matrix;
    int numThreads;
    std::string fileName;
    char fileType;
    bool unstructured;
    bool empty;

    bool exportHelper();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};


class Export_vtk_Workspace :public Export
{

public:

    Export_vtk_Workspace(puma::Workspace *work, std::string fileName, char fileType, bool unstructured, int numThreads);

    bool execute() override;

private:

    puma::Workspace *work;
    int numThreads;
    std::string fileName;
    char fileType;
    bool unstructured;
    bool empty;

    bool exportHelper();

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};


#endif // EXPORT_vtk_H
