#include "../testframework/subtest.h"
#include "puma.h"


class ExportBin_Test : public SubTest {
public:

    static const int numThreads = 19;

    ExportBin_Test() {
        testSuiteName = "ExportBin_Test";

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

    }


    static TestResult test1() {

        std::string suiteName = "export_bin_Test";
        std::string testName = "export_bin_Test: Test 1 - Empty Workspace";
        std::string testDescription;
        TestResult result(suiteName, testName, 1, testDescription);

        puma::Workspace workspace;
        bool success = puma::export_bin(&workspace,"cpp/test/out/bin/test1.puma");

        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {

        std::string suiteName = "export_bin_Test";
        std::string testName = "export_bin_Test: Test 2 - Empty Matrix<double>";
        std::string testDescription;
        TestResult result(suiteName, testName, 2, testDescription);

        puma::Matrix<double> matrix;
        bool success = puma::export_bin(&matrix,"cpp/test/out/bin/test2.puma");

        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {

        std::string suiteName = "export_bin_Test";
        std::string testName = "export_bin_Test: Test 3 - Empty Matrix<float>";
        std::string testDescription;
        TestResult result(suiteName, testName, 3, testDescription);

        puma::Matrix<float> matrix;
        bool success = puma::export_bin(&matrix,"cpp/test/out/bin/test3.puma");

        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test4() {

        std::string suiteName = "export_bin_Test";
        std::string testName = "export_bin_Test: Test 4 - Empty Matrix<short>";
        std::string testDescription;
        TestResult result(suiteName, testName, 4, testDescription);

        puma::Matrix<short> matrix;
        bool success = puma::export_bin(&matrix,"cpp/test/out/bin/test4.puma");

        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {

        std::string suiteName = "export_bin_Test";
        std::string testName = "export_bin_Test: Test 5 - Empty Matrix<int>";
        std::string testDescription;
        TestResult result(suiteName, testName, 5, testDescription);

        puma::Matrix<int> matrix;
        bool success = puma::export_bin(&matrix,"cpp/test/out/bin/test5.puma");

        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test6() {

        std::string suiteName = "export_bin_Test";
        std::string testName = "export_bin_Test: Test 6 - Empty Matrix < PVec3 > ";
        std::string testDescription;
        TestResult result(suiteName, testName, 6, testDescription);


        puma::MatVec3<double> matrix;
        bool success = puma::export_bin(&matrix,"cpp/test/out/bin/test6.puma");

        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test7() {

        std::string suiteName = "export_bin_Test";
        std::string testName = "export_bin_Test: Test 7 - Bad FileNames";
        std::string testDescription;
        TestResult result(suiteName, testName, 7, testDescription);

        puma::Workspace workspace(10,10,10,1e-6,false);
        bool success = puma::export_bin(&workspace,"");

        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        success = puma::export_bin(&workspace,"cpp/test/out/bin/test.puma");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        success = puma::export_bin(&workspace,"cpp/test/out/bin/test");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test8() {

        std::string suiteName = "export_bin_Test";
        std::string testName = "export_bin_Test: Test 8 - Bad FileNames";
        std::string testDescription;
        TestResult result(suiteName, testName, 8, testDescription);

        puma::Matrix<double> matrix(10,10,10);
        bool success = puma::export_bin(&matrix,"");

        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        success = puma::export_bin(&matrix,"cpp/test/out/bin/test.puma");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        success = puma::export_bin(&matrix,"cpp/test/out/bin/test");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test9() {

        std::string suiteName = "export_bin_Test";
        std::string testName = "export_bin_Test: Test 9 - Bad FileNames";
        std::string testDescription;
        TestResult result(suiteName, testName, 9, testDescription);

        puma::Matrix<float> matrix(10,10,10);
        bool success = puma::export_bin(&matrix,"");

        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        success = puma::export_bin(&matrix,"cpp/test/out/bin/test.puma");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        success = puma::export_bin(&matrix,"cpp/test/out/bin/test");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test10() {

        std::string suiteName = "export_bin_Test";
        std::string testName = "export_bin_Test: Test 10 - Bad FileNames";
        std::string testDescription;
        TestResult result(suiteName, testName, 10, testDescription);

        puma::Matrix<short> matrix(10,10,10);
        bool success = puma::export_bin(&matrix,"");

        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        success = puma::export_bin(&matrix,"cpp/test/out/bin/test.puma");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        success = puma::export_bin(&matrix,"cpp/test/out/bin/test");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test11() {

        std::string suiteName = "export_bin_Test";
        std::string testName = "export_bin_Test: Test 11 - Bad FileNames";
        std::string testDescription;
        TestResult result(suiteName, testName, 11, testDescription);

        puma::Matrix<int> matrix(10,10,10);
        bool success = puma::export_bin(&matrix,"");

        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        success = puma::export_bin(&matrix,"cpp/test/out/bin/test.puma");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        success = puma::export_bin(&matrix,"cpp/test/out/bin/test");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test12() {

        std::string suiteName = "export_bin_Test";
        std::string testName = "export_bin_Test: Test 12 - Bad FileNames";
        std::string testDescription;
        TestResult result(suiteName, testName, 12, testDescription);


        puma::MatVec3<double> matrix(10,10,10);
        bool success = puma::export_bin(&matrix,"");

        if(!assertEquals((bool)false,success, &result)) {
            return result;
        }

        success = puma::export_bin(&matrix,"cpp/test/out/bin/test.puma");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        success = puma::export_bin(&matrix,"cpp/test/out/bin/test");
        if(!assertEquals((bool)true,success, &result)) {
            return result;
        }

        return result;
    }



};



































