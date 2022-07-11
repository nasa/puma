#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){

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

    // generaterandomfibers: straightFlower_Hollow
    puma::Workspace grayWS(1e-6, false);
    RandomFibersInput input;

    // Isotropic
    /*
     * int xSize, int ySize, int zSize, double avgRadius, double dRadius,
                               double avgLength, double dLength, int angle_type, double angle_variability, int var_direction,
                               bool intersect, double poro, int randomSeed,
                               double AvgSmallRadius, double dSmallRadius, int AvgNumSmallFibers, int dNumSmallFibers, double dPlacement,
                               double fractionOfHollowFibers, double avgHollowRadius, double dHollowRadius
     */
    input.straightFlower_Hollow(200,200,200,5,0,200,0,0,0,0,true,0.90,100,4,1,5,2,0,1,2.5,0);
    puma::generateRandomFibers(&grayWS, input);
    puma::export_STL(&grayWS, puma::Cutoff(128,255), false,"out/straight_flower_hollow_isotropic.stl");

    // Transverse isotropic
    /*
 * int xSize, int ySize, int zSize, double avgRadius, double dRadius,
                           double avgLength, double dLength, int angle_type, double angle_variability, int var_direction,
                           bool intersect, double poro, int randomSeed,
                           double AvgSmallRadius, double dSmallRadius, int AvgNumSmallFibers, int dNumSmallFibers, double dPlacement,
                           double fractionOfHollowFibers, double avgHollowRadius, double dHollowRadius
 */
    input.straightFlower_Hollow(200,200,200,5,0,200,0,1,15,2,true,0.90,100,4,1,5,2,0,1,2.5,0);
    puma::generateRandomFibers(&grayWS, input);
    puma::export_STL(&grayWS, puma::Cutoff(128,255), false,"out/straight_flower_hollow_transverse_isotropic.stl");

    // 1D fibers in Z
    /*
 * int xSize, int ySize, int zSize, double avgRadius, double dRadius,
                           double avgLength, double dLength, int angle_type, double angle_variability, int var_direction,
                           bool intersect, double poro, int randomSeed,
                           double AvgSmallRadius, double dSmallRadius, int AvgNumSmallFibers, int dNumSmallFibers, double dPlacement,
                           double fractionOfHollowFibers, double avgHollowRadius, double dHollowRadius
 */
    input.straightFlower_Hollow(200,200,200,5,0,200,0,2,0,2,true,0.90,100,4,1,5,2,0,1,2.5,0);
    puma::generateRandomFibers(&grayWS, input);
    puma::export_STL(&grayWS, puma::Cutoff(128,255), false,"out/straight_flower_hollow_1D.stl");

    
    return 0;
}
