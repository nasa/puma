#ifndef RANDOMFIBERSINPUT_H
#define RANDOMFIBERSINPUT_H


//There are six different types of cylinders:
/*
 * The first word indicates whether the fiber is curved or straight. and the second word indicates the cross section shape
 *
 * 1. Straight circle
 * 2. Curved circle
 * 3. Straight flower
 * 4. Curved flower
 * 5. Straight flower hollow
 * 6. Curved flower hollow
 *
 *
 * The specific parameters for each are defined below. The general usage is as follows:
 *
 * puma::Workspace ws;
 * RandomFibersInput input;
 * input.straightCircle(*Insert parameters*);
 * puma::generateRandomFibers(&ws,input);
 *
 * After which the fibers will be stored in the workspace ws with id 1.
 *
 */

class RandomFibersInput{
public:
    int xSize;
    int ySize;
    int zSize;
    double avgRadius;
    double dRadius;
    double avgLength;
    double dLength;
    bool intersect;
    double poro;
    int randomSeed;

    // 0 = isotropic
    // 1 = transverse isotropic
    // 2 = 1D
    int angleType;
    double angle_variability;
    int var_direction; // 0=x, 1=y, 2=z


    // 0 = straightCirculer
    // 1 = curvedCircular
    // 2 = straightFlower
    // 3 = curvedFlower
    int fiberType;

    //CurvedCircular Paramters
    double avgRadiusOfCurvature;
    double dRadiusOfCurvature;
    double accuracy;

    //StraightFlower Parameters
    double AvgSmallRadius;
    double dSmallRadius;
    int AvgNumSmallFibers;
    int dNumSmallFibers;
    double dPlacement;

    //Hollow parameters
    bool hollow;
    double fractionOfHollowFibers;
    double avgHollowRadius;
    double dHollowRadius;

    bool bindFibers;
    double binderRadius;

    int numThreads;

    bool print;



    RandomFibersInput() {
        this->xSize = -1;
        this->ySize = -1;
        this->zSize = -1;
        this->avgRadius = -1;
        this->avgLength = -1;
        this->dRadius = -1;
        this->dLength = -1;
        this->intersect = -1;
        this->poro = -1;
        this->randomSeed = -1;
        this->fiberType = -1;
        this->avgRadiusOfCurvature = -1;
        this->dRadiusOfCurvature = -1;
        this->accuracy = -1;
        this->AvgSmallRadius = -1;
        this->dSmallRadius = -1;
        this->AvgNumSmallFibers = -1;
        this->dNumSmallFibers = -1;
        this->dPlacement = -1;
        this->hollow = -1;
        this->fractionOfHollowFibers = -1;
        this->avgHollowRadius = -1;
        this->dHollowRadius = -1;
        this->bindFibers = false;
        this->binderRadius = -1;
        this->numThreads = 0;

        this->angleType=-1;
        this->var_direction = -1;
        this->angle_variability=-1;

        this->print = true;
    }


    /*
     * xSize: domain size in x direction. Usually between 100-1000
     * ySize: domain size in y direction. Usually between 100-1000
     * zSize: domain size in z direction. Usually between 100-1000
     * avgRadius: average radius of the cylinders (in voxels). Usually 2-20
     * dRadius: deviation in radius. Cannot be more than avgRadius. Often 0
     * avgLength: average Length of the cylinders (in voxels). Usually 10x-50x the avgRadius, or approx equal to the domain size
     * dLength: deviation in length. Cannot be more than avgLength. Often 0.
     * angle_type: 0 for isotropic fibers, 1 for transverse isotropic, 2 for 1D
     * angle_variability: Only relevant for transverse isotropic fibers, angle variability from the opposite plane
     * var_direction: relevant for 1D and transverse isotropic fibers. Direction of relevance.
     * intersect: true for cylinders allowed to intersect, false of they cannot
     *      note: not allowing intersections significantly slows the generation. Also there will be a porosity limit.
     *            for example, non-intersecting cylinders with porosity <0.8 will often fail to ever generate
     * poro: porosity after generation. between 0 and 1. 0 for a solid block and 1 for fully empty space. Often 0.7-0.9
     * randomSeed: Between 1 and 32000. Seeds the random number generator. Changing random seed (while leaving all other
     *             parameters constant) will result in a different structure each time. Similarly, with a constant seed
     *             and parameters, the structure will be identical each time.
     *
     */



