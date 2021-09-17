#include "../testframework/subtest.h"
#include "puma.h"

#include <map>


class EJElectricalConductivity_Test : public SubTest {
public:

    EJElectricalConductivity_Test() {
        testSuiteName = "EJElectricalConductivity_Test";

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
        tests.push_back(test14);
        tests.push_back(test15);
        tests.push_back(test16);
        tests.push_back(test17);
        tests.push_back(test18);
        tests.push_back(test19);
        tests.push_back(test20);
        tests.push_back(test21);
        tests.push_back(test22);
        tests.push_back(test23);
        tests.push_back(test24);
        tests.push_back(test25);
        tests.push_back(test26);
        tests.push_back(test27);
        tests.push_back(test28);
        tests.push_back(test29);
        tests.push_back(test30);
        tests.push_back(test31);
        tests.push_back(test32);

    }

    static TestResult test1() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 1 - 100x100x100, k = 0.1, x direction";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 1, testDescription);

        puma::Workspace segWS(100,100,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = .1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'x',1e-3,10000,true,10);

        if(!assertEquals(0.1,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 2 - 100x100x100, k = 0.1, y direction";
        std::string testDescription = "should return 0.1 in y direction and 0 in other directions";
        TestResult result(suiteName, testName, 2, testDescription);

        puma::Workspace segWS(100,100,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'y',1e-3,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.1,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 3 - 100x100x100, k = 0.1, z direction";
        std::string testDescription = "should return 0.1 in z direction and 0 in other directions";
        TestResult result(suiteName, testName, 3, testDescription);

        puma::Workspace segWS(100,100,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'z',1e-3,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.1,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test4() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 4 - 100x100x1, k = 0.1, x direction";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 4, testDescription);

        puma::Workspace segWS(100,100,1,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'x',1e-3,10000,false,10);

        if(!assertEquals(0.1,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 5 - 100x1x100, k = 0.1, x direction";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 5, testDescription);

        puma::Workspace segWS(100,1,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'x',1e-3,10000,false,10);

        if(!assertEquals(0.1,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test6() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 6 - 1x100x100, k = 0.1, x direction";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 6, testDescription);

        puma::Workspace segWS(1,100,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'x',1e-3,10000,false,10);

        if(!assertEquals(0.1,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test7() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 7 - 100x100x1, k = 0.1, y direction";
        std::string testDescription = "should return 0.1 in y direction and 0 in other directions";
        TestResult result(suiteName, testName, 7, testDescription);

        puma::Workspace segWS(100,100,1,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'y',1e-3,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.1,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test8() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 8 - 100x1x100, k = 0.1, y direction";
        std::string testDescription = "should return 0.1 in y direction and 0 in other directions";
        TestResult result(suiteName, testName, 8, testDescription);

        puma::Workspace segWS(100,1,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'y',1e-3,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.1,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test9() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 9 - 1x100x100, k = 0.1, y direction";
        std::string testDescription = "should return 0.1 in y direction and 0 in other directions";
        TestResult result(suiteName, testName, 9, testDescription);

        puma::Workspace segWS(1,100,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'y',1e-3,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.1,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test10() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 10 - 100x100x1, k = 0.1, z direction";
        std::string testDescription = "should return 0.1 in z direction and 0 in other directions";
        TestResult result(suiteName, testName, 10, testDescription);

        puma::Workspace segWS(100,100,1,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'z',1e-3,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.1,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test11() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 11 - 100x1x100, k = 0.1, z direction";
        std::string testDescription = "should return 0.1 in y direction and 0 in other directions";
        TestResult result(suiteName, testName, 11, testDescription);

        puma::Workspace segWS(100,1,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'z',1e-3,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.1,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test12() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 12 - 1x100x100, k = 0.1, z direction";
        std::string testDescription = "should return 0.1 in z direction and 0 in other directions";
        TestResult result(suiteName, testName, 12, testDescription);

        puma::Workspace segWS(1,100,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'z',1e-3,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.1,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test13() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 13 - 100x1x1, k = 0.1, x direction";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 13, testDescription);

        puma::Workspace segWS(100,1,1,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'x',1e-3,10000,false,10);

        if(!assertEquals(0.1,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test14() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 14 - 1x100x1, k = 0.1, x direction";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 14, testDescription);

        puma::Workspace segWS(1,100,1,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'x',1e-3,10000,false,10);

        if(!assertEquals(0.1,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test15() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 15 - 1x1x100, k = 0.1, x direction";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 15, testDescription);

        puma::Workspace segWS(1,1,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'x',1e-3,10000,false,10);

        if(!assertEquals(0.1,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test16() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 16 - 100x1x1, k = 0.1, y direction";
        std::string testDescription = "should return 0.1 in y direction and 0 in other directions";
        TestResult result(suiteName, testName, 16, testDescription);

        puma::Workspace segWS(100,1,1,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'y',1e-3,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.1,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test17() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 17 - 1x100x1, k = 0.1, y direction";
        std::string testDescription = "should return 0.1 in y direction and 0 in other directions";
        TestResult result(suiteName, testName, 17, testDescription);

        puma::Workspace segWS(1,100,1,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'y',1e-3,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.1,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test18() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 18 - 1x1x100, k = 0.1, y direction";
        std::string testDescription = "should return 0.1 in y direction and 0 in other directions";
        TestResult result(suiteName, testName, 18, testDescription);

        puma::Workspace segWS(1,1,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'y',1e-3,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.1,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test19() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 19 - 100x1x1, k = 0.1, z direction";
        std::string testDescription = "should return 0.1 in z direction and 0 in other directions";
        TestResult result(suiteName, testName, 19, testDescription);

        puma::Workspace segWS(100,1,1,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'z',1e-3,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.1,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test20() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 20 - 1x100x1, k = 0.1, z direction";
        std::string testDescription = "should return 0.1 in y direction and 0 in other directions";
        TestResult result(suiteName, testName, 20, testDescription);

        puma::Workspace segWS(1,100,1,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'z',1e-3,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.1,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test21() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 21 - 1x1x100, k = 0.1, z direction";
        std::string testDescription = "should return 0.1 in z direction and 0 in other directions";
        TestResult result(suiteName, testName, 21, testDescription);

        puma::Workspace segWS(1,1,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'z',1e-3,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.1,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test22() {
        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 22 - 100x100x100, two materials in series (k=1 and k=10), x direction";
        std::string testDescription = "should return 1.8181818 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 22, testDescription);

        puma::Workspace segWS(100,100,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        segWS.matrix.set(50,99,0,99,0,99,1);

        matCond[0] = 1;
        matCond[1] = 10;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'x',1e-6,1000,false,10);

        if(!assertEquals(1.8181818,k.x, 0.001, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test23() {
        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 23 - 100x100x100, two materials in parallel (k=1 and k=10), x direction";
        std::string testDescription = "should return 5.5 in x diriection and 0 in other directions";
        TestResult result(suiteName, testName, 23, testDescription);

        puma::Workspace segWS(100,100,100,0,1e-6,false);
        puma::Matrix<double> T(100,100,100);
        std::map<int, double> matCond;
        segWS.matrix.set(0,99,50,99,0,99,1);
        matCond[0] = 1;
        matCond[1] = 10;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'x',1e-6,10000,false,10);

        if(!assertEquals(5.5,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test24() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 24 - Empty Matrix";
        std::string testDescription = "should return -1's";
        TestResult result(suiteName, testName, 24, testDescription);

        puma::Workspace segWS(1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'x',1e-3,10000,false,10);

        if(!assertEquals(-1.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(-1.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(-1.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test25() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 25 - Negative Conductivity";
        std::string testDescription = "should return -1's";
        TestResult result(suiteName, testName, 25, testDescription);

        puma::Workspace segWS(100,100,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = -0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'y',1e-3,10000,false,10);

        if(!assertEquals(-1.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(-1.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(-1.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test26() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 26 - Invalid Direction";
        std::string testDescription = "should return -1's";
        TestResult result(suiteName, testName, 26, testDescription);

        puma::Workspace segWS(100,100,100,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'w',1e-3,10000,false,10);

        if(!assertEquals(-1.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(-1.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(-1.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test27() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 27 - One Cell";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 27, testDescription);

        puma::Workspace segWS(1,1,1,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.1;
        matCond[1] = 0.9;

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'x',1e-3,10000,false,10);

        if(!assertEquals(0.1,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test28() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 28 - Multiple materials in series";
        std::string testDescription;
        TestResult result(suiteName, testName, 28, testDescription);

        puma::Workspace segWS(100,57,62,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;

        segWS.matrix.set(10,19,0,56,0,61,1);
        segWS.matrix.set(20,29,0,56,0,61,2);
        segWS.matrix.set(30,39,0,56,0,61,3);
        segWS.matrix.set(40,49,0,56,0,61,4);
        segWS.matrix.set(50,59,0,56,0,61,5);
        segWS.matrix.set(60,69,0,56,0,61,6);
        segWS.matrix.set(70,79,0,56,0,61,7);
        segWS.matrix.set(80,89,0,56,0,61,8);
        segWS.matrix.set(90,99,0,56,0,61,9);

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

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'x',1e-4,10000,false,10);

        if(!assertEquals(3.414171,k.x, 0.01, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test29() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 29 - Multiple materials in parallel";
        std::string testDescription;
        TestResult result(suiteName, testName, 29, testDescription);

        puma::Workspace segWS(100,57,62,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;

        segWS.matrix.set(10,19,0,56,0,61,1);
        segWS.matrix.set(20,29,0,56,0,61,2);
        segWS.matrix.set(30,39,0,56,0,61,3);
        segWS.matrix.set(40,49,0,56,0,61,4);
        segWS.matrix.set(50,59,0,56,0,61,5);
        segWS.matrix.set(60,69,0,56,0,61,6);
        segWS.matrix.set(70,79,0,56,0,61,7);
        segWS.matrix.set(80,89,0,56,0,61,8);
        segWS.matrix.set(90,99,0,56,0,61,9);

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

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'y',1e-4,10000,false,10);

        if(!assertEquals(0.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(5.5,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test30() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 30 - Multiple materials in parallel";
        std::string testDescription;
        TestResult result(suiteName, testName, 30, testDescription);

        puma::Workspace segWS(100,57,62,0,1e-6,false);
        puma::Matrix<double> T;
        std::map<int, double> matCond;

        segWS.matrix.set(10,19,0,56,0,61,1);
        segWS.matrix.set(20,29,0,56,0,61,2);
        segWS.matrix.set(30,39,0,56,0,61,3);
        segWS.matrix.set(40,49,0,56,0,61,4);
        segWS.matrix.set(50,59,0,56,0,61,5);
        segWS.matrix.set(60,69,0,56,0,61,6);
        segWS.matrix.set(70,79,0,56,0,61,7);
        segWS.matrix.set(80,89,0,56,0,61,8);
        segWS.matrix.set(90,99,0,56,0,61,9);

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

        puma::Vec3<double> k = puma::compute_EJElectricalConductivity(&segWS, &T, matCond,'z',1e-4,10000,false,10);

        if(!assertEquals(0.,k.x, 0.001, &result)) {
            return result;
        }
        if(!assertEquals(0.,k.y, &result)) {
            return result;
        }
        if(!assertEquals(5.5,k.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test31() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 31 - Segmented FiberForm";
        std::string testDescription;
        TestResult result(suiteName, testName, 31, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"));

        grayWS.setMaterialID(&grayWS,puma::Cutoff(0,89),0);
        grayWS.setMaterialID(&grayWS,puma::Cutoff(90,255),1);

        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[0] = 0.0257;
        matCond[1] = 12;

        puma::Vec3<double> kx = puma::compute_EJElectricalConductivity(&grayWS, &T, matCond,'x',1e-4,10000,false,10);

        if(!assertEquals(0.460509,kx.x, 0.05, &result)) {
            return result;
        }

        puma::Vec3<double> ky = puma::compute_EJElectricalConductivity(&grayWS, &T, matCond,'y',1e-4,10000,false,10);

        if(!assertEquals(0.780620,ky.y, 0.05, &result)) {
            return result;
        }

        puma::Vec3<double> kz = puma::compute_EJElectricalConductivity(&grayWS, &T, matCond,'z',1e-4,10000,false,10);

        if(!assertEquals(0.337910,kz.z, 0.05, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test32() {

        std::string suiteName = "EJElectricalConductivity_Test";
        std::string testName = "EJElectricalConductivity_Test: Test 32 - Grayscale FiberForm";
        std::string testDescription;
        TestResult result(suiteName, testName, 32, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"));

        puma::Matrix<double> T;
        std::map<int, double> matCond;
        matCond[89] = 0.0257;
        matCond[255] = 12;

        puma::Vec3<double> kx = puma::compute_EJElectricalConductivity(&grayWS, &T, matCond,'x',1e-4,10000,false,10);

        if(!assertEquals(0.460509,kx.x, 0.05, &result)) {
            return result;
        }

        puma::Vec3<double> ky = puma::compute_EJElectricalConductivity(&grayWS, &T, matCond,'y',1e-4,10000,false,10);

        if(!assertEquals(0.780620,ky.y, 0.05, &result)) {
            return result;
        }

        puma::Vec3<double> kz = puma::compute_EJElectricalConductivity(&grayWS, &T, matCond,'z',1e-4,10000,false,10);

        if(!assertEquals(0.337910,kz.z, 0.05, &result)) {
            return result;
        }

        return result;
    }



};
