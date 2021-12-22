#include "../testframework/subtest.h"
#include "puma.h"

#include <vector>


class PoreSpace_Test : public SubTest {
public:

    PoreSpace_Test() {

        testSuiteName = "PoreSpace_Test";

        tests.push_back(test1);
        tests.push_back(test2);
        tests.push_back(test3);
        tests.push_back(test4);
        tests.push_back(test5);
        tests.push_back(test6);
        tests.push_back(test7);

    }


    static TestResult test1() {

        std::string suiteName = "PoreSpace_Test";
        std::string testName = "PoreSpace_Test: 1";
        std::string testDescription = "Should return false because of invalid workspace size (empty)";
        TestResult result(suiteName, testName, 1, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace ws(1e-6,false);

        puma::Matrix<long> poreMatrix;
        bool success = puma::identifyPoreSpace(&ws,&poreMatrix,puma::Cutoff(0,127));

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {

        std::string suiteName = "PoreSpace_Test";
        std::string testName = "PoreSpace_Test: 2 ";
        std::string testDescription = "Should return false because of invalid workspace size (empty)";
        TestResult result(suiteName, testName, 2, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace ws(1e-6,false);

        bool success = puma::fillClosedPores(&ws,puma::Cutoff(0,127), 255);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {

        std::string suiteName = "PoreSpace_Test";
        std::string testName = "PoreSpace_Test: 3";
        std::string testDescription = "Should return false because of invalid Cutoff";
        TestResult result(suiteName, testName, 3, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace ws(100,100,100,1e-6, false);

        puma::Matrix<long> poreMatrix;
        bool success = puma::identifyPoreSpace(&ws,&poreMatrix,puma::Cutoff(-1,1));
        //making sure that the value in "success" is false.
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        success = puma::identifyPoreSpace(&ws,&poreMatrix,puma::Cutoff(10,1));
        //making sure that the value in "success" is false.
        if(!assertEquals(false,success,&result)) {
            return result;
        }


        return result;
    }

    static TestResult test4() {

        std::string suiteName = "PoreSpace_Test";
        std::string testName = "PoreSpace_Test: 4";
        std::string testDescription = "Should return false because of invalid Cutoff";
        TestResult result(suiteName, testName, 4, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace ws(100,100,100,1e-6, false);

        bool success = puma::fillClosedPores(&ws,puma::Cutoff(-1,1), 255);
        //making sure that the value in "success" is false.
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        success = puma::fillClosedPores(&ws,puma::Cutoff(10,1), 255);
        //making sure that the value in "success" is false.
        if(!assertEquals(false,success,&result)) {
            return result;
        }


        return result;
    }

    static TestResult test5() {

        std::string suiteName = "PoreSpace_Test";
        std::string testName = "PoreSpace_Test: 5";
        std::string testDescription = "Should return false because of invalid Cutoff";
        TestResult result(suiteName, testName, 5, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace ws(100,100,100,1e-6, false);

        bool success = puma::fillClosedPores(&ws,puma::Cutoff(0,127), -1);
        //making sure that the value in "success" is false.
        if(!assertEquals(false,success,&result)) {
            return result;
        }


        return result;
    }



    static TestResult test6() {

        std::string suiteName = "PoreSpace_Test";
        std::string testName = "PoreSpace_Test: 6";
        std::string testDescription = "Benchmark test 1";
        TestResult result(suiteName, testName, 6, testDescription);

        puma::Workspace grayWS(10,10,1,1e-6);

        RandomFibersInput input;
        input.straightFlower_Hollow(200,200,1, 30, 0, 1, 0, 2, -1, 2, true, 0.6, 2, 1, 0, 0, 0, 0, 1, 20, 0);
        generateRandomFibers(&grayWS,input);


        puma::Matrix<long> poreMatrix;
        puma::identifyPoreSpace(&grayWS,&poreMatrix,puma::Cutoff(0,127));

        double average =  poreMatrix.average();

        if(!assertEquals(3.66542, average, 0.01, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test7() {

        std::string suiteName = "PoreSpace_Test";
        std::string testName = "PoreSpace_Test: 7";
        std::string testDescription = "Benchmark test 2";
        TestResult result(suiteName, testName, 7, testDescription);

        puma::Workspace grayWS(10,10,1,1e-6);

        RandomFibersInput input;
        input.straightFlower_Hollow(200,200,1, 30, 0, 1, 0, 2, -1, 2, true, 0.6, 2, 1, 0, 0, 0, 0, 1, 20, 0);
        generateRandomFibers(&grayWS,input);

        puma::fillClosedPores(&grayWS, puma::Cutoff(0,127), 255);

        grayWS.setMaterialID(puma::Cutoff(0,127), 0);
        grayWS.setMaterialID(puma::Cutoff(128,256), 1);

        double average = grayWS.matrix.average() ;

        if(!assertEquals(0.80765, average, 0.01, &result)) {
            return result;
        }

        return result;
    }




};