    void straightCircle(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
                        double avgLength, double dLength, int angle_type, double angle_variability, int var_direction,
                        bool intersect, double poro, int randomSeed, int numThreads = 0) {
        this->xSize = xSize;
        this->ySize = ySize;
        this->zSize = zSize;
        this->avgRadius = avgRadius;
        this->avgLength = avgLength;
        this->dRadius = dRadius;
        this->dLength = dLength;
        this->angleType = angle_type;
        this->angle_variability = angle_variability;
        this->var_direction = var_direction;
        this->intersect = intersect;
        this->poro = poro;
        this->randomSeed = randomSeed;
        this->fiberType = 0;
        this->hollow = false;
        this->numThreads = numThreads;

        this->print = true;

    }

//    void straightCircle_isotropic(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
//                           double avgLength, double dLength,
//                           bool intersect, double poro, int randomSeed, int numThreads = 0) {
//        straightCircle(xSize,ySize,zSize,avgRadius,dRadius,avgLength,dLength,0,90,0,intersect,poro,randomSeed,numThreads);
//    }
//    void straightCircle_transverse(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
//                           double avgLength, double dLength, double angle_variability, int var_direction,
//                           bool intersect, double poro, int randomSeed, int numThreads = 0) {
//        straightCircle(xSize,ySize,zSize,avgRadius,dRadius,avgLength,dLength,1,angle_variability,var_direction,intersect,poro,randomSeed,numThreads);
//    }
//    void straightCircle_1D(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
//                           double avgLength, double dLength, int var_direction,
//                           bool intersect, double poro, int randomSeed, int numThreads = 0) {
//        straightCircle(xSize,ySize,zSize,avgRadius,dRadius,avgLength,dLength,2,90,var_direction,intersect,poro,randomSeed,numThreads);
//    }

    void addBinder(double binderRadius) {
        this->bindFibers = true;
        this->binderRadius = binderRadius;
    }



    /*
     * xSize: domain size in x direction. Usually between 100-1000
     * ySize: domain size in y direction. Usually between 100-1000
     * zSize: domain size in z direction. Usually between 100-1000
     * avgRadius: average radius of the cylinders (in voxels). Usually 2-20
     * dRadius: deviation in radius. Cannot be more than avgRadius. Often 0
     * avgLength: average Length of the cylinders (in voxels). Usually 10x-50x the avgRadius, or approx equal to the domain size
     * dLength: deviation in length. Cannot be more than avgLength. Often 0.
     * angle_type: 0 for isotropic fibers, 1 for transverse isotropic, 2 for 1D
     * angle_variability: Only relevant for transverse isotropic fibers, angle variability from the opposite plane
     * var_direction: relevant for 1D and transverse isotropic fibers. Direction of relevance.
     * intersect: true for cylinders allowed to intersect, false of they cannot
     *      note: not allowing intersections significantly slows the generation. Also there will be a porosity limit.
     *            for example, non-intersecting cylinders with porosity <0.8 will often fail to ever generate
     * poro: porosity after generation. between 0 and 1. 0 for a solid block and 1 for fully empty space. Often 0.7-0.9
     * randomSeed: Between 1 and 32000. Seeds the random number generator. Changing random seed (while leaving all other
     *             parameters constant) will result in a different structure each time. Similarly, with a constant seed
     *             and parameters, the structure will be identical each time.
     * avgRadiusOfCurvature: between 1/2 (avgLength - dLength) and infinity. Small radius of curvature means more curved.
     *                       infinity radius of curvature means straight cylinder
     * dRadiusOfCurvature: deviation in radius of curvature
     * accuracy: Typically 1e-3. Parameter is used for creating the curved fibers
     *
     */
    void curvedCircle(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
                      double avgLength, double dLength, int angle_type, double angle_variability, int var_direction,
                      bool intersect, double poro, int randomSeed,
                      double avgRadiusOfCurvature, double dRadiusOfCurvature, double accuracy, int numThreads = 0) {
        this->xSize = xSize;
        this->ySize = ySize;
        this->zSize = zSize;
        this->avgRadius = avgRadius;
        this->avgLength = avgLength;
        this->dRadius = dRadius;
        this->dLength = dLength;
        this->angleType = angle_type;
        this->angle_variability = angle_variability;
        this->var_direction = var_direction;
        this->intersect = intersect;
        this->poro = poro;
        this->randomSeed = randomSeed;
        this->fiberType = 1;
        this->avgRadiusOfCurvature = avgRadiusOfCurvature;
        this->dRadiusOfCurvature = dRadiusOfCurvature;
        this->accuracy = accuracy;
        this->hollow = false;
        this->numThreads = numThreads;

        this->print = true;

    }


//    void curvedCircle_isotropic(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
//                                double avgLength, double dLength,
//                                bool intersect, double poro, int randomSeed,
//                                double avgRadiusOfCurvature, double dRadiusOfCurvature, double accuracy, int numThreads = 0) {
//        curvedCircle(xSize, ySize,zSize,avgRadius,dRadius,avgLength,dLength, 0, 90,0,intersect, poro, randomSeed,avgRadiusOfCurvature, dRadiusOfCurvature, accuracy, numThreads);
//    }
//    void curvedCircle_transverse(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
//                                 double avgLength, double dLength, double angle_variability, int var_direction,
//                                 bool intersect, double poro, int randomSeed,
//                                 double avgRadiusOfCurvature, double dRadiusOfCurvature, double accuracy, int numThreads = 0) {
//        curvedCircle(xSize, ySize,zSize,avgRadius,dRadius,avgLength,dLength, 1, angle_variability,var_direction,intersect, poro, randomSeed,avgRadiusOfCurvature, dRadiusOfCurvature, accuracy, numThreads);
//    }
//    void curvedCircle_1D(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
//                         double avgLength, double dLength, int var_direction,
//                         bool intersect, double poro, int randomSeed,
//                         double avgRadiusOfCurvature, double dRadiusOfCurvature, double accuracy, int numThreads = 0) {
//        curvedCircle(xSize, ySize,zSize,avgRadius,dRadius,avgLength,dLength, 2, 90,var_direction,intersect, poro, randomSeed,avgRadiusOfCurvature, dRadiusOfCurvature, accuracy, numThreads);
//    }


