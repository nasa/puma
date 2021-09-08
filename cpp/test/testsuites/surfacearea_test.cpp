#include "../testframework/subtest.h"
#include "puma.h"

#include <vector>
#include <iomanip>


class SurfaceArea_Test : public SubTest {
public:

    SurfaceArea_Test() {
        testSuiteName = "SurfaceArea_Test";

        tests.push_back(test1);
        tests.push_back(test2);
        tests.push_back(test3);
        tests.push_back(test4);
        tests.push_back(test5);
        tests.push_back(test6);
        tests.push_back(test7);
        tests.push_back(test8);
//        tests.push_back(test9);
//        tests.push_back(test10);
        tests.push_back(test11);
        tests.push_back(test12);
        tests.push_back(test13);
        tests.push_back(test14);
        tests.push_back(test15);
        tests.push_back(test16);
        tests.push_back(test17);
        tests.push_back(test18);
//        tests.push_back(test19);
//        tests.push_back(test20);
        tests.push_back(test21);
        tests.push_back(test22);
        tests.push_back(test23);
        tests.push_back(test24);
//        tests.push_back(test25);
//        tests.push_back(test26);
        tests.push_back(test27);
        tests.push_back(test28);

    }

    static TestResult test1() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 1 - Empty Grayscale Matrix, Voxel based";
        std::string testDescription = "Should return -1";
        TestResult result(suiteName, testName, 1, testDescription);

        puma::Workspace grayWS(1e-6,false);
        std::pair<double, double> sa = compute_SurfaceAreaVoxels(&grayWS, puma::Cutoff(0, 255), 0);

