#include "../testframework/subtest.h"
#include "puma.h"


class MeanFilter3D_Test : public SubTest {
public:

    MeanFilter3D_Test() {

        testSuiteName = "MeanFilter3D_Test";

        tests.push_back(test1);
        tests.push_back(test2);
        tests.push_back(test3);
        tests.push_back(test4);
        tests.push_back(test5);
        tests.push_back(test6);
        tests.push_back(test7);
        tests.push_back(test8);
    }


    static TestResult test1() {

        std::string suiteName = "MeanFilter3D_Test";
        std::string testName = "MeanFilter3D_Test: size=0, radius=3";
        std::string testDescription = "Should return false because of invalid Workspace size";
        TestResult result(suiteName, testName, 1, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace grayWS(1e-6,false);

        //in this case, the return should be false since we passed in an empty Grayscale workspace. Also becuase 3>domain size
        bool success = puma::filter_Mean3D(&grayWS,3);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test2() {

        std::string suiteName = "MeanFilter3D_Test";
        std::string testName = "MeanFilter3D_Test: size=200^3, radius=-1";
        std::string testDescription = "Should return false because of invalid radius size (negative)";
        TestResult result(suiteName, testName, 2, testDescription);

        //creates a Grayscale workspace of size 200^3
        //first variable is x domain size (in voxels), second is y, third is z. Fourth is voxelLength (in meters)
        puma::Workspace grayWS(200,200,200,1e-6,false);

        //in this case, the return should be false since we passed in a negative radius value
        bool success = puma::filter_Mean3D(&grayWS,-1);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test3() {

        std::string suiteName = "MeanFilter3D_Test";
        std::string testName = "MeanFilter3D_Test: size=200^3, radius=0";
        std::string testDescription = "Should return false because of invalid radius size (0) ";
        TestResult result(suiteName, testName, 3, testDescription);

        //creates a Grayscale workspace of size 200^3
        //first variable is x domain size (in voxels), second is y, third is z. Fourth is voxelLength (in meters)
        puma::Workspace grayWS(200,200,200,1e-6,false);

        //in this case, the return should be false since we passed in a 0 radius value
        bool success = puma::filter_Mean3D(&grayWS,0);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test4() {

        std::string suiteName = "MeanFilter3D_Test";
        std::string testName = "MeanFilter3D_Test: size=200^3, radius=1e9";
        std::string testDescription = "Should return false because of invalid radius size (too large) ";
        TestResult result(suiteName, testName, 4, testDescription);

        //creates a Grayscale workspace of size 200^3
        //first variable is x domain size (in voxels), second is y, third is z. Fourth is voxelLength (in meters)
        puma::Workspace grayWS(200,200,200,1e-6,false);

        //in this case, the return should be false since we passed in too large of a radius value
        bool success = puma::filter_Mean3D(&grayWS,1e9);

        //making sure that the value in "success" is false.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test5() {

        std::string suiteName = "MeanFilter3D_Test";
        std::string testName = "MeanFilter3D_Test: size=200^3, radius=3";
        std::string testDescription = "Should return true because all parameters are valid ";
        TestResult result(suiteName, testName, 5, testDescription);

        //creates a Grayscale workspace of size 200^3
        //first variable is x domain size (in voxels), second is y, third is z. Fourth is voxelLength (in meters)
        puma::Workspace grayWS(200,200,200,1,1e-6,false);

        //in this case, the return should be true since all values are valid
        bool success = puma::filter_Mean3D(&grayWS,3);

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


    static TestResult test6() {

        std::string suiteName = "MeanFilter3D_Test";
        std::string testName = "MeanFilter3D_Test: size=1300^3, radius=1";
        std::string testDescription = "Testing to make sure that the filtering algorithm can work on grayWS.size > 2.1billion voxels ";
        TestResult result(suiteName, testName, 6, testDescription);

        //creates a Grayscale workspace of size 200^3
        //first variable is x domain size (in voxels), second is y, third is z. Fourth is voxelLength (in meters)
        puma::Workspace grayWS(200,200,200,1,1e-6,false);

        //in this case, the return should be true
        bool success = puma::filter_Mean3D(&grayWS,1);

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


    static TestResult test7() {

        std::string suiteName = "MeanFilter3D_Test";
        std::string testName = "MeanFilter3D_Test: test case #1 on 5x5x1. Radius=1";
        std::string testDescription = "Testing to make sure that the filtering algorithm gives the right result on a sample 5x5x1 workspace ";
        TestResult result(suiteName, testName, 7, testDescription);

        //creating a Grayscale workspace with predefined values for testing
        puma::Workspace actual(5,5,1,1e-6,false);
        actual.matrix(0,0,0) = 15;
        actual.matrix(1,0,0) = 16;
        actual.matrix(2,0,0) = 6;
        actual.matrix(3,0,0) = 17;
        actual.matrix(4,0,0) = 18;
        actual.matrix(0,1,0) = 14;
        actual.matrix(1,1,0) = 10;
        actual.matrix(2,1,0) = 7;
        actual.matrix(3,1,0) = 11;
        actual.matrix(4,1,0) = 19;
        actual.matrix(0,2,0) = 1;
        actual.matrix(1,2,0) = 2;
        actual.matrix(2,2,0) = 3;
        actual.matrix(3,2,0) = 4;
        actual.matrix(4,2,0) = 5;
        actual.matrix(0,3,0) = 25;
        actual.matrix(1,3,0) = 12;
        actual.matrix(2,3,0) = 8;
        actual.matrix(3,3,0) = 13;
        actual.matrix(4,3,0) = 20;
        actual.matrix(0,4,0) = 24;
        actual.matrix(1,4,0) = 23;
        actual.matrix(2,4,0) = 9;
        actual.matrix(3,4,0) = 22;
        actual.matrix(4,4,0) = 21;

        //the correct values of the mean filter, computed by hand
        puma::Workspace expected(5,5,1,1e-6,false);
        expected.matrix(0,0,0) = 13;
        expected.matrix(1,0,0) = 11;
        expected.matrix(2,0,0) = 11;
        expected.matrix(3,0,0) = 13;
        expected.matrix(4,0,0) = 16;
        expected.matrix(0,1,0) = 9;
        expected.matrix(1,1,0) = 8;
        expected.matrix(2,1,0) = 8;
        expected.matrix(3,1,0) = 10;
        expected.matrix(4,1,0) = 12;
        expected.matrix(0,2,0) = 10;
        expected.matrix(1,2,0) = 9;
        expected.matrix(2,2,0) = 7;
        expected.matrix(3,2,0) = 10;
        expected.matrix(4,2,0) = 12;
        expected.matrix(0,3,0) = 14;
        expected.matrix(1,3,0) = 11;
        expected.matrix(2,3,0) = 10;
        expected.matrix(3,3,0) = 11;
        expected.matrix(4,3,0) = 14;
        expected.matrix(0,4,0) = 21;
        expected.matrix(1,4,0) = 16;
        expected.matrix(2,4,0) = 14;
        expected.matrix(3,4,0) = 15;
        expected.matrix(4,4,0) = 19;

        //in this case, the return should be true
        bool success = puma::filter_Mean3D(&actual,1);

        //making sure that the value in "success" is true.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(true,success,&result)) {
            return result;
        }

        //comparing the expected result with the actual result
        if(!myAssertEquals(&expected,&actual,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test8() {

        std::string suiteName = "MeanFilter3D_Test";
        std::string testName = "MeanFilter3D_Test: test case #1 on 5x5x1. Radius=2";
        std::string testDescription = "Testing to make sure that the filtering algorithm gives the right result on a sample 5x5x1 workspace ";
        TestResult result(suiteName, testName, 8, testDescription);

        //creating a Grayscale workspace with predefined values for testing
        puma::Workspace actual(5,5,1,1e-6,false);
        actual.matrix(0,0,0) = 15;
        actual.matrix(1,0,0) = 16;
        actual.matrix(2,0,0) = 6;
        actual.matrix(3,0,0) = 17;
        actual.matrix(4,0,0) = 18;
        actual.matrix(0,1,0) = 14;
        actual.matrix(1,1,0) = 10;
        actual.matrix(2,1,0) = 7;
        actual.matrix(3,1,0) = 11;
        actual.matrix(4,1,0) = 19;
        actual.matrix(0,2,0) = 1;
        actual.matrix(1,2,0) = 2;
        actual.matrix(2,2,0) = 3;
        actual.matrix(3,2,0) = 4;
        actual.matrix(4,2,0) = 5;
        actual.matrix(0,3,0) = 25;
        actual.matrix(1,3,0) = 12;
        actual.matrix(2,3,0) = 8;
        actual.matrix(3,3,0) = 13;
        actual.matrix(4,3,0) = 20;
        actual.matrix(0,4,0) = 24;
        actual.matrix(1,4,0) = 23;
        actual.matrix(2,4,0) = 9;
        actual.matrix(3,4,0) = 22;
        actual.matrix(4,4,0) = 21;

        //the correct values of the mean filter, computed by hand
        puma::Workspace expected(5,5,1,1e-6,false);
        expected.matrix(0,0,0) = 8;
        expected.matrix(1,0,0) = 8;
        expected.matrix(2,0,0) = 9;
        expected.matrix(3,0,0) = 9;
        expected.matrix(4,0,0) = 10;
        expected.matrix(0,1,0) = 9;
        expected.matrix(1,1,0) = 10;
        expected.matrix(2,1,0) = 11;
        expected.matrix(3,1,0) = 10;
        expected.matrix(4,1,0) = 10;
        expected.matrix(0,2,0) = 11;
        expected.matrix(1,2,0) = 12;
        expected.matrix(2,2,0) = 13;
        expected.matrix(3,2,0) = 12;
        expected.matrix(4,2,0) = 12;
        expected.matrix(0,3,0) = 11;
        expected.matrix(1,3,0) = 11;
        expected.matrix(2,3,0) = 12;
        expected.matrix(3,3,0) = 11;
        expected.matrix(4,3,0) = 11;
        expected.matrix(0,4,0) = 11;
        expected.matrix(1,4,0) = 12;
        expected.matrix(2,4,0) = 12;
        expected.matrix(3,4,0) = 11;
        expected.matrix(4,4,0) = 11;

        //in this case, the return should be true
        bool success = puma::filter_Mean3D(&actual,2);

        //making sure that the value in "success" is true.
        //in assert equals, first value is expected, second value is actual, and third is the testResult
        if(!assertEquals(true,success,&result)) {
            return result;
        }

        //comparing the expected result with the actual result
        if(!myAssertEquals(&expected,&actual,&result)) {
            return result;
        }

        return result;
    }


    // custom assertEquals to compare two Grayscale workspaces
    static bool myAssertEquals(puma::Workspace *valExpected, puma::Workspace *valActual, TestResult *result) {

        //setting initial value for test success
        result->failed = 0;

        //if the two workspaces do not have the same size
        if(valExpected->size() != valActual->size() ){
            result->expected="Size = ";
            result->expected.append(toString::convert(valExpected->size()));
            result->actual="Size = ";
            result->actual.append(toString::convert(valActual->size()));

            //letting the testResult know that the test failed
            result->failed = 1;

            //returning the failed test result
            return !result->failed;
        }

        if(valExpected->X() != valActual->X() ){
            result->expected="X = ";
            result->expected.append(toString::convert(valExpected->X()));
            result->actual="X = ";
            result->actual.append(toString::convert(valActual->X()));

            //letting the testResult know that the test failed
            result->failed = 1;

            //returning the failed test result
            return !result->failed;
        }

        if(valExpected->Y() != valActual->Y() ){
            result->expected="Y = ";
            result->expected.append(toString::convert(valExpected->Y()));
            result->actual="Y = ";
            result->actual.append(toString::convert(valActual->Y()));

            //letting the testResult know that the test failed
            result->failed = 1;

            //returning the failed test result
            return !result->failed;
        }

        if(valExpected->Z() != valActual->Z() ){
            result->expected="Z = ";
            result->expected.append(toString::convert(valExpected->Z()));
            result->actual="Z = ";
            result->actual.append(toString::convert(valActual->Z()));

            //letting the testResult know that the test failed
            result->failed = 1;

            //returning the failed test result
            return !result->failed;
        }

        for(long i=0;i<valExpected->X();i++) {
            for(long j=0;j<valExpected->Y();j++) {
                for(long k=0;k<valExpected->Z();k++) {
                    if( valExpected->matrix(i,j,k) != valActual->matrix(i,j,k) ) {

                        //setting the expected result string
                        result->expected="At (";
                        result->expected.append(toString::convert(i));
                        result->expected.append(",");
                        result->expected.append(toString::convert(j));
                        result->expected.append(",");
                        result->expected.append(toString::convert(k));
                        result->expected.append(") =");
                        result->expected.append(toString::convert(valExpected->operator ()(i,j,k)));

                        //setting the actual result string
                        result->actual="At (";
                        result->actual.append(toString::convert(i));
                        result->actual.append(",");
                        result->actual.append(toString::convert(j));
                        result->actual.append(",");
                        result->actual.append(toString::convert(k));
                        result->actual.append(") =");
                        result->actual.append(toString::convert(valActual->operator ()(i,j,k)));

                        //letting the testResult know that the test failed
                        result->failed = 1;

                        //returning the failed test result
                        return !result->failed;
                    }
                }
            }
        }

        return !result->failed;
    }


};
