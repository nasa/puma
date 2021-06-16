#ifndef RAYCASTING_H
#define RAYCASTING_H

#include "materialproperty.h"
#include "orientation.h"
#include "vector.h"
#include "pstring.h"

#include <algorithm>


namespace puma {

bool compute_orientationRC(puma::Workspace *grayWS, puma::Cutoff voidRange, int initAccuracy, int degreeAccuracy, puma::MatVec3<double> *direction, bool print = true, int numThreads = 0);
bool compute_orientationRC(puma::Workspace *grayWS, puma::Cutoff voidRange, int degreeAccuracy, puma::MatVec3<double> *direction, bool print = true, int numThreads = 0);

}


typedef struct {
    puma::Vec3<double> start;
    puma::Vec3<double> pos;
    puma::Vec3<int> vox;
    puma::Vec3<double> dir;
    std::vector< puma::Vec3<double> > positions; // positions at each intersection
    bool intersection;
    double distance;
    double invDistance;
    int fiberValue;
} sphericalWalker;



class RayCasting : public MaterialProperty
{
public:
    RayCasting(puma::Workspace *segWS, int initAccuracy, int degreeAccuracy, puma::MatVec3<double> *direction, bool print, int numThreads);
    bool computeLongestDirection();

private:
    bool directionSimulation();

    void assignWalkersDirection();
    void assignRefinedDirection();
    bool resetSphericalWalkers(const puma::Vec3<int>& vox, bool isDir, int ind);
    bool setSphericalWalkersRefined(const puma::Vec3<int>& vox, int ind);
    bool resetSphericalWalkersRefined(const puma::Vec3<int>& vox, bool isDir, int ind);

    void invertParticles(int ind);

    void executeSphericalWalks(bool isDir, int ind);
    std::vector<double> nextEdgeIntersection(int i, int ind);

    double grayAt(const puma::Vec3<int>& vox);

    puma::Vec3<double> determineDirection(int ind);

    puma::Workspace *segWS;
    puma::MatVec3<double> *direction;
    int nParticles{};
    int numThreads;
    int X,Y,Z;
    bool print;

    int degreeAccuracy;
    int initAccuracy;
    std::vector< std::vector< sphericalWalker > > sphericalWalkers;


    bool logInput() override;
    bool logOutput() override;
    bool errorCheck(std::string *errorMessage) override;

};


#endif // RAYCASTING_H
