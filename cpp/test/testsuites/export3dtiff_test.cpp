#include "../testframework/subtest.h"
#include "puma.h"


class Export3DTiff_Test : public SubTest {
public:

    Export3DTiff_Test() {
        testSuiteName = "Export_3DTiff_Test";

        tests.push_back(test1);
        //tests.push_back(test2);
        tests.push_back(test3);
        tests.push_back(test4);
        tests.push_back(test5);
        tests.push_back(test6);
        //tests.push_back(test7);
        tests.push_back(test8);
        tests.push_back(test9);
        tests.push_back(test10);
        tests.push_back(test11);
        //tests.push_back(test12);
        tests.push_back(test13);
        tests.push_back(test14);
        tests.push_back(test15);
        tests.push_back(test16);
        //tests.push_back(test17);
        tests.push_back(test18);
        tests.push_back(test19);
        tests.push_back(test20);
        tests.push_back(test21);
        //tests.push_back(test22);
        tests.push_back(test23);
        tests.push_back(test24);
        tests.push_back(test25);
        tests.push_back(test26);
        //tests.push_back(test27);
        tests.push_back(test28);
        tests.push_back(test29);
        tests.push_back(test30);


    }

    static TestResult test1() {

        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::Workspace - Test 1";
        std::string testDescription = "empty matrix";
        TestResult result(suiteName, testName, 1, testDescription);

        puma::Workspace grayWS(1e-6,false);

        bool success = puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/export_3DTiff_Test1",false);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::Workspace - Test 2";
        std::string testDescription = "matrix > 1300";
        TestResult result(suiteName, testName, 2, testDescription);

        puma::Workspace grayWS(1e-6,false);

        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("1300_Spheres.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double vF_1 = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(128,255));

        success = puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/export_3DTiff_Test2.tif",false);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,"cpp/test/out/tiff/export_3DTiff_Test2.tif",0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double vF_2 = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(128,255));

        if(!assertEquals(vF_1,vF_2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::Workspace - Test 3";
        std::string testDescription = "200, invalid name, norm, 10";
        TestResult result(suiteName, testName, 3, testDescription);

        puma::Workspace grayWS(1e-6,false);

        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::export_3DTiff(&grayWS,"/jd/ekdjf/eidjf/eid5.tif",false);
        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }


        return result;
    }

    static TestResult test4() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::Workspace - Test 4";
        std::string testDescription = "200, valid name, non-norm, <0";
        TestResult result(suiteName, testName, 4, testDescription);

        puma::Workspace grayWS(1e-6,false);
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double vF_1 = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(90,255));

        success = puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/export_3DTiff_Test4.tif",false);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,"cpp/test/out/tiff/export_3DTiff_Test4.tif",0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double vF_2 = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(90,255));

        if(!assertEquals(vF_1,vF_2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::Workspace - Test 5";
        std::string testDescription = "200, valid name, norm, > 1000";
        TestResult result(suiteName, testName, 5, testDescription);

        puma::Workspace grayWS(1e-6,false);
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }
        grayWS.matrix.normalize(0,128);

        success = puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/export_3DTiff_Test5.tif",true);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,"cpp/test/out/tiff/export_3DTiff_Test5.tif",0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double vF_2 = puma::compute_VolumeFraction(&grayWS,puma::Cutoff(90,255));

        if(!assertEquals((double)0.20820775,vF_2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test6() {

        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::Workspace - Test 1";
        std::string testDescription = "empty matrix";
        TestResult result(suiteName, testName, 6, testDescription);

        puma::Workspace segWS(1e-6,false);

        bool success = puma::export_3DTiff(&segWS,"cpp/test/out/tiff/export_3DTiff_Test6",false);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test7() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::Workspace - Test 2";
        std::string testDescription = "matrix > 1300";
        TestResult result(suiteName, testName, 7, testDescription);

        puma::Workspace segWS(1e-6,false);

        bool success = puma::import_3DTiff(&segWS,puma::path_to_example_file("1300_Spheres.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double vF_1 = puma::compute_VolumeFraction(&segWS,1,8);

        success = puma::export_3DTiff(&segWS,"cpp/test/out/tiff/export_3DTiff_Test7.tif",false);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&segWS,"cpp/test/out/tiff/export_3DTiff_Test7.tif",0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double vF_2 = puma::compute_VolumeFraction(&segWS,1,8);

        std::cout << "segWS Volume Fractions: " << vF_1 << " " << vF_2 << std::endl;

        if(!assertEquals(vF_1,vF_2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test8() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::Workspace - Test 3";
        std::string testDescription = "200, invalid name, norm, 10";
        TestResult result(suiteName, testName, 8, testDescription);

        puma::Workspace segWS(1e-6,false);

        bool success = puma::import_3DTiff(&segWS,puma::path_to_example_file("200_fiberform_segmented.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::export_3DTiff(&segWS,"/jd/ekdjf/eidjf/eid5.tif",false);
        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }


        return result;
    }

    static TestResult test9() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::Workspace - Test 4";
        std::string testDescription = "200, valid name, non-norm, <0";
        TestResult result(suiteName, testName, 9, testDescription);

        puma::Workspace segWS(1e-6,false);
        bool success = puma::import_3DTiff(&segWS,puma::path_to_example_file("200_fiberform_segmented.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double vF_1 = puma::compute_VolumeFraction(&segWS,1,8);

        success = puma::export_3DTiff(&segWS,"cpp/test/out/tiff/export_3DTiff_Test9.tif",false);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&segWS,"cpp/test/out/tiff/export_3DTiff_Test9.tif",0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double vF_2 = puma::compute_VolumeFraction(&segWS,1,8);

        std::cout << "segWS Volume Fractions: " << vF_1 << " " << vF_2 << std::endl;

        if(!assertEquals(vF_1,vF_2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test10() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::Workspace - Test 5";
        std::string testDescription = "200, valid name, norm, > 1000";
        TestResult result(suiteName, testName, 10, testDescription);

        puma::Workspace segWS(1e-6,false);
        bool success = puma::import_3DTiff(&segWS,puma::path_to_example_file("200_fiberform_segmented.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::export_3DTiff(&segWS,"cpp/test/out/tiff/export_3DTiff_Test10.tif",true);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test11() {

        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<short> - Test 1";
        std::string testDescription = "empty matrix";
        TestResult result(suiteName, testName, 11, testDescription);

        puma::Matrix<short> sMatrix;

        bool success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test11",false);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test12() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<short> - Test 2";
        std::string testDescription = "matrix > 1300";
        TestResult result(suiteName, testName, 12, testDescription);

        puma::Matrix<short> sMatrix;

        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("1300_Spheres.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_1 = sMatrix.average();

        success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test12.tif",false);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test12.tif",0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_2 = sMatrix.average();

        std::cout << "segWS Averages: " << avg_1 << " " << avg_2 << std::endl;

        if(!assertEquals(avg_1,avg_2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test13() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<short> - Test 3";
        std::string testDescription = "200, invalid name, norm, 10";
        TestResult result(suiteName, testName, 13, testDescription);

        puma::Matrix<short> sMatrix;

        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::export_3DTiff(&sMatrix,"/jd/ekdjf/eidjf/eid5.tif",false);
        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }


        return result;
    }

    static TestResult test14() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<short> - Test 4";
        std::string testDescription = "200, valid name, non-norm, <0";
        TestResult result(suiteName, testName, 14, testDescription);

        puma::Matrix<short> sMatrix;

        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_1 = sMatrix.average();

        success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test14.tif",false);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test14.tif",0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_2 = sMatrix.average();

        std::cout << "segWS Volume Fractions: " << avg_1 << " " << avg_2 << std::endl;

        if(!assertEquals(avg_1,avg_2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test15() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<short> - Test 5";
        std::string testDescription = "200, valid name, norm, > 1000";
        TestResult result(suiteName, testName, 15, testDescription);

        puma::Matrix<short> sMatrix;
        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test15.tif",true);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }


        return result;
    }

    static TestResult test16() {

        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<int> - Test 1";
        std::string testDescription = "empty matrix";
        TestResult result(suiteName, testName, 16, testDescription);

        puma::Matrix<int> sMatrix;

        bool success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test6",false);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test17() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<int> - Test 2";
        std::string testDescription = "matrix > 1300";
        TestResult result(suiteName, testName, 17, testDescription);

        puma::Matrix<int> sMatrix;

        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("1300_Spheres.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_1 = sMatrix.average();

        success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test17.tif",false);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test17.tif",0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_2 = sMatrix.average();

        std::cout << "segWS Averages: " << avg_1 << " " << avg_2 << std::endl;

        if(!assertEquals(avg_1,avg_2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test18() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<int> - Test 3";
        std::string testDescription = "200, invalid name, norm, 10";
        TestResult result(suiteName, testName, 18, testDescription);

        puma::Matrix<int> sMatrix;

        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::export_3DTiff(&sMatrix,"/jd/ekdjf/eidjf/eid5.tif",false);
        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }


        return result;
    }

    static TestResult test19() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<int> - Test 4";
        std::string testDescription = "200, valid name, non-norm, <0";
        TestResult result(suiteName, testName, 19, testDescription);

        puma::Matrix<int> sMatrix;

        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_1 = sMatrix.average();

        success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test18.tif",false);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test18.tif",0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_2 = sMatrix.average();

        std::cout << "segWS Volume Fractions: " << avg_1 << " " << avg_2 << std::endl;

        if(!assertEquals(avg_1,avg_2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test20() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<int> - Test 5";
        std::string testDescription = "200, valid name, norm, > 1000";
        TestResult result(suiteName, testName, 20, testDescription);

        puma::Matrix<int> sMatrix;
        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test19.tif",true);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }


        return result;
    }

    static TestResult test21() {

        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<float> - Test 1";
        std::string testDescription = "empty matrix";
        TestResult result(suiteName, testName, 21, testDescription);

        puma::Matrix<float> sMatrix;

        bool success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test21",false);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test22() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<float> - Test 2";
        std::string testDescription = "matrix > 1300";
        TestResult result(suiteName, testName, 22, testDescription);

        puma::Matrix<float> sMatrix;

        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("1300_Spheres.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_1 = sMatrix.average();

        success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test22.tif",false);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test22.tif",0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_2 = sMatrix.average();

        std::cout << "segWS Averages: " << avg_1 << " " << avg_2 << std::endl;

        if(!assertEquals(avg_1,avg_2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test23() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<float> - Test 3";
        std::string testDescription = "200, invalid name, norm, 10";
        TestResult result(suiteName, testName, 23, testDescription);

        puma::Matrix<float> sMatrix;

        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::export_3DTiff(&sMatrix,"/jd/ekdjf/eidjf/eid5.tif",false);
        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }


        return result;
    }

    static TestResult test24() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<float> - Test 4";
        std::string testDescription = "200, valid name, non-norm, <0";
        TestResult result(suiteName, testName, 24, testDescription);

        puma::Matrix<float> sMatrix;

        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_1 = sMatrix.average();

        success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test24.tif",false);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test24.tif",0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_2 = sMatrix.average();

        std::cout << "segWS Volume Fractions: " << avg_1 << " " << avg_2 << std::endl;

        if(!assertEquals(avg_1,avg_2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test25() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<float> - Test 5";
        std::string testDescription = "200, valid name, norm, > 1000";
        TestResult result(suiteName, testName, 25, testDescription);

        puma::Matrix<float> sMatrix;
        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test25.tif",true);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }


        return result;
    }

    static TestResult test26() {

        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<double> - Test 1";
        std::string testDescription = "empty matrix";
        TestResult result(suiteName, testName, 26, testDescription);

        puma::Matrix<double> sMatrix;

        bool success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test26",false);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test27() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<double> - Test 2";
        std::string testDescription = "matrix > 1300";
        TestResult result(suiteName, testName, 27, testDescription);

        puma::Matrix<double> sMatrix;

        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("1300_Spheres.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_1 = sMatrix.average();

        success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test27.tif",false);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test27.tif",0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_2 = sMatrix.average();

        std::cout << "segWS Averages: " << avg_1 << " " << avg_2 << std::endl;

        if(!assertEquals(avg_1,avg_2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test28() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<double> - Test 3";
        std::string testDescription = "200, invalid name, norm, 10";
        TestResult result(suiteName, testName, 28, testDescription);

        puma::Matrix<double> sMatrix;

        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::export_3DTiff(&sMatrix,"/jd/ekdjf/eidjf/eid5.tif",false);
        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }


        return result;
    }

    static TestResult test29() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<double> - Test 4";
        std::string testDescription = "200, valid name, non-norm, <0";
        TestResult result(suiteName, testName, 29, testDescription);

        puma::Matrix<double> sMatrix;

        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_1 = sMatrix.average();

        success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test29.tif",false);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test29.tif",0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double avg_2 = sMatrix.average();

        std::cout << "segWS Volume Fractions: " << avg_1 << " " << avg_2 << std::endl;

        if(!assertEquals(avg_1,avg_2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test30() {
        std::string suiteName = "export_3DTiff_Test";
        std::string testName = "export_3DTiff_Test::puma::Matrix<double> - Test 5";
        std::string testDescription = "200, valid name, norm, > 1000";
        TestResult result(suiteName, testName, 30, testDescription);

        puma::Matrix<double> sMatrix;
        bool success = puma::import_3DTiff(&sMatrix,puma::path_to_example_file("200_fiberform.tif"),0);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        success = puma::export_3DTiff(&sMatrix,"cpp/test/out/tiff/export_3DTiff_Test30.tif",true);
        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        return result;
    }

};
