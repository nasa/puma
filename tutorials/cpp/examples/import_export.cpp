#include <iostream>
#include "puma.h"
using namespace std;

int main (int argc, char **argv){
    
    //modify this to run the different import export examples
    string example = "export3Dtiff";
    //options are:
    // 1. export3Dtiff
    // 2. exportbin
    // 3. exportvtk
    // 4. exportstl
    // 5. exporttextfile
    // 6. import3dtiff
    // 7. importbin

    
    if(example == "export3Dtiff"){
        // export3dtiff
        cout << endl << "Exporting a puma matrix to a tiff file:"<< endl;

        puma::Matrix<int> mat(10,10,10, 0);
        mat.set(0,-1, 4,4, 4,4, 150); // -1 specifies until the end of domain

        puma::export_3DTiff(&mat, "out/matrix_example_notnorm", false); // not normalized
        puma::export_3DTiff(&mat, "out/matrix_example_norm", true); // normalized --> 150 becomes 255 since highest value


        cout<< endl << "Exporting a workspace to a tiff file:"<< endl;

        puma::Workspace grayWS(10,10,10,1e-6, false);
        grayWS.matrix.set(0,-1, 4,4, 4,4, 255);

        puma::export_3DTiff(&grayWS, "out/workspace_example_notnorm", false);
    }
    
    else if(example == "exportbin") {
        // exportbin
        cout << endl << "Exporting a puma matrix to a binary file:"<< endl;

        puma::Matrix<int> mat(10,10,10, 0);
        mat.set(0,-1, 4,4, 4,4, 150); // -1 specifies until the end of domain

        puma::export_bin(&mat, "out/mat_example"); // to specify numThreads, add one more int to the inputs at the end


        cout<< endl << "Exporting a workspace to a binary file:"<< endl;

        puma::Workspace grayWS(10,10,10,1e-6, false);
        grayWS.matrix.set(0,-1, 4,4, 4,4, 255);

        puma::export_bin(&grayWS, "out/workspace_example");
    }
    

    else if(example == "exportvtk") {
        
        // exportvtk
        cout << endl << "Exporting a puma matrix to a vtk file:"<< endl;

        puma::Matrix<int> mat(10,10,10, 0);
        mat.set(0,-1, 4,4, 4,4, 150); // -1 specifies until the end of domain

        puma::export_vtk(&mat, "out/mat_example"); // to specify numThreads, add one more int to the inputs at the end
        puma::export_vtk(&mat, "out/mat_example_ASCII", 'a'); // export vtk it in ASCII instead of BINARY
        puma::export_vtk(&mat, "out/mat_example_ASCII_unstructured", 'a', true); // export vtk to an unstructured grid (for import in code Aster)


        cout<< endl << "Exporting a workspace to a vtk file:"<< endl;

        puma::Workspace grayWS(10,10,10,1e-6, false);
        grayWS.matrix.set(0,-1, 4,4, 4,4, 255);

        puma::export_vtk(&grayWS, "out/workspace_example");
        
    }
    
    else if(example == "exportstl") {
        
        // exportstl
        cout << endl << "Running the Marching Cubes as the previous example and exporting the triangles created to an stl file:"<< endl;
        
        puma::Workspace grayWS(1e-6, false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        // 1. Exporting the STL Directly
        puma::export_STL(&grayWS, puma::Cutoff(90,255), false,"out/triangles1");

        // 2. Computing the Triangles separately via the marching cubes algorithm, then exporting the STL
        vector< puma::Triangle<float> > tris;
        puma::isosurface_MarchingCubes(&tris, &grayWS, puma::Cutoff(90,255), true,1,false,0);
        cout << endl << "Number of triangles generated: " << tris.size()<< endl;
        puma::export_STL(&tris,false,"out/triangles2");

        // 3. Segmenting the Workspace, then computing the Triangles separately via the marching cubes algorithm, then exporting the STL
        grayWS.setMaterialID(puma::Cutoff(0,89),0);
        grayWS.setMaterialID(puma::Cutoff(90,255),1);
        puma::isosurface_MarchingCubes(&tris, &grayWS, puma::Cutoff(1,1), true,1,false,0);
        cout << endl << "Number of triangles generated: " << tris.size()<< endl;
        puma::export_STL(&tris,false,"out/triangles3");
        
    }
    
    else if(example == "exporttextfile") {
        // exporttextfile
        cout << endl << "Exporting a puma matrix to a text file:"<< endl;

        puma::Matrix<int> mat(10,10,10, 0);
        mat.set(0,-1, 4,4, 4,4, 150); // -1 specifies until the end of domain

        puma::export_Textfile(&mat, "out/mat_example"); // to specify numThreads, add one more int to the inputs at the end


        cout<< endl << "Exporting a workspace to a text file:"<< endl;

        puma::Workspace grayWS(10,10,10,1e-6, false);
        grayWS.matrix.set(0,-1, 4,4, 4,4, 255);

        puma::export_Textfile(&grayWS, "out/workspace_example");
        
    }
    
    else if(example == "import3dtiff") {
        // import3dtiff
        cout << endl << "Importing a grayscale workspace of FiberForm and printing a subset of it:" << endl;

        std::cout << "-" << puma::path_to_example_file("200_fiberform.tif") << "-" << std::endl;
        
        puma::Workspace grayWS(1e-6, false); // no need to specify the size since the import function takes care of it
        puma::import_3DTiff(&grayWS, puma::path_to_example_file("200_fiberform.tif"),0); // note that .tif is required


        grayWS.matrix.printRange(150,(int)grayWS.X()-1, 150, (int)grayWS.Y()-1, 100,101);


        cout << "Importing the same segmented workspace and printing the same subset of it:" << endl;

        puma::Workspace segWS(1e-6, false); // no need to specify the size since the import function takes care of it
        puma::import_3DTiff(&segWS, puma::path_to_example_file("200_fiberform_segmented.tif"), 0);

        segWS.matrix.printRange(150,(int)segWS.X()-1, 150, (int)segWS.Y()-1, 100,101);
        
        
    }
    
    else if(example == "importbin") {
        // importbin
        cout << endl << "Importing a puma matrix to a binary file:"<< endl;

        // *** Note: Run first ExportBin_Example to generate the data imported in this example ***

        puma::Matrix<int> mat(10,10,10, 0);
        mat.set(0,-1, 4,4, 4,4, 150); // -1 specifies until the end of domain

        puma::import_bin(&mat, "out/mat_example.puma"); // note that .puma is required
        // to specify the number of processors used to read file (i.e. numThreads), add one more int to the inputs at the end

        mat.printRange(0,-1, 0,-1, 4,5);


        cout<< endl << "Importing a workspace to a binary file:"<< endl;

        puma::Workspace grayWS(10,10,10,1e-6, false);
        grayWS.matrix.set(0,-1, 4,4, 4,4, 255);

        puma::import_bin(&grayWS, "out/workspace_example.puma");

        grayWS.matrix.printRange(0,-1, 0,-1, 4,5);
        
    }

    return 0;
}
