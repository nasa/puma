#include "../testframework/subtest.h"
#include "puma.h"

#include <vector>


class ExportSTL_Test : public SubTest {
public:

    ExportSTL_Test() {
        testSuiteName = "Export_STL_Test";

        tests.push_back(test1);
        tests.push_back(test2);
        tests.push_back(test3);
        tests.push_back(test4);
        //tests.push_back(test5);

    }

    static TestResult test1() {

        std::string suiteName = "export_STL_Test";
        std::string testName = "export_STL_Test: Test 1 - Empty Triangles";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::vector< puma::Triangle <float> > Triangles;
        bool success = puma::export_STL(&Triangles,false,"cpp/test/out/stl/test1.stl");

        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {
        std::string suiteName = "export_STL_Test";
        std::string testName = "export_STL_Test: Test 2 - Blank FileName";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 2, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0,50,0,50,0,50,0);

        std::vector< puma::Triangle <float> > Triangles;
        puma::isosurface_MarchingCubes(&Triangles,&grayWS,puma::Cutoff(90,255),true,1,false,0);

        bool success = puma::export_STL(&Triangles,false,"");
        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        success = puma::export_STL(&Triangles,true,"");
        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {
        std::string suiteName = "export_STL_Test";
        std::string testName = "export_STL_Test: Test 3 - Invalid FileName";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 3, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        std::vector< puma::Triangle <float> > Triangles;
        puma::isosurface_MarchingCubes(&Triangles,&grayWS,puma::Cutoff(90,255),true,1,false,0);

        bool success = puma::export_STL(&Triangles,false,"/sdf/sdkfi/stl.stl");
        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        success = puma::export_STL(&Triangles,true,"/sdf/sdkfi/stl.stl");
        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test4() {
        std::string suiteName = "export_STL_Test";
        std::string testName = "export_STL_Test: Test 4 - Ascii FiberForm - void and non-void";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 4, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        grayWS.crop(0,49,0,49,0,49);

        std::vector< puma::Triangle <float> > Triangles;
        puma::isosurface_MarchingCubes(&Triangles,&grayWS,puma::Cutoff(0,89),true,1,false,0);

        std::cout << "Triangle size1: " << Triangles.size() << std::endl;

        bool success = puma::export_STL(&Triangles,false,"cpp/test/out/stl/test4_void.stl");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        puma::isosurface_MarchingCubes(&Triangles,&grayWS,puma::Cutoff(90,255),true,1,false,0);

        std::cout << "Triangle size2: " << Triangles.size() << std::endl;


        success = puma::export_STL(&Triangles,false,"cpp/test/out/stl/test4_solid.stl");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        success = puma::export_STL(&grayWS,puma::Cutoff(0,89),false,"cpp/test/out/stl/test4_void_ws.stl");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }


        success = puma::export_STL(&grayWS, puma::Cutoff(90,255),false,"cpp/test/out/stl/test4_solid_ws.stl");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {
        std::string suiteName = "export_STL_Test";
        std::string testName = "export_STL_Test: Test 5 - Non-Ascii FiberForm- void and non-void";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 5, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        std::vector< puma::Triangle <float> > Triangles;
        puma::isosurface_MarchingCubes(&Triangles,&grayWS,puma::Cutoff(0,89),true,1,false,0);

        bool success = puma::export_STL(&Triangles,true,"cpp/test/out/stl/test5_void.stl");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        puma::isosurface_MarchingCubes(&Triangles,&grayWS,puma::Cutoff(90,255),true,1,false,0);

        success = puma::export_STL(&Triangles,true,"cpp/test/out/stl/test5_solid.stl");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        return result;
    }

};
