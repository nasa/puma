#ifndef CURVEDFLOWERFIBER_H
#define CURVEDFLOWERFIBER_H

#include "fiber.h"
#include "vector.h"
#include "prng_engine.h"
#include "randomfibersinput.h"
#include "curvedcirclefiber.h"


class CurvedFlowerFiber : public Fiber
{
public:
    CurvedFlowerFiber() = default;

    bool randomParameters(RandomFibersInput& input, sitmo::prng_engine *engine) override;
    double approximateVolume(RandomFibersInput& input) override;

    short getGrayValue(int i, int j, int k) override;
    bool doesFiberIntersect(puma::Workspace *work, short id = -1) override;
    bool addFiberToDomain(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, short id = -1) override;

    puma::Vec3<double> calculatePointsOfSecondFiber(puma::Vec3<double> &point_first_fiber, puma::Vec3<double> &center_first_fiber, double bigRadius);
    puma::Vec3<double> getVectorAForRotationAxis(puma::Vec3<double> &startPos_first_fiber, puma::Vec3<double> &endPos_first_fiber, puma::Vec3<double> &center_first_fiber);
    double getNewRadiusOfCurvature(puma::Vec3<double> &startPos_fiber_1, puma::Vec3<double> &startPos_fiber_n, double &RadiusOfCurvature_fiber_1, puma::Vec3<double> &center_fiber_1, double radiusCross_fiber_1);
    puma::Vec3<double> getNewCenterPoint(puma::Vec3<double> &startPos_fiber_n, puma::Vec3<double> &endPos_fiber_n, double &RadiusOfCurvature_fiber_n, puma::Vec3<double> &direction_vector);
    double getNewLength(puma::Vec3<double> &startPos_fiber_n, puma::Vec3<double> &endPos_fiber_n, puma::Vec3<double> &center_fiber_n, double &RadiusOfCurvature_fiber_n);
    puma::Vec3<double> rotateCrossSectionalVectorCurved(puma::Vec3<double> &Previous_SmallFiber_Vector, double theta, puma::Vec3<double> &point_1, puma::Vec3<double> Rotation_Axis);

    sitmo::prng_engine *engine{};
    std::vector<CurvedCircleFiber> fibers;
    bool thisFiberIsHollow{};

};

#endif // CURVEDFLOWERFIBER_H
