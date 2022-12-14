#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){
    
    // isosurface
    cout << endl << "Running the Marching Cubes on a grayscale FiberForm sample, with closed edges and factor of 2:"<< endl;

    puma::Workspace grayWS(1e-6, false);
    puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

    // 1. Computing the Triangles via the Marching Cubes Algorithm. Faster, but there can be small holes in the mesh. Good for visualizations, bad for simulations
    vector< puma::Triangle<float> > tris;
    puma::isosurface_MarchingCubes(&tris, &grayWS,puma::Cutoff(90,255), true,1,false,0);
    cout << endl << "Number of triangles generated - Maching Cubes Regular: " << tris.size()<< endl;
    puma::export_STL(&tris,false,"out/triangles_mc.stl");


    // 2. Computing the Triangles via the Lewiner Marching Cubes Algorithm. Slower, more triangles, but gaurantees topologically correct surface
    puma::isosurface_LewinerMarchingCubes(&tris, &grayWS,puma::Cutoff(90,255), true,1,false,0);
    cout << endl << "Number of triangles generated - Maching Cubes Lewiner: " << tris.size()<< endl;
    puma::export_STL(&tris,false,"out/triangles_mc_lewiner.stl");


    // 3. Segmenting the Workspace, then computing the Triangles separately via the marching cubes algorithm, then exporting the STL
    grayWS.setMaterialID(puma::Cutoff(0,89),0);
    grayWS.setMaterialID(puma::Cutoff(90,255),1);
    puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(1,1),true,1,false,0);
    cout << endl << "Number of triangles generated - Segmented: " << tris.size()<< endl;
    puma::export_STL(&tris,false,"out/triangles_mc_segmented.stl");
    
    return 0;
}
