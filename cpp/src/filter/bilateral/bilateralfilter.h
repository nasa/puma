#ifndef BilateralFilter_H
#define BilateralFilter_H

#include "filter.h"
#include "workspace.h"

#include <cmath>


namespace puma {
/** @brief Executes a Bilateral Filter a puma::Workspace
 *
 *  @param matrix Pointer to a puma::Matrix to be exported,
 *  @param fileName std::string containing the location and filename for export, Ex: /home/jsmith/Desktop/myMatrix.vtk
 *  @param numThreads Integer value, number of threads to be used in calculation. If 0 is put, it defaults to the max number of processors
 *  @param fileType 'b' for Binary, 'a' for ASCII. Typically 'b'. Defaults to 'b' if nothing is provided.
 *  @param precision Integer value for the precision of the floating point numbers. 1-7 allowed. typically 3.
 *  @return bool True if output was successful, False if an error occured.
 */
    bool filter_Bilateral(puma::Workspace *work, int window_radius, double sigma_d, double sigma_r, int numThreads = 0);
}

class BilateralFilter : Filter
{
public:

    BilateralFilter(puma::Workspace *work, int window_radius, double sigma_d, double sigma_r, int numThreads = 0);

    bool execute() override;

private:

    puma::Workspace *work;
    int window_radius;
    double sigma_d;
    double sigma_r;
    int numThreads;

    bool filterHelper();

    short bilateralValue(long x, long y, long z);
    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};


#endif // BilateralFilter_H
