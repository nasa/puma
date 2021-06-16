#ifndef TortuosityUnified_H
#define TortuosityUnified_H

#include "materialproperty.h"
#include "workspace.h"
#include "vector.h"
#include "fv_diffusion.h"
#include "mp_volumefraction.h"
#include "fv_tortuosity.h"
#include "particles_isosurfacetortuosity.h"
#include "meaninterceptlength.h"
#include <map>


namespace puma {

    typedef struct
    {
        puma::Vec3<double> tortuosity;
        puma::Vec3<double> characteristicLength;
        double legacy_meanInterceptLength;  // legacy method
        puma::Vec3<double> legacy_knudsen_tortuosity;  // legacy method
        double porosity;

    } TortuosityUnifiedReturn;


        TortuosityUnifiedReturn compute_TortuosityUnified_particle(puma::Workspace *grayWS, puma::Cutoff cutoff,
                                                                   int numParticles, int randomSeed, double totalLength, bool print, int numThreads = 0);


        TortuosityUnifiedReturn compute_TortuosityUnified(Workspace *grayWS, puma::Matrix<double> *C, puma::Cutoff cutoff,
                                                          const std::string& sideBC, const std::string& solverType, double solverTol,
                                                          int solverMaxIt , int numParticles, int randomSeed, double totalLength, bool print, int numThreads = 0);


    puma::Vec3<double> compute_EffectiveDiffusionCoefficient(const puma::Vec3<double>& tortuosity, puma::Vec3<double> characteristicLength, double porosity, puma::Vec3<double> bulkDiffusionCoefficient, double meanThermalVelocity);
};

#endif // TortuosityUnified_H
