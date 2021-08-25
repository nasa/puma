#ifndef PORESPACE_H
#define PORESPACE_H

#include "generate.h"
#include "workspace.h"
#include "matrix.h"
#include "mp_volumefractionhelper.h"

#include <algorithm>

namespace puma {

    /*! Identify the porespace
    \param ws the input workspace
    \param poreMatrix the output matrix with identified pores: porespace marked as: 0 solid, 1 largest pore (likely open porosity), >1 other pores
    \param voidRange the grayscale range to consider
    \param print whether to print progress, optional
    \param numThreads number of threads to use, optional
    \return true if successful, false if it failing
    */
    bool identifyPoreSpace(Workspace *ws, puma::Matrix<long> *poreMatrix, puma::Cutoff voidRange, bool print = true, int numThreads = 0);

    /*! Identify the porespace and fill closed porosity
    \param ws the output filled workspace with IDs set as: 0 for void, 1 for solid, filledValue for added filler material
    \param voidRange the grayscale range to consider
    \param filledValue value to fill closed porosity with
    \param print whether to print progress, optional
    \param numThreads number of threads to use, optional
    \return true if successful, false if it failing
    */
    bool fillClosedPores(Workspace *ws, puma::Cutoff voidRange, short filledValue, bool print = true, int numThreads = 0);

}


class PoreSpace : public Generate
{
public:

    PoreSpace(puma::Workspace *ws, puma::Matrix<long> *poreMatrix, puma::Cutoff voidRange, bool print, int numThreads);
    bool identify();


private:

    puma::Cutoff voidRange;
    puma::Workspace *ws;
    puma::Matrix<long> *poreMatrix;
    bool print;
    int numThreads;

    bool PoreSpaceHelper();
    long maxNeighbors(long i, long j, long k);


    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};


#endif // GENERATE_TPMS_H
