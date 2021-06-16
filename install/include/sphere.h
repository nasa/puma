#ifndef SPHERE_H
#define SPHERE_H

#include "randomspheresinput.h"
#include "prng_engine.h"
#include "workspace.h"
#include "vector.h"

#include <cmath>


class Sphere {
public:
    Sphere() = default;
    Sphere(double& sphere_radius, puma::Vec3<double>& center_point);
    bool setValues(double& sphere_radius, puma::Vec3<double>& center_point);

   // puma::Vec3<double> center_point();
    puma::Vec3<double> center_point;
    double sphere_radius{};

    short getGrayValue(int i, int j, int k);
    bool randomParameters(RandomSpheresInput& input, sitmo::prng_engine *engine);
    double approximateVolume(RandomSpheresInput& input);

    bool doesSphereIntersect(puma::Workspace *sphereWS);
    bool addSphereToDomain(puma::Workspace *sphereWS);

 //   bool addHollowSphereToDomain(TomographyWorkspace *tomo);

    long getXMin();
    long getXMax();
    long getYMin();
    long getYMax();
    long getZMin();
    long getZMax();

    //static double distanceFromCenter( puma::Vec3<double> &voxel_point, puma::Vec3<double> &center_point);
    double distanceFromCenter( puma::Vec3<double> &voxel_point, puma::Vec3<double> &center_point);

};

#endif // SPHERE_H
