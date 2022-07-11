#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){
    
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
    
    // generaterandomspheres: non-intersecting large spheres
    cout << endl << "Creating a 400^3 domain with intersecting spheres:"<< endl;

    puma::Timer t;

    // Creates an empty workspace
    puma::Workspace sphereWS(1e-6, false);

    // Preparing inputs for spheres generation
    RandomSpheresInput input(400,400,400,20,3.0,true,0.8,100);

    // Generating spheres
    puma::generateRandomSpheres(&sphereWS, input);

    // Printing the elapsed time
    cout << "Time: " << t.elapsed() << endl;

    // Export to 3D tiff
    puma::export_3DTiff(&sphereWS,"out/Generate_RandomSpheres",false);
    puma::export_STL(&sphereWS, puma::Cutoff(128,255), false,"out/Generate_RandomSpheres.stl");

    
    
    return 0;
}
