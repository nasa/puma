#include "../testframework/subtest.h"
#include "puma.h"


class VolumeFraction_Test : public SubTest {
public:

    VolumeFraction_Test() {
        testSuiteName = "VolumeFraction_test";

        tests.push_back(test1);
        tests.push_back(test2);
        tests.push_back(test3);
        tests.push_back(test4);
        tests.push_back(test5);
        tests.push_back(test6);
        tests.push_back(test7);
        //tests.push_back(test8);
        //tests.push_back(test9);
        //tests.push_back(test10);
        tests.push_back(test11);
        tests.push_back(test12);
        tests.push_back(test13);
        tests.push_back(test14);
        tests.push_back(test15);
        tests.push_back(test16);
        //tests.push_back(test17);
        //tests.push_back(test18);
        //tests.push_back(test19);

    }

    static TestResult test1() {

        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 1 - (Empty matrix)";
        std::string testDescription = "(Test on an empty matrix. Making sure that it returns -1)";
        TestResult result(suiteName, testName, 1, testDescription);

        puma::Workspace grayWS(1e-6,false);

        double vf = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(0,0));

        if(!assertEquals((double)-1,vf, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 2 - VF of 1 for range [0,0]";
        std::string testDescription;
        TestResult result(suiteName, testName, 2, testDescription);

        puma::Workspace grayWS(1e-6,false);
        grayWS.matrix.resize(100,100,100,0);

        double vf = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(0,0));

        if(!assertEquals((double)1,vf, &result)) {
            return result;
        }

        double vf2 = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(-1,0));

        if(!assertEquals((double)-1,vf2, &result)) {
            return result;
        }

