#include "../testframework/subtest.h"
#include "puma.h"


class BilateralFilter_Test : public SubTest {
public:

    BilateralFilter_Test() {

        testSuiteName = "BilateralFilter_Test";

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
    }


    static TestResult test1() {

        std::string suiteName = "BilateralFilter_Test";
        std::string testName = "BilateralFilter_Test: size=0, radius=3, sigma_d=1, sigma_r=1";
        std::string testDescription = "Should return false because of invalid Workspace size";
        TestResult result(suiteName, testName, 1, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace grayWS(1e-6, false);

        //in this case, the return should be false since we passed in an empty Grayscale workspace. Also becuase 3>domain size
        bool success = puma::filter_Bilateral(&grayWS,3,1,1);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test2() {

        std::string suiteName = "BilateralFilter_Test";
        std::string testName = "BilateralFilter_Test: size=200^3, radius=-1, sigma_d=1, sigma_r=1";
        std::string testDescription = "Should return false because of invalid radius size (negative)";
        TestResult result(suiteName, testName, 2, testDescription);

        //creates a Grayscale workspace of size 200^3
        //first variable is x domain size (in voxels), second is y, third is z. Fourth is voxelLength (in meters)
        puma::Workspace grayWS(200,200,200,1e-6, false);

        //in this case, the return should be false since we passed in a negative radius value
        bool success = puma::filter_Bilateral(&grayWS,-1,1,1);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test3() {

        std::string suiteName = "BilateralFilter_Test";
        std::string testName = "BilateralFilter_Test: size=200^3, radius=0, sigma_d=1, sigma_d=1";
        std::string testDescription = "Should return false because of invalid radius size (0) ";
        TestResult result(suiteName, testName, 3, testDescription);

        //creates a Grayscale workspace of size 200^3
        //first variable is x domain size (in voxels), second is y, third is z. Fourth is voxelLength (in meters)
        puma::Workspace grayWS(200,200,200,1e-6,false);

        //in this case, the return should be false since we passed in a 0 radius value
        bool success = puma::filter_Bilateral(&grayWS,0,1,1);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test4() {

        std::string suiteName = "BilateralFilter_Test";
        std::string testName = "BilateralFilter_Test: size=200^3, radius=1e9, sigma_d=1, sigma_r=1";
        std::string testDescription = "Should return false because of invalid radius size (too large) ";
        TestResult result(suiteName, testName, 4, testDescription);

        //creates a Grayscale workspace of size 200^3
        //first variable is x domain size (in voxels), second is y, third is z. Fourth is voxelLength (in meters)
        puma::Workspace grayWS(200,200,200,1e-6,false);

        //in this case, the return should be false since we passed in too large of a radius value
        bool success = puma::filter_Bilateral(&grayWS,1e9,1,1);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test5() {

        std::string suiteName = "BilateralFilter_Test";
        std::string testName = "BilateralFilter_Test: size=200^3, radius=1, sigma_d=-1, sigma_r=1";
        std::string testDescription = "Should return false because of invalid sigma_d (negative) ";
        TestResult result(suiteName, testName, 5, testDescription);

        //creates a Grayscale workspace of size 200^3
        //first variable is x domain size (in voxels), second is y, third is z. Fourth is voxelLength (in meters)
        puma::Workspace grayWS(200,200,200,1e-6,false);

        //in this case, the return should be false since we passed in too large of a radius value
        bool success = puma::filter_Bilateral(&grayWS,1,-1,1);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test6() {

        std::string suiteName = "BilateralFilter_Test";
        std::string testName = "BilateralFilter_Test: size=200^3, radius=1, sigma_d=201^3, sigma_r=1";
        std::string testDescription = "Should return false because of invalid sigma_d (too large) ";
        TestResult result(suiteName, testName, 6, testDescription);

        //creates a Grayscale workspace of size 200^3
        //first variable is x domain size (in voxels), second is y, third is z. Fourth is voxelLength (in meters)
        puma::Workspace grayWS(200,200,200,1e-6,false);

        //in this case, the return should be false since we passed in too large of a radius value
        bool success = puma::filter_Bilateral(&grayWS,1,201*201*201,1);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test7() {

        std::string suiteName = "BilateralFilter_Test";
        std::string testName = "BilateralFilter_Test: size=200^3, radius=1, sigma_d=1, sigma_r=-1";
        std::string testDescription = "Should return false because of invalid sigma_r (negative) ";
        TestResult result(suiteName, testName, 7, testDescription);

        //creates a Grayscale workspace of size 200^3
        //first variable is x domain size (in voxels), second is y, third is z. Fourth is voxelLength (in meters)
        puma::Workspace grayWS(200,200,200,1e-6,false);

        //in this case, the return should be false since we passed in too large of a radius value
        bool success = puma::filter_Bilateral(&grayWS,1,1,-1);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test8() {

        std::string suiteName = "BilateralFilter_Test";
        std::string testName = "BilateralFilter_Test: size=200^3, radius=1, sigma_d=1, sigma_r=2^17";
        std::string testDescription = "Should return false because of invalid sigma_r (too large) ";
        TestResult result(suiteName, testName, 8, testDescription);

        //creates a Grayscale workspace of size 200^3
        //first variable is x domain size (in voxels), second is y, third is z. Fourth is voxelLength (in meters)
        puma::Workspace grayWS(200,200,200,1e-6,false);

        //in this case, the return should be false since we passed in too large of a radius value
        bool success = puma::filter_Bilateral(&grayWS,1,1,131072);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test9() {

        std::string suiteName = "BilateralFilter_Test";
        std::string testName = "BilateralFilter_Test: size=20^3, radius=3, sigma_d=1, sigma_r=1";
        std::string testDescription = "Should return true because all parameters are valid ";
        TestResult result(suiteName, testName, 9, testDescription);


        //creates a Grayscale workspace of size 200^3
        //first variable is x domain size (in voxels), second is y, third is z. Fourth is voxelLength (in meters)
        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightCircle(100,100,100,4,0,200,0,0,-1,-1,true,0.6,42);
        input.print = false;

        puma::generateRandomFibers(&grayWS,input);

        std::cout << grayWS.X() << std::endl;

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/BilateralFilter_Test5_BeforeFilter.tif",false);

        //in this case, the return should be true since all values are valid
        bool success = puma::filter_Bilateral(&grayWS,4,4,50);


        if(success) {
            std::cout << "Worked" <<std::endl;
        }
        else{
            std::cout << "Didn't work" << std::endl;
        }


        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/BilateralFilter_Test5_AfterFilter.tif",false);

        //making sure that the value in "success" is true.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(true,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test10() {

        std::string suiteName = "BilateralFilter_Test";
        std::string testName = "BilateralFilter_Test: size=1300^3, radius=1, sigma_d=1, sigma_r=1";
        std::string testDescription = "Testing to make sure that the filtering algorithm can work on grayWS.size > 2.1billion voxels ";
        TestResult result(suiteName, testName, 10, testDescription);

        //creates a Grayscale workspace of size 200^3
        //first variable is x domain size (in voxels), second is y, third is z. Fourth is voxelLength (in meters)
        puma::Workspace grayWS(200,200,200,1,1e-6,false);

        //in this case, the return should be true
        bool success = puma::filter_Bilateral(&grayWS,1,1,1);

        //making sure that the value in "success" is true.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(true,success,&result)) {
            return result;
        }
        //making sure that after the filtering, the average value is still 1.


        if(!assertEquals((double)1.0,grayWS.average(),&result)) {
            return result;
        }

        return result;
    }

};
