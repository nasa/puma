#ifndef THERMALPARTICLESISOSURFACE_H
#define THERMALPARTICLESISOSURFACE_H

#include "materialproperty.h"
#include "workspace.h"
#include "vector.h"
#include "triangle.h"
#include "prng_engine.h"
#include "mp_volumefraction.h"
#include "timer.h"

#include <vector>
#include <cmath>
#include <cstdint>


typedef struct {
    double xPos;
    double yPos;
    double zPos;
    double lastXCollision;
    double lastYCollision;
    double lastZCollision;
    double xDir;
    double yDir;
    double zDir;
    int xVox;
    int yVox;
    int zVox;
    unsigned int seed;
    double startX;
    double startY;
    double startZ;
    double deltaR;
    double deltaRw;
    double velocity;
    double walkLength;
    double walkTime;
    long long lastTrianglePos;
    long lastTriangleNum;
    bool skip;
    sitmo::prng_engine engine;
    double totalWalked;
    long collisionCount;
    long particleCollisionCount;

    std::vector< puma::Vec3<double> > locations;

} IsoWalker;

typedef struct{
    std::vector< puma::Triangle<float> > triangles;
} Voxel;





namespace puma {


typedef struct
{
    puma::Vec3<double> tortuosity;
    puma::Vec3<double> diffusivity;
    puma::Vec3<double> diffusionCoefficient;
    double meanInterceptLength;

} TortuosityReturn;

TortuosityReturn compute_particle_isosurface_Tortuosity(puma::Workspace *grayWS, puma::Cutoff cutoff, int numParticles, double meanFreePath, double meanVelocity,
                                                          int randomSeed, double totalLength,  int numThreads);

}


class TortuosityParticles_Isosurface : public MaterialProperty
{
public:
    TortuosityParticles_Isosurface(puma::Workspace *grayWS, puma::Cutoff cutoff, int numParticles, double meanFreePath,
                                   double meanVelocity, int randomSeed, double totalLength, int numThreads);
    puma::TortuosityReturn compute();

private:

    //priate member functions
    bool solve();

    bool generateSurfaces();
    bool generateWalkers();

    void computeMeanInterceptLength();
    void computeTotalLength();
    void executeRandomWalks();
    void computeDiffusionCoefficient();
    void computeDiffusivity();

    bool randomWalk(int i, double walkLength);
    bool newDirParticleCollision(int i);
    bool newDirSurfaceCollision(int i, double xNorm, double yNorm, double zNorm);
    double newFreePath(int i);
    std::vector<double> nextEdgeIntersection(int i);
    double interpolatedGrayscale(double x, double y, double z);
    bool segmentTriangle(puma::Vec3<double> *S1, puma::Vec3<double> *S2, puma::Triangle<float> *T , puma::Vec3<double> *Intersect);
    long periodicVoxels(int xVox, int yVox, int zVox, int *reverseX , int *reverseY , int *reverseZ);


    Voxel *voxels;

    double meanFreePath;

    int count;

    int walkerCount;
    double c_temp, t_temp;

    std::vector<double> x_time;
    std::vector<double> y_MSD0;
    std::vector<double> y_MSD1;
    std::vector<double> y_MSD2;

    bool print;

    //private member variables
    std::vector<IsoWalker> walkers;
    puma::Workspace *grayWS;
    int X, Y, Z;
    puma::Cutoff cutoff;
    int numParticles;
    double meanVelocity;
    double Dp;
    double dBar;
    double voxelLength;
    int numThreads;
    int randomSeed;
    double totalLength;
    double porosity;

    puma::TortuosityReturn returnVals;

    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};

#endif // THERMALPARTICLES_H
