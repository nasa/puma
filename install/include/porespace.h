#ifndef PORESPACE_H
#define PORESPACE_H

#include "generate.h"
#include "workspace.h"
#include "matrix.h"
#include "mp_volumefractionhelper.h"

#include <algorithm>

namespace puma {

    bool identifyPoreSpace(Workspace *ws, puma::Matrix<long> *poreMatrix, puma::Cutoff voidRange, bool print = true, int numThreads = 0);
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
