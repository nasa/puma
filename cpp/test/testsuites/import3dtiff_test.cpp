#include "../testframework/subtest.h"
#include "puma.h"


class Import3DTiff_Test : public SubTest {
public:

    Import3DTiff_Test() {
        testSuiteName = "Import3DTiff_Test";

        tests.push_back(test1);
        tests.push_back(test2);
        //tests.push_back(test3);
        tests.push_back(test4);
        tests.push_back(test5);
        tests.push_back(test6);
        tests.push_back(test7);
        tests.push_back(test8);
        tests.push_back(test9);
        tests.push_back(test10);
        //tests.push_back(test11);
        tests.push_back(test12);
        tests.push_back(test13);
        tests.push_back(test14);
        tests.push_back(test15);
        tests.push_back(test16);
        tests.push_back(test17);
        tests.push_back(test18);
        tests.push_back(test19);

    }

    static TestResult test1() {

        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 1 - Function 1 Test 1";
        std::string testDescription = "Workspace isEmpty :  full";
        TestResult result(suiteName, testName, 1, testDescription);

        puma::Workspace grayWS(100,100,100,1, 1e-6,false);

        if(!assertEquals((long)100*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)1,(double)grayWS.average(), &result)) {
            return result;
        }

	
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        if(!assertEquals((long)200*200*200,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200,(long)grayWS.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)200,(long)grayWS.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)200,(long)grayWS.Z(), &result)) {
            return result;
        }

        if(!assertEquals((double)68.7171568,(double)grayWS.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 2 - Function 1 Test 2";
        std::string testDescription = "fileName Content :  blank";
        TestResult result(suiteName, testName, 2, testDescription);

        puma::Workspace grayWS(1e-6,false);

        bool success = puma::import_3DTiff(&grayWS,"",0);

        if(!assertEquals((long)0,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals(false,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 3 - Function 1 Test 3";
        std::string testDescription = "Tiff File Size :  >1300";
        TestResult result(suiteName, testName, 3, testDescription);

        puma::Workspace grayWS(1e-6,false);

	
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("1300_Spheres.tif"),0);

        if(!assertEquals((long)1300*1300*1300,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double poro = 1 - puma::compute_VolumeFraction(&grayWS,puma::Cutoff(128,255));

        if(!assertEquals((double)0.767143078,(double)poro, &result)) {
            return result;
        }


        return result;
    }

    static TestResult test4() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 4 - Function 1 Test 4";
        std::string testDescription = "NumThreads Value :  maxint";
        TestResult result(suiteName, testName, 4, testDescription);

        puma::Workspace grayWS(1e-6,false);
	
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),2147483647);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)100.811828,(double)grayWS.average(), &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),1000);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)100.811828,(double)grayWS.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 5  - Function 1 Test 5";
        std::string testDescription = "NumThreads < 0";
        TestResult result(suiteName, testName, 5, testDescription);

        puma::Workspace grayWS(1e-6,false);

	
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),-10);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)100.811828,(double)grayWS.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test6() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 6 - Function 1 Test 6";
        std::string testDescription = "NumThreads = 0 ";
        TestResult result(suiteName, testName, 6, testDescription);

        puma::Workspace grayWS(1e-6,false);

	
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),0);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)100.811828,(double)grayWS.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test7() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 7 - Function 1 Test 7";
        std::string testDescription = "FileName Valid";
        TestResult result(suiteName, testName, 7, testDescription);

        puma::Workspace grayWS(1e-6,false);

	
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),10);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)100.811828,(double)grayWS.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test8() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 8 - Function 1 Test 8";
        std::string testDescription = "FileName invalid";
        TestResult result(suiteName, testName, 8, testDescription);

        puma::Workspace grayWS(100,100,100,0, 1e-6,false);

	
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("NoFile.tif"),0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,(long)grayWS.size(), &result)) {
            return result;
        }


        return result;
    }

    static TestResult test9() {

        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 9 - Function 2 Test 1";
        std::string testDescription = "Workspace isEmpty :  full";
        TestResult result(suiteName, testName, 9, testDescription);

        puma::Workspace grayWS(100,100,100,1, 1e-6,false);

        if(!assertEquals((long)100*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)1,(double)grayWS.average(), &result)) {
            return result;
        }

	
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0,199,0,199,0,199,0);

        if(!assertEquals((long)200*200*200,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200,(long)grayWS.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)200,(long)grayWS.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)200,(long)grayWS.Z(), &result)) {
            return result;
        }

        if(!assertEquals((double)68.7171568,(double)grayWS.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test10() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 10 - Function 2 Test 2";
        std::string testDescription = "fileName Content :  blank";
        TestResult result(suiteName, testName, 10, testDescription);

        puma::Workspace grayWS(1e-6,false);

        bool success = puma::import_3DTiff(&grayWS,"",0,99,0,99,0,99,0);

        if(!assertEquals((long)0,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals(false,(bool)success, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test11() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 11 - Function 2 Test 3";
        std::string testDescription = "Tiff File Size :  >1300";
        TestResult result(suiteName, testName, 11, testDescription);

        puma::Workspace grayWS(1e-6,false);

	
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("1300_Spheres.tif"),0,1299,0,1299,0,1299,0);

        if(!assertEquals((long)1300*1300*1300,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        double poro = 1 - puma::compute_VolumeFraction(&grayWS,puma::Cutoff(128,255));

        if(!assertEquals((double)0.767143078,(double)poro, &result)) {
            return result;
        }


        return result;
    }

    static TestResult test12() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 12 - Function 2 Test 4";
        std::string testDescription = "NumThreads Value :  maxint";
        TestResult result(suiteName, testName, 12, testDescription);

        puma::Workspace grayWS(1e-6,false);

	
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),0,99,0,99,0,99,2147483647);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)100.811828,(double)grayWS.average(), &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),0,99,0,99,0,99,1000);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)100.811828,(double)grayWS.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test13() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 13  - Function 2 Test 5";
        std::string testDescription = "NumThreads < 0";
        TestResult result(suiteName, testName, 13, testDescription);

        puma::Workspace grayWS(1e-6,false);

	
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),0,99,0,99,0,99,-10);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)100.811828,(double)grayWS.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test14() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 14 - Function 2 Test 6";
        std::string testDescription = "NumThreads = 0 ";
        TestResult result(suiteName, testName, 14, testDescription);

        puma::Workspace grayWS(1e-6,false);

	
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),0,99,0,99,0,99,0);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)100.811828,(double)grayWS.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test15() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 15 - Function 2 Test 7";
        std::string testDescription = "FileName Valid";
        TestResult result(suiteName, testName, 15, testDescription);

        puma::Workspace grayWS(1e-6,false);

	
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),0,99,0,99,0,99,10);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)100.811828,(double)grayWS.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test16() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 16 - Function 2 Test 8";
        std::string testDescription = "FileName invalid";
        TestResult result(suiteName, testName, 16, testDescription);

        puma::Workspace grayWS(100,100,100,0, 1e-6,false);

	
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("NoFile.tif"),0,99,0,99,0,99,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,(long)grayWS.size(), &result)) {
            return result;
        }


        return result;
    }

    static TestResult test17() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 17 - Function 2 Test 9";
        std::string testDescription = "Testing X Domain";
        TestResult result(suiteName, testName, 16, testDescription);

        puma::Workspace grayWS(1e-6,false);

	
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),50,99,0,99,0,99,0);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)50*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)0.702066,(double)(1-puma::compute_VolumeFraction(&grayWS,puma::Cutoff(90,255))), &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),-2147483647,99,0,99,0,99,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),-1,99,0,99,0,99,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),20,100,0,99,0,99,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),20,19,0,99,0,99,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),20,2147483647,0,99,0,99,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),99,99,0,99,0,99,0);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)1*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)0.5621,(double)(1-puma::compute_VolumeFraction(&grayWS,puma::Cutoff(90,255))), &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,0,0,99,0,99,0);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)1*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)0.9921,(double)(1-puma::compute_VolumeFraction(&grayWS,puma::Cutoff(90,255))), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test18() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 18 - Function 2 Test 10";
        std::string testDescription = "Testing X Domain";
        TestResult result(suiteName, testName, 18, testDescription);

        puma::Workspace grayWS(1e-6,false);

	
        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,50,99,0,99,0);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)50*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)0.837194,(double)(1-puma::compute_VolumeFraction(&grayWS,puma::Cutoff(90,255))), &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,-2147483647,99,0,99,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,-1,99,0,99,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,20,100,0,99,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,20,19,0,99,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,20,2147483647,0,99,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,99,99,0,99,0);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)1*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)0.7939,(double)(1-puma::compute_VolumeFraction(&grayWS,puma::Cutoff(90,255))), &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,0,0,0,99,0);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)1*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)0.8659,(double)(1-puma::compute_VolumeFraction(&grayWS,puma::Cutoff(90,255))), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test19() {
        std::string suiteName = "Import3DTiff_Test";
        std::string testName = "Import3DTiff_Test: Test 19 - Function 2 Test 11";
        std::string testDescription = "Testing X Domain";
        TestResult result(suiteName, testName, 19, testDescription);

        puma::Workspace grayWS(1e-6,false);

        bool success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,0,99,50,99,0);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)50*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)0.829498,(double)(1-puma::compute_VolumeFraction(&grayWS,puma::Cutoff(90,255))), &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,0,99,-2147483647,99,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,0,99,-1,99,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,0,99,20,100,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,0,99,20,19,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,0,99,20,2147483647,0);

        if(!assertEquals((bool)false,(bool)success, &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,0,99,99,99,0);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)1*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)0.7814,(double)(1-puma::compute_VolumeFraction(&grayWS,puma::Cutoff(90,255))), &result)) {
            return result;
        }

        success = puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_fiberform.tif"),0,99,0,99,0,0,0);

        if(!assertEquals((bool)true,(bool)success, &result)) {
            return result;
        }

        if(!assertEquals((long)1*100*100,(long)grayWS.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)0.6888,(double)(1-puma::compute_VolumeFraction(&grayWS,puma::Cutoff(90,255))), &result)) {
            return result;
        }

        return result;
    }

};
