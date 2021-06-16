#ifndef THERMALPARTICLES_H
#define THERMALPARTICLES_H

#include "materialproperty.h"
#include "workspace.h"
#include "vector.h"
#include "prng_engine.h"
#include "particles_isosurfacetortuosity.h"
#include "mp_volumefraction.h"

#include <vector>
#include <cmath>


typedef struct {
    double xPos;
    double yPos;
    double zPos;
    double xDir;
    double yDir;
    double zDir;
    int xVox;
    int yVox;
    int zVox;
    double startX;
    double startY;
    double startZ;
    double deltaR;
    double deltaRw;
    double velocity;
    double walkTime;
    bool skip;
    sitmo::prng_engine engine;
    long collisionCount;

} Walker;


namespace puma {

TortuosityReturn compute_particle_cuberille_Tortuosity(puma::Workspace *grayWS, puma::Cutoff cutoff, int numParticles, double meanFreePath, double meanVelocity,
                                                          int randomSeed, double totalLength,  int numThreads);


}


class TortuosityParticles : public MaterialProperty
{
public:
    TortuosityParticles(puma::Workspace *segWS, int voidValue, int numParticles, double meanFreePath, double meanVelocity, int randomSeed, double totalLength, int numThreads);
    puma::TortuosityReturn compute();

private:

    bool generateWalkers();
    bool randomWalk(int i, double walkLength);
    double grayAt_Symmetric(int xVox, int yVox, int zVox);
    // double grayAt_Periodic(int xVox, int yVox, int zVox);
    bool newDirParticleCollision(int i);
    bool newDirSurfaceCollision(int i, char dir);
    std::vector<double> nextEdgeIntersection(int i);
    bool solve();
    void computeMeanInterceptLength();
    void computeTotalLength();
    void executeRandomWalks();
    void computeDiffusionCoefficient();
    void computeDiffusivity();

    puma::TortuosityReturn returnVals;

    double meanFreePath;

    int X, Y, Z;
    int walkerCount{};
    double c_temp{}, t_temp{};

    int count{};

    std::vector<double> x_time;
    std::vector<double> y_MSD0;
    std::vector<double> y_MSD1;
    std::vector<double> y_MSD2;

    //private member variables
    std::vector<Walker> walkers;

    puma::Workspace *segWS;
    int voidValue;

    int numParticles;
    double meanVelocity;
    double Dp{};
    double dBar{};
    int numThreads;
    int randomSeed;
    double totalLength;
    double porosity{};

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};

#endif // THERMALPARTICLES_H
