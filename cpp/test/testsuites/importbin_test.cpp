#include "../testframework/subtest.h"
#include "puma.h"


class ImportBin_Test : public SubTest {
public:

    static const int numThreads = 19;

    ImportBin_Test() {
        testSuiteName = "ImportBin_Test";

        tests.push_back(test1);
        tests.push_back(test2);
        tests.push_back(test3);
        tests.push_back(test4);
        tests.push_back(test5);
        tests.push_back(test6);
        tests.push_back(test7);
    }


    static TestResult test1() {

        std::string suiteName = "import_bin_Test";
        std::string testName = "import_bin_Test: Test 1 - Workspace";
        std::string testDescription;
        TestResult result(suiteName, testName, 1, testDescription);

        puma::Workspace workspace(100,13,43,1e-6,false);
        puma::export_bin(&workspace,"cpp/test/out/bin/test");

        //test 1 : zero numThreads, valid file - should return true
        puma::Workspace workspace2;
        bool success2 = puma::import_bin(&workspace2,"cpp/test/out/bin/test.puma",0);
        if(!assertEquals((bool)true,success2, &result)) {
            return result;
        }
        if(!assertEquals(workspace.X(),workspace2.X(), &result)) {
            return result;
        }
        if(!assertEquals(workspace.Y(),workspace2.Y(), &result)) {
            return result;
        }
        if(!assertEquals(workspace.Z(),workspace2.Z(), &result)) {
            return result;
        }

        //test 2 : negative numThreads, valid file - should return true
        puma::Workspace workspace3;
        bool success3 = puma::import_bin(&workspace3,"cpp/test/out/bin/test.puma",-5);
        if(!assertEquals((bool)true,success3, &result)) {
            return result;
        }
        if(!assertEquals(workspace.X(),workspace3.X(), &result)) {
            return result;
        }
        if(!assertEquals(workspace.Y(),workspace3.Y(), &result)) {
            return result;
        }
        if(!assertEquals(workspace.Z(),workspace3.Z(), &result)) {
            return result;
        }

        //test 3 : negative numThreads, valid file - should return true
        puma::Workspace workspace4;
        bool success4 = puma::import_bin(&workspace4,"cpp/test/out/bin/test.puma",5);
        if(!assertEquals((bool)true,success4, &result)) {
            return result;
        }
        if(!assertEquals(workspace.X(),workspace4.X(), &result)) {
            return result;
        }
        if(!assertEquals(workspace.Y(),workspace4.Y(), &result)) {
            return result;
        }
        if(!assertEquals(workspace.Z(),workspace4.Z(), &result)) {
            return result;
        }

        //test 4 : zero numThreads, invalid file - should return false
        puma::Workspace workspace5;
        bool success5 = puma::import_bin(&workspace5,"cpp/test/out/tessdfasdfasdf.puasdfma",0);
        if(!assertEquals((bool)false,success5, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {

        std::string suiteName = "import_bin_Test";
        std::string testName = "import_bin_Test: Test 2 - Matrix<double>";
        std::string testDescription;
        TestResult result(suiteName, testName, 2, testDescription);

        puma::Matrix<double> matrix(100,13,43);
        puma::export_bin(&matrix,"cpp/test/out/bin/test");

        //test 1 : zero numThreads, valid file - should return true
        puma::Matrix<double> matrix2;
        bool success2 = puma::import_bin(&matrix2,"cpp/test/out/bin/test.puma",0);
        if(!assertEquals((bool)true,success2, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix2.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix2.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix2.Z(), &result)) {
            return result;
        }

        //test 2 : negative numThreads, valid file - should return true
        puma::Matrix<double> matrix3;
        bool success3 = puma::import_bin(&matrix3,"cpp/test/out/bin/test.puma",-5);
        if(!assertEquals((bool)true,success3, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix3.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix3.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix3.Z(), &result)) {
            return result;
        }

        //test 3 : negative numThreads, valid file - should return true
        puma::Matrix<double> matrix4;
        bool success4 = puma::import_bin(&matrix4,"cpp/test/out/bin/test.puma",5);
        if(!assertEquals((bool)true,success4, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix4.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix4.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix4.Z(), &result)) {
            return result;
        }

        //test 4 : zero numThreads, invalid file - should return false
        puma::Matrix<double> matrix5;
        bool success5 = puma::import_bin(&matrix5,"cpp/test/out/tessdfasdfasdf.puasdfma",0);
        if(!assertEquals((bool)false,success5, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {

        std::string suiteName = "import_bin_Test";
        std::string testName = "import_bin_Test: Test 3 - Matrix<float>";
        std::string testDescription;
        TestResult result(suiteName, testName, 3, testDescription);

        puma::Matrix<float> matrix(100,13,43);
        puma::export_bin(&matrix,"cpp/test/out/bin/test");

        //test 1 : zero numThreads, valid file - should return true
        puma::Matrix<float> matrix2;
        bool success2 = puma::import_bin(&matrix2,"cpp/test/out/bin/test.puma",0);
        if(!assertEquals((bool)true,success2, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix2.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix2.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix2.Z(), &result)) {
            return result;
        }

        //test 2 : negative numThreads, valid file - should return true
        puma::Matrix<float> matrix3;
        bool success3 = puma::import_bin(&matrix3,"cpp/test/out/bin/test.puma",-5);
        if(!assertEquals((bool)true,success3, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix3.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix3.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix3.Z(), &result)) {
            return result;
        }

        //test 3 : negative numThreads, valid file - should return true
        puma::Matrix<float> matrix4;
        bool success4 = puma::import_bin(&matrix4,"cpp/test/out/bin/test.puma",5);
        if(!assertEquals((bool)true,success4, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix4.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix4.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix4.Z(), &result)) {
            return result;
        }

        //test 4 : zero numThreads, invalid file - should return false
        puma::Matrix<float> matrix5;
        bool success5 = puma::import_bin(&matrix5,"cpp/test/out/tessdfasdfasdf.puasdfma",0);
        if(!assertEquals((bool)false,success5, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test4() {

        std::string suiteName = "import_bin_Test";
        std::string testName = "import_bin_Test: Test 4 - Matrix<short>";
        std::string testDescription;
        TestResult result(suiteName, testName, 4, testDescription);

        puma::Matrix<short> matrix(100,13,43);
        puma::export_bin(&matrix,"cpp/test/out/bin/test");

        //test 1 : zero numThreads, valid file - should return true
        puma::Matrix<short> matrix2;
        bool success2 = puma::import_bin(&matrix2,"cpp/test/out/bin/test.puma",0);
        if(!assertEquals((bool)true,success2, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix2.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix2.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix2.Z(), &result)) {
            return result;
        }

        //test 2 : negative numThreads, valid file - should return true
        puma::Matrix<short> matrix3;
        bool success3 = puma::import_bin(&matrix3,"cpp/test/out/bin/test.puma",-5);
        if(!assertEquals((bool)true,success3, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix3.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix3.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix3.Z(), &result)) {
            return result;
        }

        //test 3 : negative numThreads, valid file - should return true
        puma::Matrix<short> matrix4;
        bool success4 = puma::import_bin(&matrix4,"cpp/test/out/bin/test.puma",5);
        if(!assertEquals((bool)true,success4, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix4.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix4.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix4.Z(), &result)) {
            return result;
        }

        //test 4 : zero numThreads, invalid file - should return false
        puma::Matrix<short> matrix5;
        bool success5 = puma::import_bin(&matrix5,"cpp/test/out/tessdfasdfasdf.puasdfma",0);
        if(!assertEquals((bool)false,success5, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {

        std::string suiteName = "import_bin_Test";
        std::string testName = "import_bin_Test: Test 5 - Matrix<int>";
        std::string testDescription;
        TestResult result(suiteName, testName, 5, testDescription);

        puma::Matrix<int> matrix(100,13,43);
        puma::export_bin(&matrix,"cpp/test/out/bin/test");

        //test 1 : zero numThreads, valid file - should return true
        puma::Matrix<int> matrix2;
        bool success2 = puma::import_bin(&matrix2,"cpp/test/out/bin/test.puma",0);
        if(!assertEquals((bool)true,success2, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix2.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix2.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix2.Z(), &result)) {
            return result;
        }

        //test 2 : negative numThreads, valid file - should return true
        puma::Matrix<int> matrix3;
        bool success3 = puma::import_bin(&matrix3,"cpp/test/out/bin/test.puma",-5);
        if(!assertEquals((bool)true,success3, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix3.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix3.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix3.Z(), &result)) {
            return result;
        }

        //test 3 : negative numThreads, valid file - should return true
        puma::Matrix<int> matrix4;
        bool success4 = puma::import_bin(&matrix4,"cpp/test/out/bin/test.puma",5);
        if(!assertEquals((bool)true,success4, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix4.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix4.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix4.Z(), &result)) {
            return result;
        }

        //test 4 : zero numThreads, invalid file - should return false
        puma::Matrix<int> matrix5;
        bool success5 = puma::import_bin(&matrix5,"cpp/test/out/tessdfasdfasdf.puasdfma",0);
        if(!assertEquals((bool)false,success5, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test6() {

        std::string suiteName = "import_bin_Test";
        std::string testName = "import_bin_Test: Test 6 - Matrix< puma::Vec3<double> >";
        std::string testDescription;
        TestResult result(suiteName, testName, 6, testDescription);

        puma::MatVec3<double> matrix(100,13,43);
        puma::export_bin(&matrix,"cpp/test/out/bin/test");

        //test 1 : zero numThreads, valid file - should return true
        puma::MatVec3<double> matrix2;
        bool success2 = puma::import_bin(&matrix2,"cpp/test/out/bin/test.puma",0);
        if(!assertEquals((bool)true,success2, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix2.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix2.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix2.Z(), &result)) {
            return result;
        }

        //test 2 : negative numThreads, valid file - should return true
        puma::MatVec3<double> matrix3;
        bool success3 = puma::import_bin(&matrix3,"cpp/test/out/bin/test.puma",-5);
        if(!assertEquals((bool)true,success3, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix3.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix3.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix3.Z(), &result)) {
            return result;
        }

        //test 3 : negative numThreads, valid file - should return true
        puma::MatVec3<double> matrix4;
        bool success4 = puma::import_bin(&matrix4,"cpp/test/out/bin/test.puma",5);
        if(!assertEquals((bool)true,success4, &result)) {
            return result;
        }
        if(!assertEquals(matrix.X(),matrix4.X(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Y(),matrix4.Y(), &result)) {
            return result;
        }
        if(!assertEquals(matrix.Z(),matrix4.Z(), &result)) {
            return result;
        }

        //test 4 : zero numThreads, invalid file - should return false
        puma::MatVec3<double> matrix5;
        bool success5 = puma::import_bin(&matrix5,"cpp/test/out/tessdfasdfasdf.puasdfma",0);
        if(!assertEquals((bool)false,success5, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test7() {

        std::string suiteName = "import_bin_Test";
        std::string testName = "import_bin_Test: Test 7 - testing whether passing an already resized matrix fails";
        std::string testDescription;
        TestResult result(suiteName, testName, 7, testDescription);

        puma::Matrix<double> matrix(100,13,43);
        puma::export_bin(&matrix,"cpp/test/out/bin/test");

        puma::Matrix<double> matrix2(50,11,40);
        bool success2 = puma::import_bin(&matrix2,"cpp/test/out/bin/test.puma",0);

        if(!assertEquals((bool)true,success2, &result)) {
            return result;
        }

        return result;
    }

};
