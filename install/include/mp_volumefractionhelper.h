#ifndef MP_VOLUMEFRACTIONHELPER_H
#define MP_VOLUMEFRACTIONHELPER_H

#include "matrix.h"

#include <vector>
#include <cmath>


class MP_VolumeFractionHelper
{
public:
    static double volumeFraction(puma::Matrix<short> *gMatrix, puma::Cutoff cutoff, int numThreads); //vf in this range, inclusive
    static double volumeFraction(puma::Matrix<short> *gMatrix, int value, int numThreads); //vf of exactly this one value
    static long count(puma::Matrix<short> *gMatrix, puma::Cutoff cutoff, int numThreads); //count in this range, inclusive

    static double volumeFraction(puma::Matrix<long> *gMatrix, puma::Cutoff cutoff, int numThreads); //vf in this range, inclusive
    static long count(puma::Matrix<long> *gMatrix, puma::Cutoff cutoff, int numThreads); //count in this range, inclusive

};

#endif // MP_VOLUMEFRACTIONHELPER_H