    /*
     * xSize: domain size in x direction. Usually between 100-1000
     * ySize: domain size in y direction. Usually between 100-1000
     * zSize: domain size in z direction. Usually between 100-1000
     * avgRadius: average radius of the cylinders (in voxels). Usually 2-20
     * dRadius: deviation in radius. Cannot be more than avgRadius. Often 0
     * avgLength: average Length of the cylinders (in voxels). Usually 10x-50x the avgRadius, or approx equal to the domain size
     * dLength: deviation in length. Cannot be more than avgLength. Often 0.
     * angle_type: 0 for isotropic fibers, 1 for transverse isotropic, 2 for 1D
     * angle_variability: Only relevant for transverse isotropic fibers, angle variability from the opposite plane
     * var_direction: relevant for 1D and transverse isotropic fibers. Direction of relevance.
     * intersect: true for cylinders allowed to intersect, false of they cannot
     *      note: not allowing intersections significantly slows the generation. Also there will be a porosity limit.
     *            for example, non-intersecting cylinders with porosity <0.8 will often fail to ever generate
     * poro: porosity after generation. between 0 and 1. 0 for a solid block and 1 for fully empty space. Often 0.7-0.9
     * randomSeed: Between 1 and 32000. Seeds the random number generator. Changing random seed (while leaving all other
     *             parameters constant) will result in a different structure each time. Similarly, with a constant seed
     *             and parameters, the structure will be identical each time.
     * AvgSmallRadius - Radius of the small cylinders used to create the flower pedals. Typically <= avgRadius and >= 2
     * dSmallRadius - deviation in this value. Often 0
     * AvgNumSmallFibers - The number of pedals that are on the flower shape. Often 4-6
     * dNumSmallFibers - +- the number of pedals
     * dPlacement - deviation in the placement (in degrees) on the circle. Usually 0.
     *
     */
    void straightFlower(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
                        double avgLength, double dLength, int angle_type, double angle_variability, int var_direction,
                        bool intersect, double poro, int randomSeed,
                        double AvgSmallRadius, double dSmallRadius, int AvgNumSmallFibers, int dNumSmallFibers, double dPlacement, int numThreads = 0) {
        this->xSize = xSize;
        this->ySize = ySize;
        this->zSize = zSize;
        this->avgRadius = avgRadius;
        this->avgLength = avgLength;
        this->dRadius = dRadius;
        this->dLength = dLength;
        this->angleType = angle_type;
        this->angle_variability = angle_variability;
        this->var_direction = var_direction;
        this->intersect = intersect;
        this->poro = poro;
        this->randomSeed = randomSeed;
        this->AvgSmallRadius = AvgSmallRadius;
        this->dSmallRadius = dSmallRadius;
        this->AvgNumSmallFibers = AvgNumSmallFibers;
        this->dNumSmallFibers = dNumSmallFibers;
        this->dPlacement = dPlacement;
        this->hollow = false;
        this->fiberType = 2;
        this->numThreads = numThreads;

        this->print = true;

    }

//    void straightFlower_isotropic(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
//                                  double avgLength, double dLength,
//                                  bool intersect, double poro, int randomSeed,
//                                  double AvgSmallRadius, double dSmallRadius, int AvgNumSmallFibers, int dNumSmallFibers, double dPlacement, int numThreads = 0) {
//        straightFlower(xSize,ySize,zSize,avgRadius,dRadius,avgLength,dLength,0,90,0,intersect,poro,randomSeed,AvgSmallRadius, dSmallRadius, AvgNumSmallFibers, dNumSmallFibers, dPlacement, numThreads);
//    }
//    void straightFlower_transverse(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
//                                   double avgLength, double dLength, double angle_variability, int var_direction,
//                                   bool intersect, double poro, int randomSeed,
//                                   double AvgSmallRadius, double dSmallRadius, int AvgNumSmallFibers, int dNumSmallFibers, double dPlacement, int numThreads = 0) {
//        straightFlower(xSize,ySize,zSize,avgRadius,dRadius,avgLength,dLength,1,angle_variability,var_direction,intersect,poro,randomSeed,AvgSmallRadius, dSmallRadius, AvgNumSmallFibers, dNumSmallFibers, dPlacement, numThreads);
//    }
//    void straightFlower_1D(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
//                           double avgLength, double dLength,  int var_direction,
//                           bool intersect, double poro, int randomSeed,
//                           double AvgSmallRadius, double dSmallRadius, int AvgNumSmallFibers, int dNumSmallFibers, double dPlacement, int numThreads = 0) {
//        straightFlower(xSize,ySize,zSize,avgRadius,dRadius,avgLength,dLength,2,90,var_direction,intersect,poro,randomSeed,AvgSmallRadius, dSmallRadius, AvgNumSmallFibers, dNumSmallFibers, dPlacement, numThreads);
//    }


