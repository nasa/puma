#include "../testframework/subtest.h"
#include "puma.h"


class ExportVTK_Test : public SubTest {
public:

    ExportVTK_Test() {
        testSuiteName = "Export_vtk_Test";

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

        std::string suiteName = "export_vtk_Test";
        std::string testName = "export_vtk_Test: Test 1 - Checking in Paraview that axis are correct";
        std::string testDescription;
        TestResult result(suiteName, testName, 1, testDescription);

        puma::Matrix<double> test(10,5,3,0);
        test.at(9,4,0) = 1;
        puma::export_vtk(&test, "cpp/test/out/vtk/test_xycorner");

        test.set(0);
        test.at(9,0,2) = 1;
        puma::export_vtk(&test, "cpp/test/out/vtk/test_xzcorner");

        test.set(0);
        test.at(0,4,2) = 1;
        puma::export_vtk(&test, "cpp/test/out/vtk/test_yzcorner");

        return result;
    }

    static TestResult test2() {

        std::string suiteName = "export_vtk_Test";
        std::string testName = "export_vtk_Test: Test 2 - Exporting WS";
        std::string testDescription;
        TestResult result(suiteName, testName, 2, testDescription);

        puma::Matrix<double> test(10,10,10, 1);
        test.set(0,4,0,4,0,4,2);
        puma::export_vtk(&test, "cpp/test/out/vtk/test_ws");

        return result;
    }

    static TestResult test3() {

        std::string suiteName = "export_vtk_Test";
        std::string testName = "export_vtk_Test: Test 3 - Exporting MatVec3 and checking vector components in Paraview";
        std::string testDescription;
        TestResult result(suiteName, testName, 3, testDescription);

        puma::Matrix<double> test(10,10,10, 0);
        test.set(4,4,4,4,0,9,1);
        puma::export_vtk(&test, "cpp/test/out/vtk/test_ws2");

        puma::MatVec3<double> test2(10,10,10);
        test2.set(4,4,4,4,0,9,puma::Vec3<double>(0.,0.,1.));
        puma::export_vtk(&test2, "cpp/test/out/vtk/test_MatVec3");

        return result;
    }

    static TestResult test4() {

        std::string suiteName = "export_vtk_Test";
        std::string testName = "export_vtk_Test: Test 4 - Testing if adding .vtk or if not when present";
        std::string testDescription;
        TestResult result(suiteName, testName, 4, testDescription);

        puma::Matrix<double> test(1,1,1);
        puma::export_vtk(&test, "cpp/test/out/vtk/test");
        puma::export_vtk(&test, "cpp/test/out/vtk/test.vtk");

        return result;
    }

    static TestResult test5() {

        std::string suiteName = "export_vtk_Test";
        std::string testName = "export_vtk_Test: Test 5 - Testing if ASCII option working for puma matrices";
        std::string testDescription;
        TestResult result(suiteName, testName, 5, testDescription);

        puma::Matrix<double> testMat(20,20,20, 5);
        testMat.set(0, testMat.X()/2, 0, -1, 0, -1, 10);
        puma::export_vtk(&testMat, "cpp/test/out/vtk/testMatASCII", 'a');

        return result;
    }

    static TestResult test6() {

        std::string suiteName = "export_vtk_Test";
        std::string testName = "export_vtk_Test: Test 6 - Testing if ASCII option working for workspaces";
        std::string testDescription;
        TestResult result(suiteName, testName, 6, testDescription);

        puma::Workspace testWS(20,20,20, 5);
        testWS.matrix.set(0, testWS.X()/2, 0, -1, 0, -1, 10);
        puma::export_vtk(&testWS, "cpp/test/out/vtk/testWSASCII", 'a');

        return result;
    }

    static TestResult test7() {

        std::string suiteName = "export_vtk_Test";
        std::string testName = "export_vtk_Test: Test 7 - Testing if ASCII option working for MatVec3";
        std::string testDescription;
        TestResult result(suiteName, testName, 7, testDescription);

        puma::MatVec3<double> testVec(20,20,20, puma::Vec3<double>(1.,0.,0.));
        testVec.set(0, testVec.X()/2, 0, -1, 0, -1, puma::Vec3<double>(0.,1.,0.));
        puma::export_vtk(&testVec, "cpp/test/out/vtk/testVecASCII", 'a');

        return result;
    }

    static TestResult test8() {

        std::string suiteName = "export_vtk_Test";
        std::string testName = "export_vtk_Test: Test 8 - Testing if unstructured option working on puma matrices, checking that it defaults to ASCII";
        std::string testDescription;
        TestResult result(suiteName, testName, 8, testDescription);

        puma::Matrix<double> testMat(20,20,20, 5);
        testMat.set(0, testMat.X()/2, 0, -1, 0, -1, 10);
        puma::export_vtk(&testMat, "cpp/test/out/vtk/testMatunstructured", 'b', true);

        return result;
    }

    static TestResult test9() {

        std::string suiteName = "export_vtk_Test";
        std::string testName = "export_vtk_Test: Test 9 - Testing if unstructured option working on puma workspaces, checking that it defaults to ASCII";
        std::string testDescription;
        TestResult result(suiteName, testName, 9, testDescription);

        puma::Workspace testWS(20,20,20, 5);
        testWS.matrix.set(0, testWS.X()/2, 0, -1, 0, -1, 10);
        puma::export_vtk(&testWS, "cpp/test/out/vtk/testWSunstructured", 'b', true);

        return result;
    }

    static TestResult test10() {

        std::string suiteName = "export_vtk_Test";
        std::string testName = "export_vtk_Test: Test 10 - Testing if unstructured option working on puma MatVec3, checking that it defaults to ASCII";
        std::string testDescription;
        TestResult result(suiteName, testName, 10, testDescription);

        puma::MatVec3<double> testVec(20,20,20, puma::Vec3<double>(1.,0.,0.));
        testVec.set(0, testVec.X()/2, 0, -1, 0, -1, puma::Vec3<double>(0.,1.,0.));
        puma::export_vtk(&testVec, "cpp/test/out/vtk/testVecunstructured", 'b', true);

        return result;
    }

};
