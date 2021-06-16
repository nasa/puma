#ifndef RANDOMSPHERESINPUT_H
#define RANDOMSPHERESINPUT_H


/*
 * xSize: domain size in x direction. Usually between 100-1000
 * ySize: domain size in y direction. Usually between 100-1000
 * zSize: domain size in z direction. Usually between 100-1000
 * avgRadius: average radius of the spheres (in voxels). Usually 2-20
 * dRadius: deviation in radius. Cannot be more than avgRadius. Often 0
 * intersect: true for spheres allowed to intersect, false of they cannot
 *      note: not allowing intersections significantly slows the generation. Also there will be a porosity limit.
 *            for example, non-intersecting spheres with porosity <0.8 will often fail to ever generate
 * poro: porosity after generation. between 0 and 1. 0 for a solid block and 1 for fully empty space. Often 0.7-0.9
 * randomSeed: Between 1 and 32000. Seeds the random number generator. Changing random seed (while leaving all other
 *             parameters constant) will result in a different structure each time. Similarly, with a constant seed
 *             and parameters, the structure will be identical each time.
 *
 */

class RandomSpheresInput{
public:
    int xSize;
    int ySize;
    int zSize;
    double avgRadius;
    double dRadius;
    bool intersect;
    double poro;
    int randomSeed;

    bool print;

    RandomSpheresInput() {
        this->xSize = -1;
        this->ySize = -1;
        this->zSize = -1;
        this->avgRadius = -1;
        this->dRadius = -1;
        this->intersect = -1;
        this->poro = -1;
        this->randomSeed = -1;
        
        this->print = true;
    }

    RandomSpheresInput(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
                      bool intersect, double poro, int randomSeed) {
        this->xSize = xSize;
        this->ySize = ySize;
        this->zSize = zSize;
        this->avgRadius = avgRadius;
        this->dRadius = dRadius;
        this->intersect = intersect;
        this->poro = poro;
        this->randomSeed = randomSeed;

        this->print = true;
    }

    void randomSpheres(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
                       bool intersect, double poro, int randomSeed) {
        this->xSize = xSize;
        this->ySize = ySize;
        this->zSize = zSize;
        this->avgRadius = avgRadius;
        this->dRadius = dRadius;
        this->intersect = intersect;
        this->poro = poro;
        this->randomSeed = randomSeed;

        this->print = true;
    }




};

#endif // RANDOMSPHERESINPUT_H