    /*
     * xSize: domain size in x direction. Usually between 100-1000
     * ySize: domain size in y direction. Usually between 100-1000
     * zSize: domain size in z direction. Usually between 100-1000
     * avgRadius: average radius of the cylinders (in voxels). Usually 2-20
     * dRadius: deviation in radius. Cannot be more than avgRadius. Often 0
     * avgLength: average Length of the cylinders (in voxels). Usually 10x-50x the avgRadius, or approx equal to the domain size
     * dLength: deviation in length. Cannot be more than avgLength. Often 0.
     * angle_type: 0 for isotropic fibers, 1 for transverse isotropic, 2 for 1D
     * angle_variability: Only relevant for transverse isotropic fibers, angle variability from the opposite plane
     * var_direction: relevant for 1D and transverse isotropic fibers. Direction of relevance.
     * intersect: true for cylinders allowed to intersect, false of they cannot
     *      note: not allowing intersections significantly slows the generation. Also there will be a porosity limit.
     *            for example, non-intersecting cylinders with porosity <0.8 will often fail to ever generate
     * poro: porosity after generation. between 0 and 1. 0 for a solid block and 1 for fully empty space. Often 0.7-0.9
     * randomSeed: Between 1 and 32000. Seeds the random number generator. Changing random seed (while leaving all other
     *             parameters constant) will result in a different structure each time. Similarly, with a constant seed
     *             and parameters, the structure will be identical each time.
     * AvgSmallRadius - Radius of the small cylinders used to create the flower pedals. Typically <= avgRadius and >= 2
     * dSmallRadius - deviation in this value. Often 0
     * AvgNumSmallFibers - The number of pedals that are on the flower shape. Often 4-6
     * dNumSmallFibers - +- the number of pedals
     * dPlacement - deviation in the placement (in degrees) on the circle. Usually 0.
     * fractionOfHollowFibers - fraction of the fibers that are hollow. 0 for none hollow, 1 for all hollow, 0.5 for half hollow
     * avgHollowRadius - avg radius of the hollow fiber. Less than the avgRadius of the center fiber. Usually 2-3ish
     * dHollowRadius- Deviation in the radius of the hollow fiber.
     *
     *
     */
    void straightFlower_Hollow(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
                               double avgLength, double dLength, int angle_type, double angle_variability, int var_direction,
                               bool intersect, double poro, int randomSeed,
                               double AvgSmallRadius, double dSmallRadius, int AvgNumSmallFibers, int dNumSmallFibers, double dPlacement,
                               double fractionOfHollowFibers, double avgHollowRadius, double dHollowRadius, int numThreads = 0) {
        this->xSize = xSize;
        this->ySize = ySize;
        this->zSize = zSize;
        this->avgRadius = avgRadius;
        this->avgLength = avgLength;
        this->dRadius = dRadius;
        this->dLength = dLength;
        this->angleType = angle_type;
        this->angle_variability = angle_variability;
        this->var_direction = var_direction;
        this->intersect = intersect;
        this->poro = poro;
        this->randomSeed = randomSeed;
        this->AvgSmallRadius = AvgSmallRadius;
        this->dSmallRadius = dSmallRadius;
        this->AvgNumSmallFibers = AvgNumSmallFibers;
        this->dNumSmallFibers = dNumSmallFibers;
        this->dPlacement = dPlacement;
        this->hollow = true;
        this->fractionOfHollowFibers = fractionOfHollowFibers;
        this->avgHollowRadius = avgHollowRadius;
        this->dHollowRadius = dHollowRadius;
        this->fiberType = 2;
        this->numThreads = numThreads;

        this->print = true;

    }


