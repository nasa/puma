#ifndef STRAIGHTCIRCLEFIBER_H
#define STRAIGHTCIRCLEFIBER_H

#include "fiber.h"
#include "vector.h"
#include "randomfibersinput.h"


class StraightCircleFiber : public Fiber
{
public:
    StraightCircleFiber() = default;
    StraightCircleFiber(double& radius, double& length, puma::Vec3<double>& startPos, puma::Vec3<double>& endPos);

    bool setValues(double& radius, double& length, puma::Vec3<double>& startPos, puma::Vec3<double>& endPos);

    short getGrayValue(int i, int j, int k) override;
    short getGrayValue_Uncapped(int i, int j, int k);

    bool randomParameters(RandomFibersInput& input, sitmo::prng_engine *engine) override;
    double approximateVolume(RandomFibersInput& input) override;

    bool doesFiberIntersect(puma::Workspace *work, short id = -1) override;
    bool addFiberToDomain(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, short id = -1) override;

    bool addHollowFiberToDomain(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, short id = -1) ;

    bool addFiberToDomain_Uncapped(puma::Workspace *work, puma::MatVec3<double> *dirMatrix);


    double getAverageValue(puma::Workspace *work);

    long getXMin();
    long getXMax();
    long getYMin();
    long getYMax();
    long getZMin();
    long getZMax();

    double radius{};
    double length{};
    puma::Vec3<double> startPos;
    puma::Vec3<double> endPos;

};

#endif // STRAIGHTCIRCLEFIBER_H
