#ifndef STRAIGHTFLOWERFIBER_H
#define STRAIGHTFLOWERFIBER_H

#include "fiber.h"
#include "vector.h"
#include "randomfibersinput.h"
#include "straightcirclefiber.h"


class StraightFlowerFiber : public Fiber
{
public:
    StraightFlowerFiber() = default;

    bool randomParameters(RandomFibersInput& input, sitmo::prng_engine *engine) override;
    double approximateVolume(RandomFibersInput& input) override;

    short getGrayValue(int i, int j, int k) override;
    bool doesFiberIntersect(puma::Workspace *work, short id = -1) override;
    bool addFiberToDomain(puma::Workspace *work, puma::MatVec3<double> *dirMatrix, short id = -1) override;

    puma::Vec3<double> calculateVectorU(puma::Vec3<double> &point_1, puma::Vec3<double> &point_2, double bigRadius);
    static puma::Vec3<double> getRandomPointInSphere(sitmo::prng_engine *engine);
    static puma::Vec3<double> rotateCrossSectionalVector(puma::Vec3<double> Previous_SmallFiber_Vector, double theta, const puma::Vec3<double>& point_1, puma::Vec3<double> point_2);

    sitmo::prng_engine *engine{};
    std::vector<StraightCircleFiber> fibers;
    bool thisFiberIsHollow{};
    //fibers[0] = bigFiber
    //fibers[1] = first smallFiber

};

#endif // STRAIGHTFLOWERFIBER_H