    /*
     * xSize: domain size in x direction. Usually between 100-1000
     * ySize: domain size in y direction. Usually between 100-1000
     * zSize: domain size in z direction. Usually between 100-1000
     * avgRadius: average radius of the cylinders (in voxels). Usually 2-20
     * dRadius: deviation in radius. Cannot be more than avgRadius. Often 0
     * avgLength: average Length of the cylinders (in voxels). Usually 10x-50x the avgRadius, or approx equal to the domain size
     * dLength: deviation in length. Cannot be more than avgLength. Often 0.
     * angle_type: 0 for isotropic fibers, 1 for transverse isotropic, 2 for 1D
     * angle_variability: Only relevant for transverse isotropic fibers, angle variability from the opposite plane
     * var_direction: relevant for 1D and transverse isotropic fibers. Direction of relevance.
     * intersect: true for cylinders allowed to intersect, false of they cannot
     *      note: not allowing intersections significantly slows the generation. Also there will be a porosity limit.
     *            for example, non-intersecting cylinders with porosity <0.8 will often fail to ever generate
     * poro: porosity after generation. between 0 and 1. 0 for a solid block and 1 for fully empty space. Often 0.7-0.9
     * randomSeed: Between 1 and 32000. Seeds the random number generator. Changing random seed (while leaving all other
     *             parameters constant) will result in a different structure each time. Similarly, with a constant seed
     *             and parameters, the structure will be identical each time.
     * avgRadiusOfCurvature: between 1/2 (avgLength - dLength) and infinity. Small radius of curvature means more curved.
     *                       infinity radius of curvature means straight cylinder
     * dRadiusOfCurvature: deviation in radius of curvature
     * accuracy: Typically 1e-3. Parameter is used for creating the curved fibers
     * AvgSmallRadius - Radius of the small cylinders used to create the flower pedals. Typically <= avgRadius and >= 2
     * dSmallRadius - deviation in this value. Often 0
     * AvgNumSmallFibers - The number of pedals that are on the flower shape. Often 4-6
     * dNumSmallFibers - +- the number of pedals
     * dPlacement - deviation in the placement (in degrees) on the circle. Usually 0.
     *
     */
    void curvedFlower(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
                      double avgLength, double dLength, int angle_type, double angle_variability, int var_direction,
                      bool intersect, double poro, int randomSeed,
                      double avgRadiusOfCurvature, double dRadiusOfCurvature, double accuracy,
                      double AvgSmallRadius, double dSmallRadius, int AvgNumSmallFibers, int dNumSmallFibers, double dPlacement, int numThreads = 0) {
        this->xSize = xSize;
        this->ySize = ySize;
        this->zSize = zSize;
        this->avgRadius = avgRadius;
        this->avgLength = avgLength;
        this->dRadius = dRadius;
        this->dLength = dLength;
        this->angleType = angle_type;
        this->angle_variability = angle_variability;
        this->var_direction = var_direction;
        this->intersect = intersect;
        this->poro = poro;
        this->randomSeed = randomSeed;
        this->fiberType = 3;
        this->avgRadiusOfCurvature = avgRadiusOfCurvature;
        this->dRadiusOfCurvature = dRadiusOfCurvature;
        this->accuracy = accuracy;
        this->AvgSmallRadius = AvgSmallRadius;
        this->dSmallRadius = dSmallRadius;
        this->AvgNumSmallFibers = AvgNumSmallFibers;
        this->dNumSmallFibers = dNumSmallFibers;
        this->dPlacement = dPlacement;
        this->hollow = false;
        this->numThreads = numThreads;


        this->print = true;

    }