        double vf3 = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(0,-1));

        if(!assertEquals((double)-1,vf3, &result)) {
            return result;
        }

        double vf4 = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(32768,0));

        if(!assertEquals((double)-1,vf4, &result)) {
            return result;
        }

        double vf5 = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(0,32768));

        if(!assertEquals((double)-1,vf5, &result)) {
            return result;
        }

        double vf6 = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(10,5));

        if(!assertEquals((double)-1,vf6, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 3 - VF of 0 for range [1,1]";
        std::string testDescription;
        TestResult result(suiteName, testName, 3, testDescription);

        puma::Workspace grayWS(1e-6,false);
        grayWS.matrix.resize(100,100,100,0);

        double vf = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(1,1));

        if(!assertEquals((double)0,vf, &result)) {
            return result;
        }

        vf = puma::compute_VolumeFraction(&grayWS,1,1);
        if(!assertEquals((double)0,vf, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test4() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 4 - VF of 1 for range [0,1]";
        std::string testDescription;
        TestResult result(suiteName, testName, 4, testDescription);

        puma::Workspace grayWS(1e-6,false);
        grayWS.matrix.resize(100,100,100,0);

        double vf = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(0,1));

        if(!assertEquals((double)1,vf, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 5 - VF of 0.5 for range [0,0]";
        std::string testDescription;
        TestResult result(suiteName, testName, 5, testDescription);

        puma::Workspace grayWS(1e-6,false);
        grayWS.matrix.resize(100,100,100,0);
        grayWS.matrix.set(0,99,0,99,0,49,1);

        double vf = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(0,0));

        if(!assertEquals((double)0.5,vf, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test6() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 6 - VF of 1 for range [0,1]";
        std::string testDescription;
        TestResult result(suiteName, testName, 6, testDescription);

        puma::Workspace grayWS(1e-6,false);
        grayWS.matrix.resize(100,100,100,0);
        grayWS.matrix.set(0,99,0,99,0,49,1);

        double vf = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(0,1));

        if(!assertEquals((double)1,vf, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test7() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 7 - VF of 0 for range [2,255]";
        std::string testDescription;
        TestResult result(suiteName, testName, 7, testDescription);

        puma::Workspace grayWS(1e-6,false);
        grayWS.matrix.resize(100,100,100,0);
        grayWS.matrix.set(0,99,0,99,0,49,1);

        double vf = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(2,255));

        if(!assertEquals((double)0,vf, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test8() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 8 - VF of 0.5 for range [0,1] > 1300^3";
        std::string testDescription;
        TestResult result(suiteName, testName, 8, testDescription);

        puma::Workspace grayWS(1e-6,false);
        grayWS.matrix.resize(1300,1300,1300,0);
        grayWS.matrix.set(0,1299,0,1299,0,649,2);

        double vf = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(0,1));

        if(!assertEquals((double)0.5,vf, &result)) {
            return result;
        }


        vf = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(0,1));

        if(!assertEquals((double)0.5,vf, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test9() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 9 - VF of 1 for range [0,2] > 1300^3";
        std::string testDescription;
        TestResult result(suiteName, testName, 9, testDescription);

        puma::Workspace grayWS(1e-6,false);
        grayWS.matrix.resize(1300,1300,1300,0);
        grayWS.matrix.set(0,1299,0,1299,0,649,2);

        double vf = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(0,2));

        if(!assertEquals((double)1,vf, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test10() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 10 - VF of 0 for range [3,20] > 1300^3";
        std::string testDescription;
        TestResult result(suiteName, testName, 10, testDescription);

        puma::Workspace grayWS(1e-6,false);
        grayWS.matrix.resize(1300,1300,1300,0);
        grayWS.matrix.set(0,1299,0,1299,0,649,2);

        double vf = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(3,20));

        if(!assertEquals((double)0,vf, &result)) {
            return result;
        }



        return result;
    }

    static TestResult test11() {

        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 11 - (Empty matrix)";
        std::string testDescription = "(Test on an empty matrix. Making sure that it returns -1)";
        TestResult result(suiteName, testName, 11, testDescription);

        puma::Workspace material(1e-6,false);

        double vf = puma::compute_VolumeFraction(&material,0);

        if(!assertEquals((double)-1,vf, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test12() {

        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 12 - VF of 1 for range 0";
        std::string testDescription;
        TestResult result(suiteName, testName, 12, testDescription);

        puma::Workspace material(1e-6,false);
        material.matrix.resize(100,100,100,0);

        double vf = puma::compute_VolumeFraction(&material,0);

        if(!assertEquals((double)1,vf, &result)) {
            return result;
        }

        double vf2 = puma::compute_VolumeFraction(&material,50767);

        if(!assertEquals((double)-1,vf2, &result)) {
            return result;
        }

        double vf3 = puma::compute_VolumeFraction(&material,-1);

        if(!assertEquals((double)-1,vf3, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test13() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 13 - VF of 0 for range 1";
        std::string testDescription;
        TestResult result(suiteName, testName, 13, testDescription);

        puma::Workspace material(1e-6,false);
        material.matrix.resize(100,100,100,0);

        double vf = puma::compute_VolumeFraction(&material,1);

        if(!assertEquals((double)0,vf, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test14() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 14 - VF of 0.5 for range 0";
        std::string testDescription;
        TestResult result(suiteName, testName, 14, testDescription);

        puma::Workspace material(1e-6,false);
        material.matrix.resize(100,100,100,0);
        material.matrix.set(0,99,0,99,0,49,1);

        double vf = puma::compute_VolumeFraction(&material,0);

        if(!assertEquals((double)0.5,vf, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test15() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 15 - VF of 0.5 for range 1";
        std::string testDescription;
        TestResult result(suiteName, testName, 15, testDescription);

        puma::Workspace material(1e-6,false);
        material.matrix.resize(100,100,100,0);
        material.matrix.set(0,99,0,99,0,49,1);

        double vf = puma::compute_VolumeFraction(&material,1);

        if(!assertEquals((double)0.5,vf, &result)) {
            return result;
        }

        return result;
    }



    static TestResult test16() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 16 - VF of 0 for range 2";
        std::string testDescription;
        TestResult result(suiteName, testName, 16, testDescription);

        puma::Workspace material(1e-6,false);
        material.matrix.resize(100,100,100,0);
        material.matrix.set(0,99,0,99,0,49,1);

        double vf = puma::compute_VolumeFraction(&material,2);

        if(!assertEquals((double)0,vf, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test17() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 17 - VF of 0.5 for range 0 > 1300^3";
        std::string testDescription;
        TestResult result(suiteName, testName, 17, testDescription);

        puma::Workspace material(1e-6,false);
        material.matrix.resize(1300,1300,1300,0);
        material.matrix.set(0,1299,0,1299,0,649,2);

        double vf = puma::compute_VolumeFraction(&material,0);

        if(!assertEquals((double)0.5,vf, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test18() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 18 - VF of 0.5 for range 2 > 1300^3";
        std::string testDescription;
        TestResult result(suiteName, testName, 18, testDescription);

        puma::Workspace material(1e-6,false);
        material.matrix.resize(1300,1300,1300,0);
        material.matrix.set(0,1299,0,1299,0,649,2);

        double vf = puma::compute_VolumeFraction(&material,2);

        if(!assertEquals((double)0.5,vf, &result)) {
            return result;
        }

        vf = puma::compute_VolumeFraction(&material,2);

        if(!assertEquals((double)0.5,vf, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test19() {
        std::string suiteName = "VolumeFraction_test";
        std::string testName = "VolumeFraction_test: Test 19 - VF of 0 for range 1 > 1300^3";
        std::string testDescription;
        TestResult result(suiteName, testName, 19, testDescription);

        puma::Workspace material(1e-6,false);
        material.matrix.resize(1300,1300,1300,0);
        material.matrix.set(0,1299,0,1299,0,649,2);

        double vf = puma::compute_VolumeFraction(&material,1);

        if(!assertEquals((double)0,vf, &result)) {
            return result;
        }

        return result;
    }

};
