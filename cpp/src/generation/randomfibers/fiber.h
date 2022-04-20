#ifndef FIBER_H
#define FIBER_H

#include "randomfibersinput.h"
#include "prng_engine.h"
#include "workspace.h"
#include "vector.h"

#include <cmath>


class Fiber {
public:
    Fiber() = default;

    virtual short getGrayValue(int i, int j, int k) = 0;
    virtual bool randomParameters(RandomFibersInput& input, sitmo::prng_engine *engine) = 0;
    virtual double approximateVolume(RandomFibersInput& input) = 0;

    virtual bool doesFiberIntersect(puma::Workspace *work, short id = -1) = 0;
    virtual bool addFiberToDomain(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, short id = -1) = 0;

    static double pointSegment( puma::Vec3<double> &voxel_point, puma::Vec3<double> &point_1, puma::Vec3<double> &point_2);
    static double pointSegment( double px, double py, double pz, double s1x, double s1y, double s1z, double s2x, double s2y, double s2z);
    static double pointSegment_Uncapped( puma::Vec3<double> &voxel_point, puma::Vec3<double> &point_1, puma::Vec3<double> &point_2);

    static puma::Vec3<double> get_end_position(RandomFibersInput *input, sitmo::prng_engine *engine, puma::Vec3<double> startPos, double length);

};

#endif // FIBER_H