#include "../testframework/subtest.h"
#include "puma.h"


class ExportTextfile_Test : public SubTest {
public:

    ExportTextfile_Test() {
        testSuiteName = "Export_textfile_Test";

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

        std::string suiteName = "export_textfile_Test";
        std::string testName = "export_textfile_Test::Matrix - Test 1";
        std::string testDescription = "empty matrix";
        TestResult result(suiteName, testName, 1, testDescription);

        puma::MatVec3<double> mat;

        bool success = puma::export_Textfile(&mat,"cpp/test/out/textfiles/export_textfile_Test1");

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {

        std::string suiteName = "export_textfile_Test";
        std::string testName = "export_textfile_Test::Matrix - Test 2";
        std::string testDescription = "invalid filepath";
        TestResult result(suiteName, testName, 2, testDescription);

        puma::MatVec3<double> mat;

        bool success = puma::export_Textfile(&mat,"cpp/test/out/abc/export_textfile_Test2");

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {

        std::string suiteName = "export_textfile_Test";
        std::string testName = "export_textfile_Test::Matrix - Test 3";
        std::string testDescription = "default values";
        TestResult result(suiteName, testName, 3, testDescription);

        puma::Matrix<double> mat(2,2,2,1.123456789);

        bool success = puma::export_Textfile(&mat,"cpp/test/out/textfiles/export_textfile_Test3");

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test4() {

        std::string suiteName = "export_textfile_Test";
        std::string testName = "export_textfile_Test::Matrix - Test 4";
        std::string testDescription = "test on vector";
        TestResult result(suiteName, testName, 4, testDescription);

        puma::MatVec3<double> mat(2,2,2,puma::Vec3<double>(1.123456789,2.123456789,3.123456789));

        bool success = puma::export_Textfile(&mat,"cpp/test/out/textfiles/export_textfile_Test4");

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {

        std::string suiteName = "export_textfile_Test";
        std::string testName = "export_textfile_Test::Matrix - Test 5";
        std::string testDescription = "test on triangles";
        TestResult result(suiteName, testName, 5, testDescription);

        puma::Vec3<double> vec(1.123456789,2.123456789,3.123456789);
        puma::Matrix<puma::Triangle<double> > mat(2,2,2,puma::Triangle<double>(vec,vec,vec,1.1));

        bool success = puma::export_Textfile(&mat,"cpp/test/out/textfiles/export_textfile_Test5");

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test6() {

        std::string suiteName = "export_textfile_Test";
        std::string testName = "export_textfile_Test::Matrix - Test 6";
        std::string testDescription = "test with nondefault values (Precision should be changed to 18)";
        TestResult result(suiteName, testName, 6, testDescription);

        puma::MatVec3<double> mat(2,2,2,puma::Vec3<double>(1.123456789,2.123456789,3.123456789));

        bool success = puma::export_Textfile(&mat,"cpp/test/out/textfiles/export_textfile_Test6",true,true,true,128,26);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test7() {

        std::string suiteName = "export_textfile_Test";
        std::string testName = "export_textfile_Test::Workspace - Test 7";
        std::string testDescription = "empty matrix";
        TestResult result(suiteName, testName, 7, testDescription);

        puma::Workspace work(1e-6,false);

        bool success = puma::export_Textfile(&work,"cpp/test/out/textfiles/export_textfile_Test7");

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test8() {

        std::string suiteName = "export_textfile_Test";
        std::string testName = "export_textfile_Test::Workspace - Test 8";
        std::string testDescription = "invalid filepath";
        TestResult result(suiteName, testName, 8, testDescription);

        puma::MatVec3<double> mat;

        bool success = puma::export_Textfile(&mat,"cpp/test/out/abc/export_textfile_Test8");

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test9() {

        std::string suiteName = "export_textfile_Test";
        std::string testName = "export_textfile_Test::Workspace - Test 9";
        std::string testDescription = "default values";
        TestResult result(suiteName, testName, 9, testDescription);

        puma::Workspace work(2,2,2,4,1e-6,false);

        bool success = puma::export_Textfile(&work,"cpp/test/out/textfiles/export_textfile_Test9");

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test10() {

        std::string suiteName = "export_textfile_Test";
        std::string testName = "export_textfile_Test::Workspace - Test 10";
        std::string testDescription = "nondefault values (Precision should be changed to 18)";
        TestResult result(suiteName, testName, 10, testDescription);

        puma::Workspace work(2,2,2,4,1e-6,false);

        bool success = puma::export_Textfile(&work,"cpp/test/out/textfiles/export_textfile_Test10",true,true,true,128,26);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        return result;
    }

};