    /*
     * xSize: domain size in x direction. Usually between 100-1000
     * ySize: domain size in y direction. Usually between 100-1000
     * zSize: domain size in z direction. Usually between 100-1000
     * avgRadius: average radius of the cylinders (in voxels). Usually 2-20
     * dRadius: deviation in radius. Cannot be more than avgRadius. Often 0
     * avgLength: average Length of the cylinders (in voxels). Usually 10x-50x the avgRadius, or approx equal to the domain size
     * dLength: deviation in length. Cannot be more than avgLength. Often 0.
     * angle_type: 0 for isotropic fibers, 1 for transverse isotropic, 2 for 1D
     * angle_variability: Only relevant for transverse isotropic fibers, angle variability from the opposite plane
     * var_direction: relevant for 1D and transverse isotropic fibers. Direction of relevance.
     * intersect: true for cylinders allowed to intersect, false of they cannot
     *      note: not allowing intersections significantly slows the generation. Also there will be a porosity limit.
     *            for example, non-intersecting cylinders with porosity <0.8 will often fail to ever generate
     * poro: porosity after generation. between 0 and 1. 0 for a solid block and 1 for fully empty space. Often 0.7-0.9
     * randomSeed: Between 1 and 32000. Seeds the random number generator. Changing random seed (while leaving all other
     *             parameters constant) will result in a different structure each time. Similarly, with a constant seed
     *             and parameters, the structure will be identical each time.
     * avgRadiusOfCurvature: between 1/2 (avgLength - dLength) and infinity. Small radius of curvature means more curved.
     *                       infinity radius of curvature means straight cylinder
     * dRadiusOfCurvature: deviation in radius of curvature
     * accuracy: Typically 1e-3. Parameter is used for creating the curved fibers
     * AvgSmallRadius - Radius of the small cylinders used to create the flower pedals. Typically <= avgRadius and >= 2
     * dSmallRadius - deviation in this value. Often 0
     * AvgNumSmallFibers - The number of pedals that are on the flower shape. Often 4-6
     * dNumSmallFibers - +- the number of pedals
     * dPlacement - deviation in the placement (in degrees) on the circle. Usually 0.
     * fractionOfHollowFibers - fraction of the fibers that are hollow. 0 for none hollow, 1 for all hollow, 0.5 for half hollow
     * avgHollowRadius - avg radius of the hollow fiber. Less than the avgRadius of the center fiber. Usually 2-3ish
     * dHollowRadius- Deviation in the radius of the hollow fiber.
     *
     */
    void curvedFlower_Hollow(int xSize, int ySize, int zSize, double avgRadius, double dRadius,
                             double avgLength, double dLength, int angle_type, double angle_variability, int var_direction,
                             bool intersect, double poro, int randomSeed,
                             double avgRadiusOfCurvature, double dRadiusOfCurvature, double accuracy,
                             double AvgSmallRadius, double dSmallRadius, int AvgNumSmallFibers, int dNumSmallFibers, double dPlacement,
                             double fractionOfHollowFibers, double avgHollowRadius, double dHollowRadius, int numThreads = 0) {
        this->xSize = xSize;
        this->ySize = ySize;
        this->zSize = zSize;
        this->avgRadius = avgRadius;
        this->avgLength = avgLength;
        this->dRadius = dRadius;
        this->dLength = dLength;
        this->angleType = angle_type;
        this->angle_variability = angle_variability;
        this->var_direction = var_direction;
        this->intersect = intersect;
        this->poro = poro;
        this->randomSeed = randomSeed;
        this->fiberType = 3;
        this->avgRadiusOfCurvature = avgRadiusOfCurvature;
        this->dRadiusOfCurvature = dRadiusOfCurvature;
        this->accuracy = accuracy;
        this->AvgSmallRadius = AvgSmallRadius;
        this->dSmallRadius = dSmallRadius;
        this->AvgNumSmallFibers = AvgNumSmallFibers;
        this->dNumSmallFibers = dNumSmallFibers;
        this->dPlacement = dPlacement;
        this->hollow = true;
        this->fractionOfHollowFibers = fractionOfHollowFibers;
        this->avgHollowRadius = avgHollowRadius;
        this->dHollowRadius = dHollowRadius;
        this->numThreads = numThreads;


        this->print = true;

    }


};

#endif // RANDOMFIBERSINPUT_H
