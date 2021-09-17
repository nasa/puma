#include "../testframework/subtest.h"
#include "puma.h"


class MeanInterceptLength_Test : public SubTest {
public:

    MeanInterceptLength_Test() {
        testSuiteName = "MeanInterceptLength_Test";

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
        //tests.push_back(test12);
        //tests.push_back(test13);
        tests.push_back(test14);
        tests.push_back(test15);
        tests.push_back(test16);
        tests.push_back(test17);
        tests.push_back(test18);
        tests.push_back(test19);
        tests.push_back(test20);

    }

    static TestResult test1() {

        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 1 - size = 0";
        std::string testDescription = "Should return puma::Vec3 of -1's because of invalid material workspace size";
        TestResult result(suiteName, testName, 1, testDescription);

        puma::Workspace segWS(1e-6,false);
        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(0,255));

        std::cout << mil.x << ' ' << mil.y << ' ' << mil.z << std::endl;

        if(!assertEquals((long)-1,(long)mil.x, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.y, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 2 - size = 100^3, val = -1";
        std::string testDescription = "Should return puma::Vec3 of -1's because of invalid value for material workspace";
        TestResult result(suiteName, testName, 2, testDescription);

        puma::Workspace segWS(100,100,100,0,1e-6,false);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(-1,-1));

        if(!assertEquals((long)-1,(long)mil.x, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.y, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 3 - size = 100^3, val = 32767";
        std::string testDescription = "Should return puma::Vec3 of -1's because of invalid value for material workspace";
        TestResult result(suiteName, testName, 3, testDescription);

        puma::Workspace segWS(100,100,100,0,1e-6,false);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(32768,32768));

        if(!assertEquals((long)-1,(long)mil.x, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.y, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test4() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 4 - size = 100^3, empty matrix, val = 0";
        std::string testDescription = "Should return puma::Vec3 of -1's because of division by zero due to infinite mean intercept length";
        TestResult result(suiteName, testName, 4, testDescription);

        puma::Workspace segWS(100,100,100,0,1e-6,false);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(0,0));

        if(!assertEquals((long)-1,(long)mil.x, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.y, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 5 - size = 100^3, full matrix, val = 0";
        std::string testDescription = "Should return puma::Vec3 of zeros because there are no void cells";
        TestResult result(suiteName, testName, 5, testDescription);

        puma::Workspace segWS(100,100,100,1,1e-6,false);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(0,0));

        if(!assertEquals((long)0,(long)mil.x, &result)) {
            return result;
        }
        if(!assertEquals((long)0,(long)mil.y, &result)) {
            return result;
        }
        if(!assertEquals((long)0,(long)mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test6() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 6 - size = 1x100x100, channel in y, val = 0";
        std::string testDescription = "Should return -1 in x and y directions and 98*voxel_length in z";
        TestResult result(suiteName, testName, 6, testDescription);

        puma::Workspace segWS(1,100,100,0,1e-6,false);
        segWS.matrix.set(0,0,0,99,0,0,1);
        segWS.matrix.set(0,0,0,99,99,99,1);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(0,0));

        if(!assertEquals((long)-1,(long)mil.x, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.y, &result)) {
            return result;
        }
        if(!assertEquals(1e-6*98,mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test7() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 7 - size = 1x100x100, channel in z, val = 0";
        std::string testDescription = "Should return -1 in x and z directions and 98*voxel_length in y";
        TestResult result(suiteName, testName, 7, testDescription);

        puma::Workspace segWS(1,100,100,0,1e-6,false);
        segWS.matrix.set(0,0,0,0,0,99,1);
        segWS.matrix.set(0,0,99,99,0,99,1);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(0,0));

        if(!assertEquals((long)-1,(long)mil.x, &result)) {
            return result;
        }
        if(!assertEquals(1e-6*98,mil.y, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test8() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 8 - size = 100x1x100, channel in z, val = 0";
        std::string testDescription = "Should return -1 in y and z directions and 98*voxel_length in x";
        TestResult result(suiteName, testName, 8, testDescription);

        puma::Workspace segWS(100,1,100,0,1e-6,false);
        segWS.matrix.set(0,0,0,0,0,99,1);
        segWS.matrix.set(99,99,0,0,0,99,1);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(0,0));

        if(!assertEquals(1e-6*98,mil.x, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.y, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test9() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 9 - size = 100x1x100, channel in x, val = 0";
        std::string testDescription = "Should return -1 in y and x directions and 98*voxel_length in z";
        TestResult result(suiteName, testName, 9, testDescription);

        puma::Workspace segWS(100,1,100,0,1e-6,false);
        segWS.matrix.set(0,99,0,0,0,0,1);
        segWS.matrix.set(0,99,0,0,99,99,1);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(0,0));

        if(!assertEquals((long)-1,(long)mil.x, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.y, &result)) {
            return result;
        }
        if(!assertEquals(1e-6*98,mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test10() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 10 - size = 100x100x1, channel in y, val = 0";
        std::string testDescription = "Should return -1 in y and z directions and 98*voxel_length in x";
        TestResult result(suiteName, testName, 10, testDescription);

        puma::Workspace segWS(100,100,1,0,1e-6,false);
        segWS.matrix.set(0,0,0,99,0,0,1);
        segWS.matrix.set(99,99,0,99,0,0,1);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(0,0));

        if(!assertEquals(1e-6*98,mil.x, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.y, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test11() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 11 - size = 100x100x1, channel in x, val = 0";
        std::string testDescription = "Should return -1 in x and z directions and 98*voxel_length in y";
        TestResult result(suiteName, testName, 11, testDescription);

        puma::Workspace segWS(100,100,1,0,1e-6,false);
        segWS.matrix.set(0,99,0,0,0,0,1);
        segWS.matrix.set(0,99,99,99,0,0,1);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(0,0));

        if(!assertEquals((long)-1,(long)mil.x, &result)) {
            return result;
        }
        if(!assertEquals(1e-6*98,mil.y, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test12() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 12 - size = 1300x1300x1300, Spheres, val = 0";
        std::string testDescription = "Checking Large Spheres Sample";
        TestResult result(suiteName, testName, 12, testDescription);

        puma::Workspace segWS(1300,1300,1300,0,1e-6,false);
        puma::import_3DTiff(&segWS,puma::path_to_example_file("1300_Spheres.tif"));

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(0,0));

        if(!assertEquals(2.182895e-4, mil.x, &result)) {
            return result;
        }
        if(!assertEquals(2.182895e-4,mil.y, &result)) {
            return result;
        }
        if(!assertEquals(2.182895e-4,mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test13() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 13 - size = 130x130x130, Sphere, val = 0";
        std::string testDescription = "Checking 1 Sphere from Large Sample";
        TestResult result(suiteName, testName, 13, testDescription);

        puma::Workspace segWS(130,130,130,0,1e-6,false);
        puma::import_3DTiff(&segWS,puma::path_to_example_file("1300_Spheres.tif"),0,130,0,130,0,130);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(0,0));

        if(!assertEquals(2.162933e-4, mil.x, &result)) {
            return result;
        }
        if(!assertEquals(2.162933e-4,mil.y, &result)) {
            return result;
        }
        if(!assertEquals(2.162933e-4,mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test14() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 14 - size = 200x200x200, FiberForm, val = 0";
        std::string testDescription = "Checking FiberForm Sample";
        TestResult result(suiteName, testName, 14, testDescription);

        puma::Workspace segWS(200,200,200,0,1e-6,false);
        puma::import_3DTiff(&segWS,puma::path_to_example_file("200_fiberform_segmented.tif"));

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&segWS,puma::Cutoff(0,0));

        if(!assertEquals(6.32693e-5, mil.x, &result)) {
            return result;
        }
        if(!assertEquals(7.02535e-5,mil.y, &result)) {
            return result;
        }
        if(!assertEquals(5.43159e-5,mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test15() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 15 - size = 200x200x200, FiberForm, lowVal = 0, highVal = 89";
        std::string testDescription = "Checking FiberForm Sample using Grayscale Workspace";
        TestResult result(suiteName, testName, 15, testDescription);

        puma::Workspace grayWS(200,200,200,0,1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"));

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&grayWS,puma::Cutoff(0,89));

        if(!assertEquals(6.32693e-5, mil.x, &result)) {
            return result;
        }
        if(!assertEquals(7.02535e-5,mil.y, &result)) {
            return result;
        }
        if(!assertEquals(5.43159e-5,mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test16() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 16 - size = 0, empty, lowVal = 0, highVal = 0";
        std::string testDescription = "Testing that puma::Vec3 of -1's is returned when empty Grayscale workspace is given";
        TestResult result(suiteName, testName, 16, testDescription);

        puma::Workspace grayWS(1e-6,false);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&grayWS,puma::Cutoff(0,0));

        if(!assertEquals((long)-1,(long)mil.x, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.y, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test17() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 17 - size = 100x100x100, empty, lowVal = -1, highVal = 0";
        std::string testDescription = "Testing that puma::Vec3 of -1's is returned when low value is < 0";
        TestResult result(suiteName, testName, 17, testDescription);

        puma::Workspace grayWS(100,100,100,0,1e-6,false);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&grayWS,puma::Cutoff(-1,0));

        if(!assertEquals((long)-1,(long)mil.x, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.y, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test18() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 18 - size = 100x100x100, empty, lowVal = 0, highVal = -1";
        std::string testDescription = "Testing that puma::Vec3 of -1's is returned when high value is < 0";
        TestResult result(suiteName, testName, 18, testDescription);

        puma::Workspace grayWS(100,100,100,0,1e-6,false);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&grayWS,puma::Cutoff(0,-1));

        if(!assertEquals((long)-1,(long)mil.x, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.y, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test19() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 19 - size = 100x100x100, empty, lowVal = 1, highVal = 0";
        std::string testDescription = "Testing that puma::Vec3 of -1's is returned when high value < low value";
        TestResult result(suiteName, testName, 19, testDescription);

        puma::Workspace grayWS(100,100,100,0,1e-6,false);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&grayWS,puma::Cutoff(1,0));

        if(!assertEquals((long)-1,(long)mil.x, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.y, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test20() {
        std::string suiteName = "MeanInterceptLength_Test";
        std::string testName = "MeanInterceptLength_Test: Test 20 - size = 100x100x100, empty, lowVal = 0, highVal = 255";
        std::string testDescription = "Testing that puma::Vec3 of -1's is returned when the entire range is considered void";
        TestResult result(suiteName, testName, 20, testDescription);

        puma::Workspace grayWS(100,100,100,0,1e-6,false);

        puma::Vec3<double> mil = puma::compute_MeanInterceptLength(&grayWS,puma::Cutoff(0,255));

        if(!assertEquals((long)-1,(long)mil.x, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.y, &result)) {
            return result;
        }
        if(!assertEquals((long)-1,(long)mil.z, &result)) {
            return result;
        }

        return result;
    }

};