        if(!assertEquals((long)-1,(long)sa.first, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 2 - Empty Segmented Matrix, Voxel based";
        std::string testDescription = "Should return -1";
        TestResult result(suiteName, testName, 2, testDescription);

        puma::Workspace segWS(1e-6,false);
        std::pair<double, double> sa = compute_SurfaceAreaVoxels(&segWS, puma::Cutoff(0, 1), 0);

        if(!assertEquals((long)-1,(long)sa.first, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 3 - Invalid Cutoffs, Grayscale Matrix, Voxel based";
        std::string testDescription = "Should return -1";
        TestResult result(suiteName, testName, 3, testDescription);

        puma::Workspace grayWS(1,1,1,0,1e-6,false);
        std::pair<double, double> sa = compute_SurfaceAreaVoxels(&grayWS, puma::Cutoff(-1, 255), 0);

        if(!assertEquals((long)-1,(long)sa.first, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test4() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 4 - Invalid Cutoffs, Segmented Matrix, Voxel based";
        std::string testDescription = "Should return -1";
        TestResult result(suiteName, testName, 4, testDescription);

        puma::Workspace segWS(1,1,1,0,1e-6,false);
        std::pair<double, double> sa = compute_SurfaceAreaVoxels(&segWS, puma::Cutoff(-1, 1), 0);

        if(!assertEquals((long)-1,(long)sa.first, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {
        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 5 - Singular Cutoff Value Test, Grayscale, Voxel based";
        std::string testDescription = "Tests that the voxel based method performs correctly when the low and high cutoff values are the same.";
        TestResult result(suiteName, testName, 5, testDescription);

        puma::Workspace grayWS(3,3,3,0,1e-6,false);
        grayWS.matrix.set(2,2,0,2,0,2,255);
        grayWS.matrix.set(1,1,1,1,1,1,128);

        std::pair<double, double> sa = compute_SurfaceAreaVoxels(&grayWS, puma::Cutoff(128, 128), 0);

        if(!assertEquals(6e-12,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(2./9*1e6,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test6() {
        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 6 - Surface Area at Edge of Domain, Grayscale, Voxel based";
        std::string testDescription = "Tests that the surface area on the face of the domain is not included";
        TestResult result(suiteName, testName, 6, testDescription);

        puma::Workspace grayWS(3,3,3,0,1e-6,false);
        grayWS.matrix.set(2,2,0,2,0,2,255);
        grayWS.matrix.set(1,1,1,1,1,1,128);

        std::pair<double, double> sa = compute_SurfaceAreaVoxels(&grayWS, puma::Cutoff(255, 255), 0);

        if(!assertEquals(9e-12,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(1./3*1e6,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test7() {
        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 7 - Singular Cutoff Value Test, Segmented, Voxel based";
        std::string testDescription = "Tests that the voxel based method performs correctly when the low and high cutoff values are the same.";
        TestResult result(suiteName, testName, 7, testDescription);

        puma::Workspace segWS(3,3,3,0,1e-6,false);
        segWS.matrix.set(2,2,0,2,0,2,2);
        segWS.matrix.set(1,1,1,1,1,1,1);

        std::pair<double, double> sa = compute_SurfaceAreaVoxels(&segWS, puma::Cutoff(1, 1), 0);

        if(!assertEquals(6e-12,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(2./9*1e6,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test8() {
        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 8 - Surface Area at Edge of Domain, Segmented, Voxel based";
        std::string testDescription = "Tests that the surface area on the face of the domain is not included";
        TestResult result(suiteName, testName, 8, testDescription);

        puma::Workspace segWS(3,3,3,0,1e-6,false);
        segWS.matrix.set(2,2,0,2,0,2,2);
        segWS.matrix.set(1,1,1,1,1,1,1);

        std::pair<double, double> sa = compute_SurfaceAreaVoxels(&segWS, puma::Cutoff(2, 2), 0);

        if(!assertEquals(9e-12,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(1./3*1e6,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test9() {
        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 9 - Large Spheres, Segmented, Voxel based";
        std::string testDescription = "Tests on a large dataset";
        TestResult result(suiteName, testName, 9, testDescription);

        puma::Workspace segWS(1300,1300,1300,0,1e-6,false);
        puma::import_3DTiff(&segWS,puma::path_to_example_file("1300_Spheres.tif"),0);

        std::pair<double, double> sa = compute_SurfaceAreaVoxels(&segWS, puma::Cutoff(1, 1), 0);

        if(!assertEquals(4.6326e-5,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(4.6326e-5/2.197e-9,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test10() {
        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 10 - Large Spheres, Grayscale, Voxel based";
        std::string testDescription = "Tests on a large dataset";
        TestResult result(suiteName, testName, 10, testDescription);

        puma::Workspace grayWS(1300,1300,1300,0,1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("1300_Spheres.tif"),0);

        std::pair<double, double> sa = compute_SurfaceAreaVoxels(&grayWS, puma::Cutoff(128, 255), 0);

        if(!assertEquals(4.6326e-5,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(4.6326e-5/2.197e-9,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test11() {
        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 11 - FiberForm, Segmented, Voxel based";
        std::string testDescription = "Tests on fiberform";
        TestResult result(suiteName, testName, 11, testDescription);

        puma::Workspace segWS(200,200,200,0,1e-6,false);
        puma::import_3DTiff(&segWS,puma::path_to_example_file("200_fiberform_segmented.tif"),0);

        std::pair<double, double> sa = compute_SurfaceAreaVoxels(&segWS, puma::Cutoff(1, 1), 0);

        if(!assertEquals(6.17327e-7,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(6.17327e-7/8e-12,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test12() {
        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 12 - FiberForm, Grayscale, Voxel based";
        std::string testDescription = "Tests on fiberform";
        TestResult result(suiteName, testName, 12, testDescription);

        puma::Workspace grayWS(200,200,200,0,1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        std::pair<double, double> sa = compute_SurfaceAreaVoxels(&grayWS, puma::Cutoff(90, 255), 0);

        if(!assertEquals(6.17327e-7,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(6.17327e-7/8e-12,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test13() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 13 - Empty Grayscale Matrix, Marching Cubes based";
        std::string testDescription = "Should return -1";
        TestResult result(suiteName, testName, 13, testDescription);

        puma::Workspace grayWS(1e-6,false);
        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&grayWS, puma::Cutoff(0, 255), 0, 0);

        if(!assertEquals((long)-1,(long)sa.first, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test14() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 14 - Empty Segmented Matrix, Marching Cubes based";
        std::string testDescription = "Should return -1";
        TestResult result(suiteName, testName, 14, testDescription);

        puma::Workspace segWS(1e-6,false);
        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&segWS, puma::Cutoff(0, 1), 0, 0);

        if(!assertEquals((long)-1,(long)sa.first, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test15() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 15 - Invalid Cutoffs, Grayscale Matrix, Marching Cubes based";
        std::string testDescription = "Should return -1";
        TestResult result(suiteName, testName, 15, testDescription);

        puma::Workspace grayWS(1,1,1,0,1e-6,false);
        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&grayWS, puma::Cutoff(0, 50000), 0, 0);

        if(!assertEquals((long)-1,(long)sa.first, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test16() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 16 - Invalid Cutoffs, Segmented Matrix, Marching Cubes based";
        std::string testDescription = "Should return -1";
        TestResult result(suiteName, testName, 16, testDescription);

        puma::Workspace segWS(1,1,1,0,1e-6,false);
        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&segWS, puma::Cutoff(0, 50000), 0, 0);

        if(!assertEquals((long)-1,(long)sa.first, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test17() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 17 - 3 Materials, Grayscale Matrix, Marching Cubes based";
        std::string testDescription = "Tests on a small slab";
        TestResult result(suiteName, testName, 17, testDescription);

        puma::Workspace grayWS(3,3,3,0,1e-6,false);
        grayWS.matrix.set(1,1,0,2,0,2,127);
        grayWS.matrix.set(2,2,0,2,0,2,255);
        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&grayWS, puma::Cutoff(63, 191), 0, 0);

        if(!assertEquals(8e-12,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(1e6,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test18() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 18 - 3 Materials, Segmented Matrix, Marching Cubes based";
        std::string testDescription = "Tests on a small slab";
        TestResult result(suiteName, testName, 18, testDescription);

        puma::Workspace segWS(3,3,3,0,1e-6,false);
        segWS.matrix.set(1,1,0,2,0,2,1);
        segWS.matrix.set(2,2,0,2,0,2,2);
        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&segWS, puma::Cutoff(1, 1), 0, 0);

        if(!assertEquals(8e-12,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(1e6,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test19() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 19 - Large Spheres, Grayscale Matrix, Marching Cubes based";
        std::string testDescription = "Tests on a large dataset";
        TestResult result(suiteName, testName, 19, testDescription);

        puma::Workspace grayWS(1300,1300,1300,0,1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("1300_Spheres.tif"),0);

        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&grayWS, puma::Cutoff(128, 255), 0, 0);

        if(!assertEquals(3.105341790e-5,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(3.105341790e-5/2.191933899e-9,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test20() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 20 - Large Spheres, Segmented Matrix, Marching Cubes based";
        std::string testDescription = "Tests on a large dataset";
        TestResult result(suiteName, testName, 20, testDescription);

        puma::Workspace segWS(1300,1300,1300,0,1e-6,false);
        puma::import_3DTiff(&segWS,puma::path_to_example_file("1300_Spheres.tif"),0);
        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&segWS, puma::Cutoff(1, 1), 0, 0);

        if(!assertEquals(3.350938727e-5,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(3.350938727e-5/2.191933899e-9,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test21() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 21 - FiberForm, Grayscale Matrix, Marching Cubes based";
        std::string testDescription = "Tests on fiberform";
        TestResult result(suiteName, testName, 21, testDescription);

        puma::Workspace grayWS(200,200,200,0,1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&grayWS, puma::Cutoff(90, 255), 0, 0);

        if(!assertEquals(4.12109e-07,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(4.12109e-07/7.880599e-12,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test22() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 22 - FiberForm, Segmented Matrix, Marching Cubes based";
        std::string testDescription = "Tests on fiberform";
        TestResult result(suiteName, testName, 22, testDescription);

        puma::Workspace segWS(200,200,200,0,1e-6,false);
        puma::import_3DTiff(&segWS,puma::path_to_example_file("200_fiberform_segmented.tif"),0);
        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&segWS, puma::Cutoff(1, 1), 0, 0);

        if(!assertEquals(4.479217316e-7,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(4.479217316e-7/7.880599e-12,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test23() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 23 - 3 Materials, Grayscale Matrix, Marching Cubes based, Interpolated Vertices";
        std::string testDescription = "Tests on a small slab";
        TestResult result(suiteName, testName, 23, testDescription);

        puma::Workspace grayWS(3,3,3,0,1e-6,false);
        grayWS.matrix.set(1,1,0,2,0,2,127);
        grayWS.matrix.set(2,2,0,2,0,2,255);
        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&grayWS, puma::Cutoff(63, 191), 1, 0);

        if(!assertEquals(9e-12,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(1e6/3,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test24() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 24 - 3 Materials, Segmented Matrix, Marching Cubes based, Interpolated Vertices";
        std::string testDescription = "Tests on a small slab";
        TestResult result(suiteName, testName, 24, testDescription);

        puma::Workspace segWS(3,3,3,0,1e-6,false);
        segWS.matrix.set(1,1,0,2,0,2,1);
        segWS.matrix.set(2,2,0,2,0,2,2);
        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&segWS, puma::Cutoff(1, 1), 1, 0);

        if(!assertEquals(9e-12,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(1e6/3,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test25() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 25 - Large Spheres, Grayscale Matrix, Marching Cubes based, Interpolated Vertices";
        std::string testDescription = "Tests on a large dataset";
        TestResult result(suiteName, testName, 25, testDescription);

        puma::Workspace grayWS(1300,1300,1300,0,1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("1300_Spheres.tif"),0);

        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&grayWS, puma::Cutoff(128, 255), 1, 0);

        if(!assertEquals(3.070990279e-05,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(3.070990279e-05/2.197e-9,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test26() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 26 - Large Spheres, Segmented Matrix, Marching Cubes based, Interpolated Vertices";
        std::string testDescription = "Tests on a large dataset";
        TestResult result(suiteName, testName, 26, testDescription);

        puma::Workspace segWS(1300,1300,1300,0,1e-6,false);
        puma::import_3DTiff(&segWS,puma::path_to_example_file("1300_Spheres.tif"),0);
        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&segWS, puma::Cutoff(1, 1), 1, 0);

        if(!assertEquals(3.130272474e-05,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(3.130272474e-05/2.197e-9,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test27() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 27 - FiberForm, Grayscale Matrix, Marching Cubes based, Interpolated Vertices";
        std::string testDescription = "Tests on fiberform";
        TestResult result(suiteName, testName, 27, testDescription);

        puma::Workspace grayWS(200,200,200,0,1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&grayWS, puma::Cutoff(90, 255), 1, 0);

        if(!assertEquals(4.09222e-07,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(4.09222e-07/8e-12,sa.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test28() {

        std::string suiteName = "SurfaceArea_Test";
        std::string testName = "SurfaceArea_Test: Test 28 - FiberForm, Segmented Matrix, Marching Cubes based, Interpolated Vertices";
        std::string testDescription = "Tests on fiberform";
        TestResult result(suiteName, testName, 28, testDescription);

        puma::Workspace segWS(200,200,200,0,1e-6,false);
        puma::import_3DTiff(&segWS,puma::path_to_example_file("200_fiberform_segmented.tif"),0);
        std::pair<double, double> sa = compute_SurfaceAreaMarchingCubes(&segWS, puma::Cutoff(1, 1), 1, 0);

        if(!assertEquals(4.122054032e-7,sa.first, &result)) {
            return result;
        }
        if(!assertEquals(4.122054032e-7/8e-12,sa.second, &result)) {
            return result;
        }

        return result;
    }
};
