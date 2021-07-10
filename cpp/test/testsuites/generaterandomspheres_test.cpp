#include "../testframework/subtest.h"
#include "puma.h"

#include <vector>


class Generate_RandomSpheres_Test : public SubTest {
public:

    Generate_RandomSpheres_Test() {

        testSuiteName = "Generate_RandomSpheres_Test";

        tests.push_back(test1);
        tests.push_back(test2);
        tests.push_back(test3);
        tests.push_back(test4);
        tests.push_back(test5);
        tests.push_back(test6);
        tests.push_back(test7);
        tests.push_back(test8);
        tests.push_back(test9);
        tests.push_back(test10);
        tests.push_back(test11);
        tests.push_back(test12);
        tests.push_back(test13);
    }


    static TestResult test1() {

        std::string suiteName = "Generate_RandomSpheres_Test";
        std::string testName = "Generate_RandomSpheres_Test: xsize= -200, ysize=200, zsize=200, avgRadius=5.0, dRadius=0.0, "
                              "intersect=true, poro=0.9, randomSeed=100";
        std::string testDescription = "Should return false because of invalid xsize (negative)";
        TestResult result(suiteName, testName, 1, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace sphereWS(1e-6,false);
        RandomSpheresInput input(-200,200,200,20.0,0.0,true,0.9,100);

        bool success = puma::generateRandomSpheres(&sphereWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test2() {

        std::string suiteName = "Generate_RandomSpheres_Test";
        std::string testName = "Generate_RandomSpheres_Test: xsize= 200, ysize= -200, zsize=200, avgRadius=5.0, dRadius=0.0, "
                              "intersect=true, poro=0.9, randomSeed=100";
        std::string testDescription = "Should return false because of invalid ysize (negative)";
        TestResult result(suiteName, testName, 2, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace sphereWS(1e-6,false);
        RandomSpheresInput input(200,-200,200,20.0,0.0,true,0.9,100);

        bool success = puma::generateRandomSpheres(&sphereWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test3() {

        std::string suiteName = "Generate_RandomSpheres_Test";
        std::string testName = "Generate_RandomSpheres_Test: xsize= 200, ysize= 200, zsize= -200, avgRadius=5.0, dRadius=0.0, "
                              "intersect=true, poro=0.9, randomSeed=100";
        std::string testDescription = "Should return false because of invalid zsize (negative)";
        TestResult result(suiteName, testName, 3, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace sphereWS(1e-6,false);
        RandomSpheresInput input(200,200,-200,20.0,0.0,true,0.9,100);

        bool success = puma::generateRandomSpheres(&sphereWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test4() {

        std::string suiteName = "Generate_RandomSpheres_Test";
        std::string testName = "Generate_RandomSpheres_Test: xsize= 200, ysize= 200, zsize= 200, avgRadius= -20.0, dRadius=0.0, "
                              "intersect=true, poro=0.9, randomSeed=100";
        std::string testDescription = "Should return false because of invalid avgRadius (negative)";
        TestResult result(suiteName, testName, 4, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace sphereWS(1e-6,false);
        RandomSpheresInput input(200,200,200,-20.0,0.0,true,0.9,100);

        bool success = puma::generateRandomSpheres(&sphereWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test5() {

        std::string suiteName = "Generate_RandomSpheres_Test";
        std::string testName = "Generate_RandomSpheres_Test: xsize= 200, ysize= 200, zsize= 200, avgRadius= 20.0, dRadius= -10.0, "
                              "intersect=true, poro=0.9, randomSeed=100";
        std::string testDescription = "Should return false because of invalid dRadius (negative)";
        TestResult result(suiteName, testName, 5, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace sphereWS(1e-6,false);
        RandomSpheresInput input(200,200,200,20.0,-10.0,true,0.9,100);

        bool success = puma::generateRandomSpheres(&sphereWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test6() {

        std::string suiteName = "Generate_RandomSpheres_Test";
        std::string testName = "Generate_RandomSpheres_Test: xsize= 200, ysize= 200, zsize= 200, avgRadius= 20.0, dRadius= 30.0, "
                              "intersect=true, poro=0.9, randomSeed=100";
        std::string testDescription = "Should return false because of invalid dRadius (larger than avgRadius)";
        TestResult result(suiteName, testName, 6, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace sphereWS(1e-6,false);
        RandomSpheresInput input(200,200,200,20.0,30.0,true,0.9,100);

        bool success = puma::generateRandomSpheres(&sphereWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test7() {

        std::string suiteName = "Generate_RandomSpheres_Test";
        std::string testName = "Generate_RandomSpheres_Test: xsize= 200, ysize= 200, zsize= 200, avgRadius= 20.0, dRadius= 0.0, "
                              "intersect=true, poro= -0.9, randomSeed=100";
        std::string testDescription = "Should return false because of invalid porosity (negative)";
        TestResult result(suiteName, testName, 7, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace sphereWS(1e-6,false);
        RandomSpheresInput input(200,200,200,20.0,0.0,true,-0.9,100);

        bool success = puma::generateRandomSpheres(&sphereWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test8() {

        std::string suiteName = "Generate_RandomSpheres_Test";
        std::string testName = "Generate_RandomSpheres_Test: xsize= 200, ysize= 200, zsize= 200, avgRadius= 20.0, dRadius= 0.0, "
                              "intersect=true, poro= 1.9, randomSeed=100";
        std::string testDescription = "Should return false because of invalid porosity (larger than 1)";
        TestResult result(suiteName, testName, 8, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace sphereWS(1e-6,false);
        RandomSpheresInput input(200,200,200,20.0,0.0,true,1.9,100);

        bool success = puma::generateRandomSpheres(&sphereWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test9() {

        std::string suiteName = "Generate_RandomSpheres_Test";
        std::string testName = "Generate_RandomSpheres_Test: xsize= 200, ysize= 200, zsize= 200, avgRadius= 20.0, dRadius= 0.0, "
                              "intersect=true, poro= 0.9, randomSeed= -100";
        std::string testDescription = "Should return false because of invalid randomSeed (negative)";
        TestResult result(suiteName, testName, 9, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace sphereWS(1e-6,false);
        RandomSpheresInput input(200,200,200,20.0,0.0,true,0.9,-100);

        bool success = puma::generateRandomSpheres(&sphereWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test10() {

        std::string suiteName = "Generate_RandomSpheres_Test";
        std::string testName = "Generate_RandomSpheres_Test: xsize= 200, ysize= 200, zsize= 200, avgRadius= 20.0, dRadius= 0.0, "
                              "intersect=true, poro= 0.9, randomSeed= INT32_MAX";
        std::string testDescription = "Should return false because of invalid randomSeed (equal to INT32_MAX)";
        TestResult result(suiteName, testName, 10, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace sphereWS(1e-6,false);
        RandomSpheresInput input(200,200,200,20.0,0.0,true,0.9,INT32_MAX);

        bool success = puma::generateRandomSpheres(&sphereWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test11() {

        std::string suiteName = "Generate_RandomSpheres_Test";
        std::string testName = "Generate_RandomSpheres_Test: xsize=200, ysize=200, zsize=200, avgRadius=80.0, dRadius=3.0, "
                               "intersect=false, poro=0.99, randomSeed=100";
        std::string testDescription = "Testing for ONLY A FEW non-intersecting spheres: Should return true because of all parameters are valid";
        TestResult result(suiteName, testName, 11, testDescription);

        puma::Timer t;

        //creates an empty Grayscale workspace
        puma::Workspace sphereWS(1e-6,false);
        RandomSpheresInput input(200,200,200,80.0,3.0,false,0.99,100);

        //in this case, the return should be false since we passed in an empty tomography workspace. Also becuase 3>domain size
        bool success = puma::generateRandomSpheres(&sphereWS, input);

        std::cout << "Time: " << t.elapsed() << std::endl;

        puma::export_3DTiff(&sphereWS,"cpp/test/out/tiff/Generate_RandomSpheres_Test11.tif",false);

        std::vector<puma::Triangle<float> > Triangles;
        puma::isosurface_MarchingCubes(&Triangles,&sphereWS,puma::Cutoff(128,255),true,1,false,40);

        Export_STL test(&Triangles,false,"cpp/test/out/stl/Generate_RandomSpheres_Test11.stl");

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(true,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test12() {

        std::string suiteName = "Generate_RandomSpheres_Test";
        std::string testName = "Generate_RandomSpheres_Test: xsize=200, ysize=200, zsize=200, avgRadius=10.0, dRadius=3.0, "
                               "intersect=false, poro=0.99, randomSeed=100";
        std::string testDescription = "Testing for non-intersecting spheres: Should return true because of all parameters are valid";
        TestResult result(suiteName, testName, 11, testDescription);

        puma::Timer t;

        //creates an empty Grayscale workspace
        puma::Workspace sphereWS(1e-6,false);
        RandomSpheresInput input(200,200,200,10.0,3.0,false,0.99,100);

        //in this case, the return should be false since we passed in an empty tomography workspace. Also becuase 3>domain size
        bool success = puma::generateRandomSpheres(&sphereWS, input);

        std::cout << "Time: " << t.elapsed() << std::endl;

        puma::export_3DTiff(&sphereWS,"cpp/test/out/tiff/Generate_RandomSpheres_Test12.tif",false);

        std::vector<puma::Triangle<float> > Triangles;
        puma::isosurface_MarchingCubes(&Triangles,&sphereWS,puma::Cutoff(128,255),true,1,false);

        Export_STL test(&Triangles,false,"cpp/test/out/stl/Generate_RandomSpheres_Test12.stl");

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(true,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test13() {

        std::string suiteName = "Generate_RandomSpheres_Test";
        std::string testName = "Generate_RandomSpheres_Test: xsize=200, ysize=200, zsize=200, avgRadius=10.0, dRadius=3.0, "
                               "intersect=false, poro=0.99, randomSeed=100";
        std::string testDescription = "Testing for intersecting spheres: Should return true because of all parameters are valid";
        TestResult result(suiteName, testName, 12, testDescription);

        puma::Timer t;

        //creates an empty Grayscale workspace
        puma::Workspace sphereWS(1e-6,false);
        RandomSpheresInput input(200,200,200,10.0,3.0,true,0.99,100);

        //in this case, the return should be false since we passed in an empty tomography workspace. Also becuase 3>domain size
        bool success = puma::generateRandomSpheres(&sphereWS, input);

        std::cout << "Time: " << t.elapsed() << std::endl;

        puma::export_3DTiff(&sphereWS,"cpp/test/out/tiff/Generate_RandomSpheres_Test13.tif",false);

        std::vector<puma::Triangle<float> > Triangles;
        puma::isosurface_MarchingCubes(&Triangles,&sphereWS,puma::Cutoff(128,255),true,1,false);

        Export_STL test(&Triangles,false,"cpp/test/out/stl/Generate_RandomSpheres_Test13.stl");

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(true,success,&result)) {
            return result;
        }

        return result;
    }

};

