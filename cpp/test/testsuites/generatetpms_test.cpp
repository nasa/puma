#include "../testframework/subtest.h"
#include "puma.h"

#include <vector>


class Generate_TPMS_Test : public SubTest {
public:

    Generate_TPMS_Test() {

        testSuiteName = "Generate_TPMS_Test";

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
    }


    static TestResult test1() {

        std::string suiteName = "Generate_TPMS_Test";
        std::string testName = "Generate_TPMS_Test: xsize= -200, ysize=200, zsize=200 ";
        std::string testDescription = "Should return false because of invalid xsize (negative)";
        TestResult result(suiteName, testName, 1, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace tpmsWS(1e-6,false);
        TPMSInput input(-200,200,200,1,1,1,1);

        bool success = puma::generateTPMS(&tpmsWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {

        std::string suiteName = "Generate_TPMS_Test";
        std::string testName = "Generate_TPMS_Test: xsize= 200, ysize=-200, zsize=200 ";
        std::string testDescription = "Should return false because of invalid ysize (negative)";
        TestResult result(suiteName, testName, 2, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace tpmsWS(1e-6,false);
        TPMSInput input(200,-200,200,1,1,1,1);

        bool success = puma::generateTPMS(&tpmsWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {

        std::string suiteName = "Generate_TPMS_Test";
        std::string testName = "Generate_TPMS_Test: xsize= 200, ysize=200, zsize=-200 ";
        std::string testDescription = "Should return false because of invalid zsize (negative)";
        TestResult result(suiteName, testName, 3, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace tpmsWS(1e-6,false);
        TPMSInput input(200,200,-200,1,1,1,1);

        bool success = puma::generateTPMS(&tpmsWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test4() {

        std::string suiteName = "Generate_TPMS_Test";
        std::string testName = "Generate_TPMS_Test: xsize= 0 ysize=200, zsize=200 ";
        std::string testDescription = "Should return false because of invalid xsize (zero)";
        TestResult result(suiteName, testName, 4, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace tpmsWS(1e-6,false);
        TPMSInput input(0,200,200,1,1,1,1);

        bool success = puma::generateTPMS(&tpmsWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {

        std::string suiteName = "Generate_TPMS_Test";
        std::string testName = "Generate_TPMS_Test: xsize= 200, ysize=0, zsize=200 ";
        std::string testDescription = "Should return false because of invalid ysize (zero)";
        TestResult result(suiteName, testName, 5, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace tpmsWS(1e-6,false);
        TPMSInput input(200,0,200,1,1,1,1);

        bool success = puma::generateTPMS(&tpmsWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test6() {

        std::string suiteName = "Generate_TPMS_Test";
        std::string testName = "Generate_TPMS_Test: xsize= 200, ysize=200, zsize=0 ";
        std::string testDescription = "Should return false because of invalid zsize (zero)";
        TestResult result(suiteName, testName, 6, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace tpmsWS(1e-6,false);
        TPMSInput input(200,200,0,1,1,1,1);

        bool success = puma::generateTPMS(&tpmsWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test7() {

        std::string suiteName = "Generate_TPMS_Test";
        std::string testName = "Generate_TPMS_Test: xsize= 200, ysize=200, zsize=200, wMin negative ";
        std::string testDescription = "Should return false because of invalid wMin (negative)";
        TestResult result(suiteName, testName, 7, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace tpmsWS(1e-6,false);
        TPMSInput input(200,200,200,-1,1,1,1);

        bool success = puma::generateTPMS(&tpmsWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test8() {

        std::string suiteName = "Generate_TPMS_Test";
        std::string testName = "Generate_TPMS_Test: xsize= 200, ysize=200, zsize=200, wMin > wMax ";
        std::string testDescription = "Should return false because of invalid wMax (below wMin)";
        TestResult result(suiteName, testName, 8, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace tpmsWS(1e-6,false);
        TPMSInput input(200,200,200,1,0.8,1,1);

        bool success = puma::generateTPMS(&tpmsWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test9() {

        std::string suiteName = "Generate_TPMS_Test";
        std::string testName = "Generate_TPMS_Test: xsize= 200, ysize=200, zsize=200, qMin negative ";
        std::string testDescription = "Should return false because of invalid wMin (negative)";
        TestResult result(suiteName, testName, 9, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace tpmsWS(1e-6,false);
        TPMSInput input(200,200,200,1,1,-1,1);

        bool success = puma::generateTPMS(&tpmsWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test10() {

        std::string suiteName = "Generate_TPMS_Test";
        std::string testName = "Generate_TPMS_Test: xsize= 200, ysize=200, zsize=200, qMin > qMax ";
        std::string testDescription = "Should return false because of invalid wMax (below wMin)";
        TestResult result(suiteName, testName, 10, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace tpmsWS(1e-6,false);
        TPMSInput input(200,200,200,1,1,1,0.8);

        bool success = puma::generateTPMS(&tpmsWS, input);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test11() {

        std::string suiteName = "Generate_TPMS_Test";
        std::string testName = "Generate_TPMS_Test: xsize= 200, ysize=200, zsize=200, qMin > qMax ";
        std::string testDescription = "Should return false because of invalid wMax (below wMin)";
        TestResult result(suiteName, testName, 11, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace tpmsWS(1e-6,false);
        TPMSInput input(200,200,200,0.05,0.05,0.201,0.201);
        bool success = puma::generateTPMS(&tpmsWS, input);

        puma::export_STL(&tpmsWS, puma::Cutoff(0,127), false, "cpp/test/out/stl/tpms_test.stl");
        puma::export_3DTiff(&tpmsWS, "cpp/test/out/stl/tpms_test.tif", false);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(true,success,&result)) {
            return result;
        }

        return result;
    }





};

