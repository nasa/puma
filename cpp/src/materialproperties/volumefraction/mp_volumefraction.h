#ifndef MATERIALPROPERTYVF_H
#define MATERIALPROPERTYVF_H

#include "materialproperty.h"
#include "workspace.h"
#include "mp_volumefractionhelper.h"


namespace puma{
double compute_VolumeFraction(Workspace *work, int value, int numThreads = 0);
double compute_VolumeFraction(Workspace *work, puma::Cutoff cutoff, int numThreads = 0);
}

class MP_VolumeFraction : MaterialProperty
{
public:

    MP_VolumeFraction(puma::Workspace *work, puma::Cutoff cutoff, int numThreads);
    double compute();


private:
    double volumeFraction{};
    int numThreads;

    puma::Workspace *work;
    puma::Cutoff cutoff;

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};


#endif // MATERIALPROPERTYVF_H
