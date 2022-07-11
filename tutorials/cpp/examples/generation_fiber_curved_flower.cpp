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

    // generaterandomfibers: curvedFlower
    puma::Workspace grayWS(1e-6, false);

    // Isotropic
    RandomFibersInput input;
    /*
     * int xSize, int ySize, int zSize, double avgRadius, double dRadius,
                      double avgLength, double dLength, int angle_type, double angle_variability, int var_direction,
                      bool intersect, double poro, int randomSeed,
                      double avgRadiusOfCurvature, double dRadiusOfCurvature, double accuracy,
                      double AvgSmallRadius, double dSmallRadius, int AvgNumSmallFibers, int dNumSmallFibers, double dPlacement
     */
    input.curvedFlower(100,100,100,5,0,100,0,0,0,0,true,0.90,100,120,0,1e-3,4,1,5,2,0);
    puma::generateRandomFibers(&grayWS, input);
    puma::export_STL(&grayWS, puma::Cutoff(128,255), false,"out/curved_flower_isotropic.stl");

    // Transverse isotropic in xy
    /*
 * int xSize, int ySize, int zSize, double avgRadius, double dRadius,
                  double avgLength, double dLength, int angle_type, double angle_variability, int var_direction,
                  bool intersect, double poro, int randomSeed,
                  double avgRadiusOfCurvature, double dRadiusOfCurvature, double accuracy,
                  double AvgSmallRadius, double dSmallRadius, int AvgNumSmallFibers, int dNumSmallFibers, double dPlacement
 */
    input.curvedFlower(100,100,100,5,0,100,0,1,15,2,true,0.90,100,120,0,1e-3,4,1,5,2,0);
    puma::generateRandomFibers(&grayWS, input);
    puma::export_STL(&grayWS, puma::Cutoff(128,255), false,"out/curved_flower_transverse_isotropic.stl");

    // 1D in z
    /*
 * int xSize, int ySize, int zSize, double avgRadius, double dRadius,
                  double avgLength, double dLength, int angle_type, double angle_variability, int var_direction,
                  bool intersect, double poro, int randomSeed,
                  double avgRadiusOfCurvature, double dRadiusOfCurvature, double accuracy,
                  double AvgSmallRadius, double dSmallRadius, int AvgNumSmallFibers, int dNumSmallFibers, double dPlacement
 */
    input.curvedFlower(100,100,100,5,0,50,0,2,0,2,true,0.90,100,120,0,1e-3,4,1,5,2,0);
    puma::generateRandomFibers(&grayWS, input);
    puma::export_STL(&grayWS, puma::Cutoff(128,255), false,"out/curved_flower_1d.stl");
    
    return 0;
}
