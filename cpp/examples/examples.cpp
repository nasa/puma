#include <iostream>
#include "puma.h"

using namespace std;


//_____________________________________________________________________________
// main function for Examples
int main (int argc, char **argv)
//-----------------------------------------------------------------------------
{
    // Get puma path from environment and attach example data folder path
    char* pPath = getenv("PuMA_DIR");
    string puma_dir, example_docs;
    if (pPath!=nullptr) {
        puma_dir = string(pPath);
        example_docs = puma_dir + "/cpp/examples/exampledata/";
    } else {
        cout << "PuMA_DIR path not found in environment. Did you source pumarc?" << endl;
        return 1;
    }

    // To run input one command line argument from the list of codes below
    // e.g. you can enter individual examples by entering "prim1" or the whole category as "prim"
    if (argc <= 1){

            cout << endl << "Enter one of the following codes to run examples:" << endl;

        cout << "Name of Examples                 Code and # of examples to input in command line argument" << endl;
        cout << "all examples                     all" << endl;
        cout << "primitives                       prim1-5" << endl;
        cout << "export                           export1-5" << endl;
        cout << "import                           import1-2" << endl;
        cout << "volumefraction                   volfrac" << endl;
        cout << "isosurface                       isosurf" << endl;
        cout << "surfacearea                      surfarea" << endl;
        cout << "medianfilter3d                   filter1-3" << endl;
        cout << "generaterandomfibers             genfibers1-6" << endl;
        cout << "generaterandomspheres            genspheres1-2" << endl;
        cout << "generatetpms                     gentpms" << endl;
        cout << "porespace                        pore1-2" << endl;
        cout << "meaninterceptlength              mil" << endl;
        cout << "orientation                      orient1-6" << endl;
        cout << "conductivity                     cond1-11" << endl;
        cout << "tortuosity                       tort1-4" << endl;

    } else {

        //// primitives ////

        // primitives: Matrix
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"prim1")) || (!strcmp(argv[1],"prim"))){

            cout << endl << "Creating a matrix with user input size and printing a slice of it:"<< endl;

            puma::Matrix<double> mat(10,10,10, 3.5); // sets all the elements to 3.5
            // N.B. note that if a value is not specified, matrix just allocates the memory, does not set the matrix to zeros

            mat.set(0,-1, 4,4, 4,4, 1); // creating a central solid fiber
            // N.B. -1 specifies until the end of domain mat.X()-1

            mat.printRange(0,-1, 0,-1, 4,5);
        }

        // primitives: Workspace
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"prim2")) || (!strcmp(argv[1],"prim"))){

            cout << endl << "Creating a grayscale workspace:"<< endl;

            puma::Workspace grayWS(10,10,10,1e-6, false); // automatically initializes it to zeros
            // false prevents it from writing the log, unnecessary for these tutorials

            grayWS.matrix.set(0,-1, 4,4, 4,4, 255); // creating a central solid fiber

            grayWS.matrix.printRange(0,-1, 0,-1, 4,5);


            cout << endl << "Segmenting it into a segmented workspace:"<< endl;

            puma::Cutoff cutoff(0,89); // calling puma::Cutoff is equivalent to calling pair<int,int>
            grayWS.setMaterialID(cutoff,0);   // mask the segmented ws with the grayscale one, thresholding by assigning 0 to the range 0-89

            grayWS.setMaterialID(puma::Cutoff(90,255),1); // assigning 1 to the range 90-255, with the direct cutoff pair

            grayWS.matrix.printRange(0,-1, 0,-1, 4,5);
        }

        // primitives: Vec3
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"prim3")) || (!strcmp(argv[1],"prim"))){

            cout << endl << "Creating a vector with three components:"<< endl;

            puma::Vec3<double> vector(60,5,1);
            vector.x = 1; // reassign one of the components

            cout << "Computing magnitude of original vector: " << vector.magnitude() << endl; // compute the magnitude
            cout << "Computing magnitude of normalized vector: " << (vector.norm()).magnitude() << endl; // compute the magnitude

            puma::Vec3<double> vector2(50,0,0);
            vector.dot(vector2);   // dot product
            vector.cross(vector2); // cross product
        }

        // primitives: Vec4
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"prim4")) || (!strcmp(argv[1],"prim"))){

            cout << endl << "Creating a vector with three components and a time value" << endl;

            puma::Vec4<float> vec1(1.2, -3.4, 5.6);
            puma::Vec4<float> vec2(1.2, -3.4, 5.6, 1);
        }

        // primitives: MatVec3
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"prim5"))){

            cout << endl << "Creating a direction matrix for a single fiber running along x:" << endl;

            puma::MatVec3<double> directions(10,10,10); // initializes it to Vec3(0,0,0)
            directions.set(0,-1, 4,4, 4,4, puma::Vec3<double>(1,0,0));

            directions.printRange(0,-1, 0,-1, 4,5);
        }


        //// export3dtiff ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"export1")) || (!strcmp(argv[1],"export"))){

            cout << endl << "Exporting a puma matrix to a tiff file:"<< endl;

            puma::Matrix<int> mat(10,10,10, 0);
            mat.set(0,-1, 4,4, 4,4, 150); // -1 specifies until the end of domain

            puma::export_3DTiff(&mat, example_docs+"matrix_example_notnorm", false); // not normalized
            puma::export_3DTiff(&mat, example_docs+"matrix_example_norm", true); // normalized --> 150 becomes 255 since highest value


            cout<< endl << "Exporting a workspace to a tiff file:"<< endl;

            puma::Workspace grayWS(10,10,10,1e-6, false);
            grayWS.matrix.set(0,-1, 4,4, 4,4, 255);

            puma::export_3DTiff(&grayWS, example_docs+"workspace_example_notnorm", false);
        }


        //// exportbin ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"export2")) || (!strcmp(argv[1],"export"))){

            cout << endl << "Exporting a puma matrix to a binary file:"<< endl;

            puma::Matrix<int> mat(10,10,10, 0);
            mat.set(0,-1, 4,4, 4,4, 150); // -1 specifies until the end of domain

            puma::export_bin(&mat, example_docs+"mat_example"); // to specify numThreads, add one more int to the inputs at the end


            cout<< endl << "Exporting a workspace to a binary file:"<< endl;

            puma::Workspace grayWS(10,10,10,1e-6, false);
            grayWS.matrix.set(0,-1, 4,4, 4,4, 255);

            puma::export_bin(&grayWS, example_docs+"workspace_example");
        }


        //// exportvtk ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"export3")) || (!strcmp(argv[1],"export"))){

            cout << endl << "Exporting a puma matrix to a vtk file:"<< endl;

            puma::Matrix<int> mat(10,10,10, 0);
            mat.set(0,-1, 4,4, 4,4, 150); // -1 specifies until the end of domain

            puma::export_vtk(&mat, example_docs+"mat_example"); // to specify numThreads, add one more int to the inputs at the end
            puma::export_vtk(&mat, example_docs+"mat_example_ASCII", 'a'); // export vtk it in ASCII instead of BINARY
            puma::export_vtk(&mat, example_docs+"mat_example_ASCII_unstructured", 'a', true); // export vtk to an unstructured grid (for import in code Aster)


            cout<< endl << "Exporting a workspace to a vtk file:"<< endl;

            puma::Workspace grayWS(10,10,10,1e-6, false);
            grayWS.matrix.set(0,-1, 4,4, 4,4, 255);

            puma::export_vtk(&grayWS, example_docs+"workspace_example");
        }


        //// exportstl ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"export4")) || (!strcmp(argv[1],"export"))){

            cout << endl << "Running the Marching Cubes as the previous example and exporting the triangles created to an stl file:"<< endl;

            puma::Workspace grayWS(1e-6, false);
            puma::import_3DTiff(&grayWS,puma_dir+"/cpp/test/tiff/grayWS/200_FiberForm.tif",0);

            // 1. Exporting the STL Directly
            puma::export_STL(&grayWS, puma::Cutoff(90,255), false,example_docs+"triangles1");

            // 2. Computing the Triangles separately via the marching cubes algorithm, then exporting the STL
            vector< puma::Triangle<float> > tris;
            puma::isosurface_MarchingCubes(&tris, &grayWS, puma::Cutoff(90,255), true,1,false,0);
            cout << endl << "Number of triangles generated: " << tris.size()<< endl;
            puma::export_STL(&tris,false,example_docs+"triangles2");

            // 3. Segmenting the Workspace, then computing the Triangles separately via the marching cubes algorithm, then exporting the STL
            grayWS.setMaterialID(puma::Cutoff(0,89),0);
            grayWS.setMaterialID(puma::Cutoff(90,255),1);
            puma::isosurface_MarchingCubes(&tris, &grayWS, puma::Cutoff(1,1), true,1,false,0);
            cout << endl << "Number of triangles generated: " << tris.size()<< endl;
            puma::export_STL(&tris,false,example_docs+"triangles3");
        }


        //// exporttextfile ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"export5")) || (!strcmp(argv[1],"export"))){

            cout << endl << "Exporting a puma matrix to a text file:"<< endl;

            puma::Matrix<int> mat(10,10,10, 0);
            mat.set(0,-1, 4,4, 4,4, 150); // -1 specifies until the end of domain

            puma::export_Textfile(&mat, example_docs+"mat_example"); // to specify numThreads, add one more int to the inputs at the end


            cout<< endl << "Exporting a workspace to a text file:"<< endl;

            puma::Workspace grayWS(10,10,10,1e-6, false);
            grayWS.matrix.set(0,-1, 4,4, 4,4, 255);

            puma::export_Textfile(&grayWS, example_docs+"workspace_example");
        }


        //// import3dtiff ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"import1")) || (!strcmp(argv[1],"import"))){

            cout << endl << "Importing a grayscale workspace of FiberForm and printing a subset of it:" << endl;

            puma::Workspace grayWS(1e-6, false); // no need to specify the size since the import function takes care of it
            puma::import_3DTiff(&grayWS, puma_dir+"/cpp/test/tiff/grayWS/200_FiberForm.tif",0); // note that .tif is required

            grayWS.matrix.printRange(150,(int)grayWS.X()-1, 150, (int)grayWS.Y()-1, 100,101);


            cout << "Importing the same segmented workspace and printing the same subset of it:" << endl;

            puma::Workspace segWS(1e-6, false); // no need to specify the size since the import function takes care of it
            puma::import_3DTiff(&segWS, puma_dir+"/cpp/test/tiff/segWS/200_FiberForm.tif",0);

            segWS.matrix.printRange(150,(int)segWS.X()-1, 150, (int)segWS.Y()-1, 100,101);
        }


        //// importbin ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"import2")) || (!strcmp(argv[1],"import"))){

            cout << endl << "Importing a puma matrix to a binary file:"<< endl;

            // *** Note: Run first ExportBin_Example to generate the data imported in this example ***

            puma::Matrix<int> mat(10,10,10, 0);
            mat.set(0,-1, 4,4, 4,4, 150); // -1 specifies until the end of domain

            puma::import_bin(&mat, example_docs+"mat_example.puma"); // note that .puma is required
            // to specify the number of processors used to read file (i.e. numThreads), add one more int to the inputs at the end

            mat.printRange(0,-1, 0,-1, 4,5);


            cout<< endl << "Importing a workspace to a binary file:"<< endl;

            puma::Workspace grayWS(10,10,10,1e-6, false);
            grayWS.matrix.set(0,-1, 4,4, 4,4, 255);

            puma::import_bin(&grayWS, example_docs+"workspace_example.puma");

            grayWS.matrix.printRange(0,-1, 0,-1, 4,5);
        }


        //// volumefraction ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"volfrac"))){

            cout << endl << "Computing Porosity and Volume Fraction of a segmented Fiberform sample:"<< endl;

            puma::Workspace workspace(1.3, false);
            puma::import_3DTiff(&workspace, puma_dir+"/cpp/test/tiff/segWS/200_FiberForm.tif",0);

            cout << endl << "Porosity: " << puma::compute_VolumeFraction(&workspace,0,0) << endl;
            cout << "Fiber Volume Fraction: " << puma::compute_VolumeFraction(&workspace,1,0) << endl;

        }


        //// isosurface ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"isosurf"))){

            cout << endl << "Running the Marching Cubes on a grayscale FiberForm sample, with closed edges and factor of 2:"<< endl;

            puma::Workspace grayWS(1e-6, false);
            puma::import_3DTiff(&grayWS,puma_dir+"/cpp/test/tiff/grayWS/200_FiberForm.tif",0);

            // 1. Computing the Triangles via the Marching Cubes Algorithm. Faster, but there can be small holes in the mesh. Good for visualizations, bad for simulations
            vector< puma::Triangle<float> > tris;
            puma::isosurface_MarchingCubes(&tris, &grayWS,puma::Cutoff(90,255), true,1,false,0);
            cout << endl << "Number of triangles generated - Maching Cubes Regular: " << tris.size()<< endl;


            // 2. Computing the Triangles via the Lewiner Marching Cubes Algorithm. Slower, more triangles, but gaurantees topologically correct surface
            puma::isosurface_LewinerMarchingCubes(&tris, &grayWS,puma::Cutoff(90,255), true,1,false,0);
            cout << endl << "Number of triangles generated - Maching Cubes Lewiner: " << tris.size()<< endl;


            // 3. Segmenting the Workspace, then computing the Triangles separately via the marching cubes algorithm, then exporting the STL
            grayWS.setMaterialID(puma::Cutoff(0,89),0);
            grayWS.setMaterialID(puma::Cutoff(90,255),1);
            puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(90,255),true,1,false,0);
            cout << endl << "Number of triangles generated - Segmented: " << tris.size()<< endl;
        }


        //// surfacearea ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"surfarea"))){

            cout << endl << "Computing the surface area of a sample of FiberForm:"<< endl;

            puma::Workspace grayWS(1e-6, false);
            puma::import_3DTiff(&grayWS,puma_dir+"/cpp/test/tiff/grayWS/200_FiberForm.tif",0);

            pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&grayWS, puma::Cutoff(128, 255), true, 0);

            cout << endl << "Raw Surface Area: " << sa.first << " m^2" << endl;
            cout << "Specific Surface Area: " << sa.first << " 1/m" << endl;
        }


        //// medianfilter3d ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"filter1")) || (!strcmp(argv[1],"filter"))){

            cout << endl << "Applying a Median filter to a FiberForm sample and printing a slice of it:"<< endl;

            puma::Workspace grayWS(1e-6, false);

            puma::import_3DTiff(&grayWS, puma_dir+"/cpp/test/tiff/grayWS/100_FiberForm.tif",0);

            cout << "Before Median filter:"<< endl;
            grayWS.matrix.printRange(50,-1, 50, -1, 50,51);

            //in this case, the return should be true since all values are valid
            puma::filter_Median3D(&grayWS,3);

            cout << endl << "After Median filter:"<< endl;
            grayWS.matrix.printRange(50,-1, 50, -1, 50,51);
        }


        //// meanfilter3d ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"filter2")) || (!strcmp(argv[1],"filter"))){

            cout << endl << "Applying a Mean filter to a FiberForm sample and printing a slice of it:"<< endl;

            puma::Workspace grayWS(1e-6, false);

            puma::import_3DTiff(&grayWS, puma_dir+"/cpp/test/tiff/grayWS/100_FiberForm.tif",0);

            cout << "Before Mean filter:"<< endl;
            grayWS.matrix.printRange(50,-1, 50, -1, 50,51);

            //in this case, the return should be true since all values are valid
            puma::filter_Mean3D(&grayWS,3);

            cout << endl << "After Mean filter:"<< endl;
            grayWS.matrix.printRange(50,-1, 50, -1, 50,51);
        }


        //// bilateralfilter ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"filter3")) || (!strcmp(argv[1],"filter"))){

            cout << endl << "Applying a Bilateral filter to a FiberForm sample and printing a slice of it:"<< endl;

            puma::Workspace grayWS(1e-6, false);

            puma::import_3DTiff(&grayWS, puma_dir+"/cpp/test/tiff/grayWS/100_FiberForm.tif",0);

            cout << "Before Bilateral filter:"<< endl;
            grayWS.matrix.printRange(50,-1, 50, -1, 50,51);

            //in this case, the return should be true since all values are valid
            puma::filter_Bilateral(&grayWS,4,4,50);

            cout << endl << "After Bilateral filter:"<< endl;
            grayWS.matrix.printRange(50,-1, 50, -1, 50,51);
        }


        //// generaterandomfibers ////

        // generaterandomfibers: straightCircle
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"genfibers1")) || (!strcmp(argv[1],"genfibers"))){

            cout << endl << "Creating a 200^3 domain with non-intersecting random fibers having a circular cross section, slight preference for angle in XY:"<< endl;

            // Creates an empty workspace
            puma::Workspace grayWS(1e-6, false);

            // Preparing inputs for fibers generation
            RandomFibersInput input;
            input.straightCircle(200,200,200,5,0,200,0,90,90,15,false,0.9,100);
            input.print = true; // printing option can be turned off like this, ON as a default

            // Generating fibers
            puma::generateRandomFibers(&grayWS, input);

            // Export to 3D tiff
            puma::export_3DTiff(&grayWS,example_docs+"RandomFibers_straightCircle_example.tiff",false);
        }

        // generaterandomfibers: curvedCircle
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"genfibers2")) || (!strcmp(argv[1],"genfibers"))){

            puma::Workspace grayWS(1e-6, false);

            RandomFibersInput input;
            input.curvedCircle(100,100,100,5,0,50,0,90,90,15,false,0.95,100,150,0,1e-3);

            puma::generateRandomFibers(&grayWS, input);


            puma::Workspace grayWS2(1e-6, false);

            RandomFibersInput input2;
            input2.curvedCircle(100,100,100,5,0,50,0,90,90,15,true,0.9,100,150,0,1e-3);

            puma::generateRandomFibers(&grayWS2, input2);
        }

        // generaterandomfibers: straightFlower
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"genfibers3")) || (!strcmp(argv[1],"genfibers"))){

            puma::Workspace grayWS(1e-6, false);

            RandomFibersInput input;
            input.straightFlower(200,200,200,5,0,200,0,90,90,15,false,0.90,100,4,1,5,2,0);

            puma::generateRandomFibers(&grayWS, input);


            puma::Workspace grayWS2(1e-6, false);

            RandomFibersInput input2;
            input2.straightFlower(200,200,200,5,0,200,0,90,90,15,true,0.90,100,4,1,5,2,0);

            puma::generateRandomFibers(&grayWS2, input2);
        }

        // generaterandomfibers: straightFlower_Hollow
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"genfibers4")) || (!strcmp(argv[1],"genfibers"))){

            puma::Workspace grayWS(1e-6, false);

            RandomFibersInput input;
            input.straightFlower_Hollow(200,200,200,5,0,200,0,90,90,15,false,0.90,100,4,1,5,2,0,1,2.5,0);

            puma::generateRandomFibers(&grayWS, input);


            puma::Workspace grayWS2(1e-6, false);

            RandomFibersInput input2;
            input2.straightFlower_Hollow(200,200,200,5,0,200,0,90,90,15,true,0.90,100,4,1,5,2,0,1,2.5,0);

            puma::generateRandomFibers(&grayWS2, input2);
        }

        // generaterandomfibers: curvedFlower
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"genfibers5")) || (!strcmp(argv[1],"genfibers"))){

            puma::Workspace grayWS(1e-6, false);

            RandomFibersInput input;
            input.curvedFlower(100,100,100,5,0,100,0,90,90,15,false,0.95,100,120,0,1e-2,4,1,5,2,0);

            puma::generateRandomFibers(&grayWS, input);


            puma::Workspace grayWS2(1e-6, false);

            RandomFibersInput input2;
            input2.curvedFlower(100,100,100,5,0,100,0,90,90,15,true,0.95,100,120,0,1e-2,4,1,5,2,0);

            puma::generateRandomFibers(&grayWS2, input2);
        }

        // generaterandomfibers: curvedFlower_Hollow
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"genfibers6")) || (!strcmp(argv[1],"genfibers"))){

            puma::Workspace grayWS(1e-6, false);

            RandomFibersInput input;
            input.curvedFlower_Hollow(100,100,100,5,0,100,0,90,90,15,false,0.95,100,120,0,1e-2,4,1,5,2,0,1,2.5,0);

            puma::generateRandomFibers(&grayWS, input);


            puma::Workspace grayWS2(1e-6, false);

            RandomFibersInput input2;
            input2.curvedFlower_Hollow(100,100,100,5,0,100,0,90,90,15,true,0.95,100,120,0,1e-2,4,1,5,2,0,1,2.5,0);

            puma::generateRandomFibers(&grayWS2, input2);
        }


        //// generaterandomspheres ////

        // generaterandomspheres: non-intersecting large spheres
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"genspheres1")) || (!strcmp(argv[1],"genspheres"))){

            cout << endl << "Creating a 200^3 domain with non-intersecting spheres:"<< endl;

            puma::Timer t;

            // Creates an empty workspace
            puma::Workspace sphereWS(1e-6, false);

            // Preparing inputs for spheres generation
            RandomSpheresInput input(200,200,200,80.0,3.0,false,0.99,100);

            // Generating spheres
            puma::generateRandomSpheres(&sphereWS, input);

            // Printing the elapsed time
            cout << "Time: " << t.elapsed() << endl;

            // Export to 3D tiff
            puma::export_3DTiff(&sphereWS,example_docs+"Generate_RandomSpheres_nonintersecting",false);

            // Creating triangulated isosurface
            vector<puma::Triangle<float> > Triangles;
            puma::isosurface_MarchingCubes(&Triangles,&sphereWS,puma::Cutoff(128,255),true,1,false,40);

            // Exporting to stl file
            Export_STL test(&Triangles,false,example_docs+"Generate_RandomSpheres_nonintersecting");
        }

        // generaterandomspheres: intersecting large spheres
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"genspheres2")) || (!strcmp(argv[1],"genspheres"))){

            cout << endl << "Creating a 200^3 domain with intersecting spheres:"<< endl;

            puma::Workspace sphereWS(1e-6, false);

            RandomSpheresInput input(200,200,200,10.0,3.0,true,0.99,100);
            puma::generateRandomSpheres(&sphereWS, input);

            puma::export_3DTiff(&sphereWS,example_docs+"Generate_RandomSpheres_intersecting.tif",false);

            vector<puma::Triangle<float> > Triangles;
            puma::isosurface_MarchingCubes(&Triangles,&sphereWS,puma::Cutoff(128,255),true,1,false);

            Export_STL test(&Triangles,false,example_docs+"Generate_RandomSpheres_intersecting.stl");
        }

        //// generatetpms ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"gentpms"))){
            
            
            double diameter = 20; // in mm
            double height = 38.1; // in mm
            double p = 4.72;
            double q = 0.5045;
            bool circle = true;
            bool exportSTL = false;
            bool binaryTiff = true;
            bool continuousTiff = true;
            int equationNumber = 0; // equation 0, 1 or 2
            
           std::string outputFolder = example_docs;
            std::string outputLabel = "equation1";
            
            
            int scale = 10; // keep between 5-20
   
            
            int voxelsX = diameter * scale;
            int voxelsY = diameter * scale;
            int voxelsZ = height * scale;
            
            double radius = diameter * scale / 2.0;
            double p_scaled = p / scale;

            puma::Workspace ws(1e-6,false);

            //  triply periodic minimal surface (TPMS)
            TPMSInput input(voxelsX,voxelsY,voxelsZ,p_scaled,p_scaled,q,q, equationNumber);
            bool success = puma::generateTPMS(&ws, input);
            
            /// Comment out this section to keep rectangular
            if(circle){
                for(int i=0;i<ws.X();i++){
                    for(int j=0;j<ws.Y();j++){
                        for(int k=0;k<ws.Z();k++){
                            if(std::sqrt((i-ws.X()/2.0)*(i-ws.X()/2.0) + (j-ws.Y()/2.0)*(j-ws.Y()/2.0)) > radius ){
                                ws.matrix(i,j,k) = 0;
                            }
                        }
                    }
                }
            }

            if (success) {
                if(exportSTL){
                    puma::export_STL(&ws, puma::Cutoff(128, 255), false, outputFolder + outputLabel +"_tpms_test.stl");
                }
                
                if(continuousTiff) {
                    puma::export_3DTiff(&ws, outputFolder + outputLabel + "_tpms_test.tif", false);
                }
                
                if(binaryTiff) {
                    ws.setMaterialID(puma::Cutoff(0,127),0);
                    ws.setMaterialID(puma::Cutoff(128,255),255);
                    puma::export_3DTiff(&ws, outputFolder + outputLabel + "_tpms_test_binary.tif", false);
                }
                
            }
        }


        //// porespace ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"pore1")) || (!strcmp(argv[1],"pore"))){

            puma::Workspace grayWS;

            RandomFibersInput input;
            input.straightFlower_Hollow(200,200,1, 30, 0, 1, 0, 0, 0, 90, true, 0.6, 2, 1, 0, 0, 0, 0, 1, 20, 0);
            generateRandomFibers(&grayWS,input);

            puma::Matrix<long> poreMatrix;
            puma::identifyPoreSpace(&grayWS,&poreMatrix,puma::Cutoff(0,127));

            double average =  poreMatrix.average();

            cout << "Average porespace: " << average << endl;
        }

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"pore2")) || (!strcmp(argv[1],"pore"))) {

            puma::Workspace grayWS;

            RandomFibersInput input;
            input.straightFlower_Hollow(200, 200, 1, 30, 0, 1, 0, 0, 0, 90, true, 0.6, 2, 1, 0, 0, 0, 0, 1, 20, 0);
            generateRandomFibers(&grayWS, input);

            puma::fillClosedPores(&grayWS, puma::Cutoff(0, 127), 255);

            grayWS.setMaterialID(puma::Cutoff(0, 127), 0);
            grayWS.setMaterialID(puma::Cutoff(128, 256), 1);

            double average = grayWS.matrix.average();

            cout << "Average porespace: " << average << endl;
        }


        //// meaninterceptlength ////

        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"mil"))){

            cout << endl << "Finding the mean intercept length of a FiberForm 200^3 sample:"<< endl;

            // Creating empty workspace
            puma::Workspace segWS(1e-6, false);

            // Importing FiberForm 200^3 tiff, selecting only subsection of it
            puma::import_3DTiff(&segWS,puma_dir+"/cpp/test/tiff/segWS/200_FiberForm.tif");

            // Computing the Mean Intercept Length
            puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(0,0));

            cout << "Mean Intercept Length: " << mil.x << ' ' << mil.y << ' ' << mil.z << endl;
        }


        //// orientation ////

        // orientation: Ray Casting on Generated Random Fibers
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"orient1")) || (!strcmp(argv[1],"orient"))){

            cout << endl << "Creating a random fiber domain and computing the Orientation via the ray casting method:"<< endl;

            // Generating random fiber sample (see generateRandomFibers example for step-by-step description)
            int scale = 50;
            puma::Workspace grayWS(1e-5, false);
            puma::MatVec3<double> tangents;
            RandomFibersInput input;
            input.straightCircle(scale,scale,scale,2,0,scale,0,90,90,90,false,0.9,1);
            input.print = false; puma::generateRandomFibers(&grayWS,&tangents,input);

            // Segmenting sample
            puma::Workspace segWS(grayWS.shape(), false);
            segWS.setMaterialID(&grayWS, puma::Cutoff(0,127),0);
            segWS.setMaterialID(&grayWS, puma::Cutoff(128,255),1);

            // Initializing orientation matrix
            puma::MatVec3<double> dirs;

            // Computing orientation using Cay Casting (RC)
            puma::compute_orientationRC(&segWS, puma::Cutoff(1, 1), 15,5, &dirs, false);

            // Initializing orientation error matrix
            puma::Matrix<double> error;

            // Computing orientation error between the estimated orientation (dirs) and the true orientation (tangents)
            pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

            cout << "Mean: " << MeanSD.first << endl;
            cout << "Standard Deviation: " << MeanSD.second << endl;
        }

        // orientation: Artificial Flux on Generated Random Fibers
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"orient3")) || (!strcmp(argv[1],"orient"))){

            cout << endl << "Creating a random fiber domain and computing the Orientation via the artificial flux method:"<< endl;

            // Generating random fiber sample (see generateRandomFibers example for step-by-step description)
            int scale = 50;
            puma::Workspace grayWS(1e-5, false);
            puma::MatVec3<double> tangents;
            RandomFibersInput input;
            input.straightCircle(scale,scale,scale,2,0,scale,0,90,90,90,false,0.9,1);
            input.print = false; puma::generateRandomFibers(&grayWS,&tangents,input);

            puma::Workspace segWS(grayWS.shape(), false);
            segWS.setMaterialID(&grayWS,puma::Cutoff(0,127),0);
            segWS.setMaterialID(&grayWS,puma::Cutoff(128,255),1);

            puma::MatVec3<double> dirs;

            // Computing orientation using Artificial Flux (AF)
            puma::compute_orientationAF(&segWS, &dirs, puma::Cutoff(1, 1), 1e-5, 10000,false);

            puma::Matrix<double> error;
            pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

            cout << "Mean: " << MeanSD.first << endl;
            cout << "Standard Deviation: " << MeanSD.second << endl;
        }

        // orientation: Structure Tensor on Generated Random Fibers
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"orient5")) || (!strcmp(argv[1],"orient"))){

            cout << endl << "Creating a random fiber domain and computing the Orientation via the structure tensor method:"<< endl;

            // Generating random fiber sample (see generateRandomFibers example for step-by-step description)
            int scale = 50;
            puma::Workspace grayWS(1e-5, false);
            puma::MatVec3<double> tangents;
            RandomFibersInput input;
            input.straightCircle(scale,scale,scale,2,0,scale,0,90,90,90,false,0.9,1);
            input.print = false; puma::generateRandomFibers(&grayWS,&tangents,input);

            puma::Workspace segWS(grayWS.shape(), false);
            segWS.setMaterialID(&grayWS, puma::Cutoff(0,127),0);
            segWS.setMaterialID(&grayWS, puma::Cutoff(128,255),1);

            puma::MatVec3<double> dirs;

            // Computing orientation using Structure Tensor (ST)
            puma::compute_orientationST(&segWS, 0.7, 1.1, puma::Cutoff(1, 1), &dirs,false);

            puma::Matrix<double> error;
            pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

            cout << "Mean: " << MeanSD.first << endl;
            cout << "Standard Deviation: " << MeanSD.second << endl;
        }

        //// conductivity ////

        // fvthermalconductivity: Multiple materials in series
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"cond1")) || (!strcmp(argv[1],"cond"))){

            cout << endl << "Computing the thermal conductivity using finite volume considering a locally isotropic sample having several materials in series:"<< endl;

            // Initializing Workspace
            puma::Workspace segWS(100,57,62,0,1e-6, false);

            // Setting the workspace values of the materials in series
            segWS.matrix.set(10,19,0,56,0,61,1);
            segWS.matrix.set(20,29,0,56,0,61,2);
            segWS.matrix.set(30,39,0,56,0,61,3);
            segWS.matrix.set(40,49,0,56,0,61,4);
            segWS.matrix.set(50,59,0,56,0,61,5);
            segWS.matrix.set(60,69,0,56,0,61,6);
            segWS.matrix.set(70,79,0,56,0,61,7);
            segWS.matrix.set(80,89,0,56,0,61,8);
            segWS.matrix.set(90,99,0,56,0,61,9);

            // Initializing Temperature field
            puma::Matrix<double> T;

            // Initializing material conductivity
            map<int, double> matCond;
            matCond[0] = 1;
            matCond[1] = 2;
            matCond[2] = 3;
            matCond[3] = 4;
            matCond[4] = 5;
            matCond[5] = 6;
            matCond[6] = 7;
            matCond[7] = 8;
            matCond[8] = 9;
            matCond[9] = 10;

            // Running simulation
            puma::Vec3<double> k = puma::compute_FVThermalConductivity(&segWS, &T, matCond,"periodic","bicgstab",'z',1e-3,10000,true);

            cout << endl << "Conductivity: " << endl;
            cout << "kxx " << k.x << " kxy " << k.y << " kxz " << k.z << endl;
        }

        // fvthermalconductivity: FiberForm 100^3
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"cond2")) || (!strcmp(argv[1],"cond"))){

            cout << endl << "Computing the thermal conductivity using finite volume considering a locally isotropic FiberForm 100^3 sample:"<< endl;

            // Initializing Workspace
            puma::Workspace ws(1e-6, false);

            // Importing 3D tiff
            puma::import_3DTiff(&ws,puma_dir+"/cpp/test/tiff/grayWS/100_FiberForm.tif");

            // Segmenting workspace by thresholding
            ws.setMaterialID(&ws,puma::Cutoff(0,89),0);
            ws.setMaterialID(&ws,puma::Cutoff(90,255),1);

            // Initializing Temperature field and material conductivity
            puma::Matrix<double> T;
            map<int, double> matCond;
            matCond[0] = 0.0257; // air conductivity
            matCond[1] = 12; // approximation to fiber conductivity

            // Running simulation in three directions
            puma::Vec3<double> kx = puma::compute_FVThermalConductivity(&ws, &T, matCond,"symmetric","cg",'x',1e-3,10000,true);
            puma::Vec3<double> ky = puma::compute_FVThermalConductivity(&ws, &T, matCond,"symmetric","cg",'y',1e-3,10000,true);
            puma::Vec3<double> kz = puma::compute_FVThermalConductivity(&ws, &T, matCond,"symmetric","cg",'z',1e-3,10000,true);

            cout << endl << "Conductivity: " << endl;
            cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << endl;
            cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << endl;
            cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << endl;
        }


        // ejthermalconductivity: Multiple materials in series
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"cond3")) || (!strcmp(argv[1],"cond"))){

            cout << endl << "Computing the thermal conductivity using explicit jump considering a locally isotropic sample having several materials in series:"<< endl;

            // Initializing Workspace
            puma::Workspace segWS(100,57,62,0,1e-6, false);

            // Setting the workspace values of the materials in series
            segWS.matrix.set(10,19,0,56,0,61,1);
            segWS.matrix.set(20,29,0,56,0,61,2);
            segWS.matrix.set(30,39,0,56,0,61,3);
            segWS.matrix.set(40,49,0,56,0,61,4);
            segWS.matrix.set(50,59,0,56,0,61,5);
            segWS.matrix.set(60,69,0,56,0,61,6);
            segWS.matrix.set(70,79,0,56,0,61,7);
            segWS.matrix.set(80,89,0,56,0,61,8);
            segWS.matrix.set(90,99,0,56,0,61,9);

            // Initializing Temperature field
            puma::Matrix<double> T;

            // Initializing material conductivity
            map<int, double> matCond;
            matCond[0] = 1;
            matCond[1] = 2;
            matCond[2] = 3;
            matCond[3] = 4;
            matCond[4] = 5;
            matCond[5] = 6;
            matCond[6] = 7;
            matCond[7] = 8;
            matCond[8] = 9;
            matCond[9] = 10;

            // Running simulation
            puma::Vec3<double> k = compute_EJThermalConductivity(&segWS, &T, matCond,'x',1e-3,10000,true);

            cout << endl << "Conductivity: " << endl;
            cout << "kxx " << k.x << " kxy " << k.y << " kxz " << k.z << endl;
        }

        // ejthermalconductivity: FiberForm 100^3
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"cond4")) || (!strcmp(argv[1],"cond"))){

            cout << endl << "Computing the thermal conductivity using explicit jump considering a locally isotropic FiberForm 100^3 sample:"<< endl;

            // Initializing Workspace
            puma::Workspace ws(1e-6, false);

            // Importing 3D tiff
            puma::import_3DTiff(&ws,puma_dir+"/cpp/test/tiff/grayWS/100_FiberForm.tif");

            // Segmenting workspace by thresholding
            ws.setMaterialID(&ws,puma::Cutoff(0,89),0);
            ws.setMaterialID(&ws,puma::Cutoff(90,255),1);

            // Initializing Temperature field and material conductivity
            puma::Matrix<double> T;
            map<int, double> matCond;
            matCond[0] = 0.0257; // air conductivity
            matCond[1] = 12; // approximation to fiber conductivity

            // Running simulation in three directions
            puma::Vec3<double> kx = compute_EJThermalConductivity(&ws, &T, matCond,'x',1e-3,10000,true);
            puma::Vec3<double> ky = compute_EJThermalConductivity(&ws, &T, matCond,'x',1e-3,10000,true);
            puma::Vec3<double> kz = compute_EJThermalConductivity(&ws, &T, matCond,'x',1e-3,10000,true);

            cout << endl << "Conductivity: " << endl;
            cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << endl;
            cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << endl;
            cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << endl;
        }


        // fvanisotropicthermalconductivity: Multiple materials in series
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"cond5")) || (!strcmp(argv[1],"cond"))){

            cout << endl << "Computing the thermal conductivity using finite volume considering a locally anisotropic sample having several materials in series:"<< endl;

            // Initializing Workspace
            puma::Workspace segWS(100,57,62,0,1e-6, false);

            // Setting the workspace values of the materials in series
            segWS.matrix.set(10,19,0,56,0,61,1);
            segWS.matrix.set(20,29,0,56,0,61,2);
            segWS.matrix.set(30,39,0,56,0,61,3);
            segWS.matrix.set(40,49,0,56,0,61,4);
            segWS.matrix.set(50,59,0,56,0,61,5);
            segWS.matrix.set(60,69,0,56,0,61,6);
            segWS.matrix.set(70,79,0,56,0,61,7);
            segWS.matrix.set(80,89,0,56,0,61,8);
            segWS.matrix.set(90,99,0,56,0,61,9);

            // Initializing Temperature and Flux field
            puma::Matrix<double> T;
            puma::MatVec3<double> q;

            // Initializing material conductivity
            map<int, vector<double>> matCond;
            matCond[0] = {1,1,1,0,0,0};
            matCond[1] = {2,2,2,0,0,0};
            matCond[2] = {3,3,3,0,0,0};
            matCond[3] = {4,4,4,0,0,0};
            matCond[4] = {5,5,5,0,0,0};
            matCond[5] = {6,6,6,0,0,0};
            matCond[6] = {7,7,7,0,0,0};
            matCond[7] = {8,8,8,0,0,0};
            matCond[8] = {9,9,9,0,0,0};
            matCond[9] = {10,10,10,0,0,0};

            // Running simulation
            puma::Vec3<double> k = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'x',1e-2,10000,true);

            cout << endl << "Conductivity: " << endl;
            cout << "kxx " << k.x << " kxy " << k.y << " kxz " << k.z << endl;
        }

        // fvanisotropicthermalconductivity: FiberForm 100^3
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"cond6")) || (!strcmp(argv[1],"cond"))){

            cout << endl << "Computing the thermal conductivity using finite volume considering a locally anisotropic FiberForm 100^3 sample:"<< endl;

            // Initializing Workspace
            puma::Workspace ws(1e-6, false);

            // Importing 3D tiff
            puma::import_3DTiff(&ws,puma_dir+"/cpp/test/tiff/grayWS/100_FiberForm.tif");

            // Computing orientations using Structure Tensor (ST) method
            puma::MatVec3< double> tangents;
            puma::compute_orientationST(&ws, 0.7, 0.7, puma::Cutoff(89,255), &tangents);

            // Initializing Temperature, flux field and material conductivity
            puma::Matrix<double> T;
            puma::MatVec3<double> q;
            map<int, vector<double>> matCond;
            matCond[89] = {0.0257}; // air conductivity
            matCond[255] = {15, 8}; // approximation to fiber conductivity, here considered isotropic as an example

            // Running simulation in three directions
            puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond, &tangents, "mpfa", "symmetric","bicgstab",'x',0.1,10000,true);

            cout << endl << "Conductivity: " << endl;
            cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << endl;
        }

        // fvanisotropicthermalconductivity: homogeneous analytical solution
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"cond7")) || (!strcmp(argv[1],"cond"))){

            cout << endl << "Computing the thermal conductivity of an anisotropic slab with a prescribed boundary temperature:"<< endl;

            // Domain size
            int X = 22, Y = 102, Z = 3;

            // Initializing datastructures
            puma::Workspace segWS(X, Y, Z, 0, 1e-6, false);
            puma::Matrix<double> T;
            puma::MatVec3<double> q;
            std::map<int, std::vector<double>> matCond;

            // Assigning conductivity
            matCond[0] = {1, 1, 1, 0.75, 0.75, 0.75}; // kxx,kyy,kzz,kxy,kxz,kyz

            // Initialize prescribed boundary temperature
            puma::Matrix<double> prescribedBC(2, Y, Z, 0);

            // Assigning point temperature in the middle of the top surface
            for (int k = 0; k < Z; k++) {
                for (int j = Y/2-1; j < Y/2+1; j++) {
                    prescribedBC.at(0, j, k) = 1;
                }
            }

            // Running anisotropic conductivity solver
            puma::Vec3<double> kx = compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "symmetric", &prescribedBC,"bicgstab", 'x', 1e-8, 10000, false);

            // Printing slice of the temperature field
            T.printSlice('z', 2, 12);

        }

        // fvelectricalconductivity: Multiple materials in series
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"cond8")) || (!strcmp(argv[1],"cond"))){

            cout << endl << "Computing the electrical conductivity using finite volume considering a locally isotropic sample having several materials in series:"<< endl;

            // Initializing Workspace
            puma::Workspace segWS(100,57,62,0,1e-6, false);

            // Setting the workspace values of the materials in series
            segWS.matrix.set(10,19,0,56,0,61,1);
            segWS.matrix.set(20,29,0,56,0,61,2);
            segWS.matrix.set(30,39,0,56,0,61,3);
            segWS.matrix.set(40,49,0,56,0,61,4);
            segWS.matrix.set(50,59,0,56,0,61,5);
            segWS.matrix.set(60,69,0,56,0,61,6);
            segWS.matrix.set(70,79,0,56,0,61,7);
            segWS.matrix.set(80,89,0,56,0,61,8);
            segWS.matrix.set(90,99,0,56,0,61,9);

            // Initializing Temperature field
            puma::Matrix<double> T;

            // Initializing material conductivity
            map<int, double> matCond;
            matCond[0] = 1;
            matCond[1] = 2;
            matCond[2] = 3;
            matCond[3] = 4;
            matCond[4] = 5;
            matCond[5] = 6;
            matCond[6] = 7;
            matCond[7] = 8;
            matCond[8] = 9;
            matCond[9] = 10;

            // Running simulation
            puma::Vec3<double> k = puma::compute_FVElectricalConductivity(&segWS, &T, matCond,"periodic","bicgstab",'z',1e-3,10000,true);

            cout << endl << "Conductivity: " << endl;
            cout << "kxx " << k.x << " kxy " << k.y << " kxz " << k.z << endl;
        }

        // fvelectricalconductivity: FiberForm 100^3
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"cond9")) || (!strcmp(argv[1],"cond"))){

            cout << endl << "Computing the electrical conductivity using finite volume considering a locally isotropic FiberForm 100^3 sample:"<< endl;

            // Initializing Workspace
            puma::Workspace ws(1e-6, false);

            // Importing 3D tiff
            puma::import_3DTiff(&ws,puma_dir+"/cpp/test/tiff/grayWS/100_FiberForm.tif");

            // Segmenting workspace by thresholding
            ws.setMaterialID(&ws,puma::Cutoff(0,89),0);
            ws.setMaterialID(&ws,puma::Cutoff(90,255),1);

            // Initializing Temperature field and material conductivity
            puma::Matrix<double> T;
            map<int, double> matCond;
            matCond[0] = 0.0257; // air conductivity
            matCond[1] = 12; // approximation to fiber conductivity

            // Running simulation in three directions
            puma::Vec3<double> kx = puma::compute_FVElectricalConductivity(&ws, &T, matCond,"symmetric","cg",'x',1e-3,10000,true);
            puma::Vec3<double> ky = puma::compute_FVElectricalConductivity(&ws, &T, matCond,"symmetric","cg",'y',1e-3,10000,true);
            puma::Vec3<double> kz = puma::compute_FVElectricalConductivity(&ws, &T, matCond,"symmetric","cg",'z',1e-3,10000,true);

            cout << endl << "Conductivity: " << endl;
            cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << endl;
            cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << endl;
            cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << endl;
        }


        // ejelectricalconductivity: Multiple materials in series
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"cond10")) || (!strcmp(argv[1],"cond"))){

            cout << endl << "Computing the electrical conductivity using explicit jump considering a locally isotropic sample having several materials in series:"<< endl;

            // Initializing Workspace
            puma::Workspace segWS(100,57,62,0,1e-6, false);

            // Setting the workspace values of the materials in series
            segWS.matrix.set(10,19,0,56,0,61,1);
            segWS.matrix.set(20,29,0,56,0,61,2);
            segWS.matrix.set(30,39,0,56,0,61,3);
            segWS.matrix.set(40,49,0,56,0,61,4);
            segWS.matrix.set(50,59,0,56,0,61,5);
            segWS.matrix.set(60,69,0,56,0,61,6);
            segWS.matrix.set(70,79,0,56,0,61,7);
            segWS.matrix.set(80,89,0,56,0,61,8);
            segWS.matrix.set(90,99,0,56,0,61,9);

            // Initializing Temperature field
            puma::Matrix<double> T;

            // Initializing material conductivity
            map<int, double> matCond;
            matCond[0] = 1;
            matCond[1] = 2;
            matCond[2] = 3;
            matCond[3] = 4;
            matCond[4] = 5;
            matCond[5] = 6;
            matCond[6] = 7;
            matCond[7] = 8;
            matCond[8] = 9;
            matCond[9] = 10;

            // Running simulation
            puma::Vec3<double> k = compute_EJElectricalConductivity(&segWS, &T, matCond,'x',1e-3,10000,true);

            cout << endl << "Conductivity: " << endl;
            cout << "kxx " << k.x << " kxy " << k.y << " kxz " << k.z << endl;
        }

        // ejelectricalconductivity: FiberForm 100^3
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"cond11")) || (!strcmp(argv[1],"cond"))){

            cout << endl << "Computing the electrical conductivity using explicit jump considering a locally isotropic FiberForm 100^3 sample:"<< endl;

            // Initializing Workspace
            puma::Workspace ws(1e-6, false);

            // Importing 3D tiff
            puma::import_3DTiff(&ws,puma_dir+"/cpp/test/tiff/grayWS/100_FiberForm.tif");

            // Segmenting workspace by thresholding
            ws.setMaterialID(&ws,puma::Cutoff(0,89),0);
            ws.setMaterialID(&ws,puma::Cutoff(90,255),1);

            // Initializing Temperature field and material conductivity
            puma::Matrix<double> T;
            map<int, double> matCond;
            matCond[0] = 0.0257; // air conductivity
            matCond[1] = 12; // approximation to fiber conductivity

            // Running simulation in three directions
            puma::Vec3<double> kx = compute_EJElectricalConductivity(&ws, &T, matCond,'x',1e-3,10000,true);
            puma::Vec3<double> ky = compute_EJElectricalConductivity(&ws, &T, matCond,'x',1e-3,10000,true);
            puma::Vec3<double> kz = compute_EJElectricalConductivity(&ws, &T, matCond,'x',1e-3,10000,true);

            cout << endl << "Conductivity: " << endl;
            cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << endl;
            cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << endl;
            cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << endl;
        }


        //// tortuosity ////

        // tortuosity: compute_EJTortuosity
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"tort1")) || (!strcmp(argv[1],"tort"))) {

            RandomFibersInput input;
            input.straightCircle(200,200,200,5,0,200,0,90,90,90,true,0.6,1);

            puma::Workspace grayWS;
            generateRandomFibers(&grayWS,input);

            puma::Matrix<double> C;
            puma::Vec3<double> t = compute_EJTortuosity(&grayWS, &C, puma::Cutoff(0,127), 'x', 1e-3, 10000, true, 0);

            cout << "tx: " << t.x << ' ' << t.y << ' ' << t.z << endl;
        }

        // tortuosity: compute_FVTortuosity
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"tort2")) || (!strcmp(argv[1],"tort"))) {

            RandomFibersInput input;
            input.straightCircle(200,200,200,5,0,200,0,90,90,90,true,0.6,1);

            puma::Workspace grayWS;
            generateRandomFibers(&grayWS,input);

            puma::Matrix<double> C;
            puma::Vec3<double> t = compute_FVTortuosity(&grayWS, &C, puma::Cutoff(0,127), "symmetric", "bicgstab", 'x', 1e-3, 10000, true, 0);

            cout << "tx: " << t.x << ' ' << t.y << ' ' << t.z << endl;
        }

        // tortuosity: compute_particle_cuberille_Tortuosity
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"tort3")) || (!strcmp(argv[1],"tort"))) {

            RandomFibersInput input;
            input.straightCircle(200,200,200,5,0,200,0,90,90,90,true,0.6,1);

            puma::Workspace grayWS;
            generateRandomFibers(&grayWS,input);

            puma::TortuosityReturn tortReturn = compute_particle_cuberille_Tortuosity(&grayWS,puma::Cutoff(0,127),20000,20000,500,1,10000,0);

            cout << "Tortuosity: " << tortReturn.tortuosity.toString() << endl;
            cout << "Mean Intercept Length: " << tortReturn.meanInterceptLength << endl;
            cout << "Diffusion Coefficient: " << tortReturn.diffusionCoefficient.toString() << endl;
        }

        // tortuosity: compute_particle_isosurface_Tortuosity
        if((!strcmp(argv[1],"all")) || (!strcmp(argv[1],"tort4")) || (!strcmp(argv[1],"tort"))) {
            char dir = 'x';

            RandomFibersInput input;
            input.straightCircle(200,200,200,5,0,200,0,90,90,90,true,0.6,1);

            puma::Workspace grayWS;
            generateRandomFibers(&grayWS,input);

            puma::TortuosityReturn tortReturn = compute_particle_isosurface_Tortuosity(&grayWS,puma::Cutoff(0,127),20000,20000,500,1,10000,0);

            cout << "Tortuosity: " << tortReturn.tortuosity.toString() << endl;
            cout << "Mean Intercept Length: " << tortReturn.meanInterceptLength << endl;
            cout << "Diffusion Coefficient: " << tortReturn.diffusionCoefficient.toString() << endl;
        }
    }

    return 0 ;
}
