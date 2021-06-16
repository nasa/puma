#ifndef CURVEDCIRCLEFIBER_H
#define CURVEDCIRCLEFIBER_H

#include "fiber.h"
#include "vector.h"
#include "prng_engine.h"
#include "randomfibersinput.h"


class Curve {
public:
    puma::Vec3<double> p1;
    puma::Vec3<double> p2;
    puma::Vec3<double> center;
    double radiusOfCurvature{};
    puma::Vec3<double> direction_vector;

    Curve() = default;

    Curve(puma::Vec3<double> p1, puma::Vec3<double> p2, puma::Vec3<double> center, double radiusOfCurvature, puma::Vec3<double> direction_vector) {
        this->p1 = p1;
        this->p2 = p2;
        this->center = center;
        this->radiusOfCurvature = radiusOfCurvature;
        this->direction_vector = direction_vector;
    }

};

class CurvedCircleFiber : public Fiber
{
public:
    CurvedCircleFiber() = default;
    CurvedCircleFiber(double& radius, double& length, puma::Vec3<double>& startPos, puma::Vec3<double>& endPos, double& RadiusOfCurvature, puma::Vec3<double>& center_point, double& accuracy, puma::Vec3<double>& direction_vector);

    bool setValues(double& radius, double& length, puma::Vec3<double>& startPos, puma::Vec3<double>& endPos, double& RadiusOfCurvature, puma::Vec3<double>& center_point, double& accuracy, puma::Vec3<double>& direction_vector);

    static puma::Vec3<double> RandomPointInSphere(sitmo::prng_engine *engine);
    static double pointCurve(Curve &c, puma::Vec3<double> &voxel_point, double accuracy, int level);
    static std::pair<Curve,Curve> SplitCurve(Curve &c);
    Curve GenerateRandomCurve(puma::Vec3<double> &point_1, puma::Vec3<double> &point_2, double RadiusOfCurvature);
    static puma::Vec3<double> GetBisectionPoint(puma::Vec3<double> &point_1, puma::Vec3<double> &vector_u, double dist_midpoint_to_center, double radius);
    static puma::Vec3<double> GetCenterPoint(puma::Vec3<double> &point_1, puma::Vec3<double> &vector_u, double dist_midpoint_to_center);
    static double pointSegment_Curved_Fiber( puma::Vec3<double> &voxel_point, puma::Vec3<double> &point_1, puma::Vec3<double> &point_2, bool *outside_normal);

    short getGrayValue(int i, int j, int k);
    bool randomParameters(RandomFibersInput& input, sitmo::prng_engine *engine);
    double approximateVolume(RandomFibersInput& input);

    bool doesFiberIntersect(puma::Workspace *work, short id = -1);
    bool addFiberToDomain(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, short id = -1);

    bool addHollowFiberToDomain(puma::Workspace *work,puma::MatVec3<double> *dirMatrix, short id = -1);

    long getXMin();
    long getXMax();
    long getYMin();
    long getYMax();
    long getZMin();
    long getZMax();

    double curveDistance2{};
    sitmo::prng_engine *engine{};
    Curve curve;
    double radius;
    double length;
    puma::Vec3<double> startPos;
    puma::Vec3<double> endPos;
    double RadiusOfCurvature;
    puma::Vec3<double> center_point;
    double accuracy;
    puma::Vec3<double> direction_vector;

};

#endif // CURVEDCIRCLEFIBER_H
