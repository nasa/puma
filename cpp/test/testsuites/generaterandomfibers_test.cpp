#include "../testframework/subtest.h"
#include "puma.h"


class Generate_RandomFibers_Test : public SubTest {
public:

    Generate_RandomFibers_Test() {

        testSuiteName = "Generate_RandomFibers_Test";

        tests.push_back(test1);
        tests.push_back(test2);
        tests.push_back(test3);
        tests.push_back(test4);
        tests.push_back(test5);
        tests.push_back(test6);
        tests.push_back(test7);
        tests.push_back(test8);
        tests.push_back(test9);
//        tests.push_back(test10);
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
//        tests.push_back(test23);
        tests.push_back(test24);
        tests.push_back(test25);
        tests.push_back(test26);
        tests.push_back(test27);
        tests.push_back(test28);
        tests.push_back(test29);
//        tests.push_back(test30);
        tests.push_back(test31);
        tests.push_back(test32);
        tests.push_back(test33);
        tests.push_back(test34);
        tests.push_back(test35);
        tests.push_back(test36);
        tests.push_back(test37);
        tests.push_back(test38);
        tests.push_back(test39);
        tests.push_back(test40);
        tests.push_back(test41);
        tests.push_back(test42);
        tests.push_back(test43);
        tests.push_back(test44);
        tests.push_back(test45);
        tests.push_back(test46);
        tests.push_back(test47);
        tests.push_back(test48);
        tests.push_back(test49);
        tests.push_back(test50);
        tests.push_back(test51);
        tests.push_back(test52);
        tests.push_back(test53);
        tests.push_back(test54);
        tests.push_back(test55);
        tests.push_back(test56);
        tests.push_back(test57);
        tests.push_back(test58);
        tests.push_back(test59);
        tests.push_back(test60);
        tests.push_back(test61);
//        tests.push_back(test62);
//        tests.push_back(test63);
        tests.push_back(test64);
        tests.push_back(test65);
        tests.push_back(test66);
        tests.push_back(test67);
        tests.push_back(test68);
        tests.push_back(test69);
//        tests.push_back(test70);
//        tests.push_back(test71);
        tests.push_back(test72);
        tests.push_back(test73);


    }


    static TestResult test1() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight circle fiber error catch #1 - Invalid X Size";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=-1, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5.0, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100,"
                                      "; Should return false because of invalid xsize (negative)";
        TestResult result(suiteName, testName, 1, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightCircle(-1,200,200,5,1,5,1,0, -1, -1,false,0.8,100);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test2() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight circle fiber error catch #2 - Invalid Y Size";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=-1, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5.0, dLength=1.0, isotropic "
                                      ",intersect=false, poro=0.8, randomSeed=100,"
                                      "; Should return false because of invalid ysize (negative)";
        TestResult result(suiteName, testName, 2, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightCircle(200,-1,200,5,1,5,1,0, -1, -1,false,0.8,100);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);
        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test3() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight circle fiber error catch #3 - Invalid Z Size";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=-1, avgRadius=5.0, dRadius=1.0, avgLength=5.0, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100,"
                                      "; Should return false because of invalid zsize (negative)";
        TestResult result(suiteName, testName, 3, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightCircle(200,200,-1,5,1,5,1,0, -1, -1,false,0.8,100);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test4() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "curved circle fiber error catch #1 - Invalid X Size";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=0, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5.0, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 120, dRadiusOfCurvature=0, accuracy=1e-4"
                                      "; Should return false because of invalid xsize (0)";
        TestResult result(suiteName, testName, 4, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;

        input.curvedCircle(0,200,200,5,1,5,1,0, -1, -1,false,0.8,100,120,0,1e-4);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test5() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "curved circle fiber error catch #2 - Invalid Y Size";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=-1, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5.0, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 120, dRadiusOfCurvature=0, accuracy=1e-4"
                                      "; Should return false because of invalid ysize (0)";
        TestResult result(suiteName, testName, 5, testDescription);

        //creates an empty Grayscale workspace
        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,0,200,5,1,5,1,0, -1, -1,false,0.8,100,120,0,1e-4);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test6() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved circle fiber error catch #3 - Invalid Z Size";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=-1, avgRadius=5.0, dRadius=1.0, avgLength=5.0, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 120, dRadiusOfCurvature=0, accuracy=1e-4"
                                      "; Should return false because of invalid zsize (0)";
        TestResult result(suiteName, testName, 6, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,200,0,5,1,5,1,0, -1, -1,false,0.8,100,120,0,1e-4);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test7() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved circle fiber error catch #4 - Invalid Radius";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=0, dRadius=1.0, avgLength=5.0, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 120, dRadiusOfCurvature=0, accuracy=1e-4"
                                      "; Should return false because of invalid radius (0)";
        TestResult result(suiteName, testName, 7, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,200,200,0,1,5,1,0, -1, -1,false,0.8,100,120,0,1e-4);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test8() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved circle fiber error catch #5 - Invalid Radius";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=-1, dRadius=1.0, avgLength=5.0, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 120, dRadiusOfCurvature=0, accuracy=1e-4"
                                      "; Should return false because of invalid radius (negative)";
        TestResult result(suiteName, testName, 8, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,200,200,-1,1,5,1,0, -1, -1,false,0.8,100,120,0,1e-4);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test9() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved circle fiber error catch #6 - Invalid DRadius";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=5.0, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 120, dRadiusOfCurvature=0, accuracy=1e-4"
                                      "; Should return false because of invalid dRadius (negative)";
        TestResult result(suiteName, testName, 9, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,200,200,5,-1,5,1,0, -1, -1,false,0.8,100,120,0,1e-4);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test10() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved circle fiber error catch #7 - valid DRadius";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=0, avgLength=50.0, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 120, dRadiusOfCurvature=0, accuracy=1e-4"
                                      "; Should return true because of valid dRadius (zero)";
        TestResult result(suiteName, testName, 10, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,200,200,5,0,50,1,0, -1, -1,true,0.995,100,120,0,1e-2);
        input.print = false;

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test10.tiff",false);

        return result;
    }

    static TestResult test11() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved circle fiber error catch #7 - Invalid DRadius";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=5, avgLength=5.0, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 120, dRadiusOfCurvature=0, accuracy=1e-4"
                                      "; Should return false because of invalid dRadius (equal to the avgRadius)";
        TestResult result(suiteName, testName, 11, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,200,200,5,5,5,1,0, -1, -1,false,0.8,100,120,0,1e-4);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test12() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber error catch #1 - Invalid AvgLength";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=-1, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid AvgLength (Negative)";
        TestResult result(suiteName, testName, 12, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,-1,1,0, -1, -1,false,0.8,100,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test13() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber error catch #2 - Invalid AvgLength";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=0, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid AvgLength (0)";
        TestResult result(suiteName, testName, 13, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,0,1,0, -1, -1,false,0.8,100,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test14() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber error catch #3 - Invalid DLength";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=-1, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid DLength (Negative)";
        TestResult result(suiteName, testName, 14, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,-1,0, -1, -1,false,0.8,100,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test15() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber error catch #4 - Invalid DLength";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid DLength (0)";
        TestResult result(suiteName, testName, 15, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,50,0,0, -1, -1,true,0.9995,100,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test16() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber error catch #5 - Invalid DLength";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=5, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid DLength ( equal to avg )";
        TestResult result(suiteName, testName, 16, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,5,0, -1, -1,false,0.8,100,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test17() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber error catch #6 - Invalid angle type";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, angleType=-1, angle_variability=90, "
                                      "var_direction=0, intersect=false, poro=0.8, randomSeed=100, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid angletype (Negative)";
        TestResult result(suiteName, testName, 17, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,1,-1,30,0,false,0.8,100,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test18() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber error catch #7 - Valid 1D fibers";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, angleType=2, angle_variability=-1, "
                                      "var_direction=0, intersect=false, poro=0.8, randomSeed=100, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return true because all valid (0)";
        TestResult result(suiteName, testName, 18, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,100,1,2,-1,0,true,0.9,100,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test18.tiff",false);

        return result;
    }

    static TestResult test19() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber error catch #8 - Invalid angle_variability";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, angle_type=1, angle_variability=-1, "
                                      "direction=0, intersect=false, poro=0.8, randomSeed=100, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid angle_variability (negative)";
        TestResult result(suiteName, testName, 19, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,1,1,-1,0,false,0.8,100,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test20() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber error catch #9 - Invalid angle_variability";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, angle_type=1, angle_variability=100, "
                                      "direction=0, intersect=false, poro=0.8, randomSeed=100, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid angle_variability (0)";
        TestResult result(suiteName, testName, 20, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,100,1,1,90,0,true,0.9,100,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test20.tiff",false);

        return result;
    }

    static TestResult test21() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber error catch #10 - Invalid direction";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, angle_type=1, angle_variability=100, "
                                      "direction=-1, intersect=false, poro=0.8, randomSeed=100, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid direction (negative)";
        TestResult result(suiteName, testName, 21, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,1,1,30,-1,false,0.8,100,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test22() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber error catch #11 - invalid direction";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, type=2, var=-1, "
                                      "direction=-1, intersect=false, poro=0.8, randomSeed=100, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return true because of valid direction (0)";
        TestResult result(suiteName, testName, 22, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,100,1,2,-1,-1,true,0.9,100,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test22.tiff",false);

        return result;
    }

    static TestResult test24() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber error catch #2 - Invalid Porosity";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=-0.8, randomSeed=100, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid Porosity (Negative) ";
        TestResult result(suiteName, testName, 24, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,1,0, -1, -1,false,-0.8,100,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test25() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber error catch #3 - Invalid Porosity";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=1.8, randomSeed=100, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid Porosity (>1) ";
        TestResult result(suiteName, testName, 25, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,1,0, -1, -1,false,1.8,100,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test26() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber error catch #4 - Invalid Random Seed";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid random Seed (>16bit int Limit) ";
        TestResult result(suiteName, testName, 26, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,2100000000,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test27() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber error catch #5 - Invalid Random Seed";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      " intersect=false, poro=0.8, randomSeed=2.1 Billion, avgSmallradius=4, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid random Seed (Negative) ";
        TestResult result(suiteName, testName, 27, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,-1,4,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test28() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved circle fiber Specific error catch #1 - Invalid AvgRadiusOfCurvature";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=0, isotropic "
                                      ", intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 0, dRadiusOfCurvature=0, accuracy=1e-4"
                                      "; Should return false because of invalid AvgRadiusOfCurvature (0)";
        TestResult result(suiteName, testName, 28, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,200,200,5,0,200,0,0, -1, -1,true,0.8,100,0,0,1e-4);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test29() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved circle fiber Specific error catch #2 - Invalid AvgRadiusOfCurvature";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=5, isotropic "
                                      ", intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 101, dRadiusOfCurvature=0, accuracy=1e-4"
                                      "; Should return false because of invalid AvgRadiusOfCurvature ( < avgLength + dLength)";
        TestResult result(suiteName, testName, 29, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,200,200,5,0,200,5,0, -1, -1,true,0.8,100,101,0,1e-4);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test30() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved circle fiber Specific error catch #3 - Valid AvgRadiusOfCurvature";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=0, isotropic "
                                      ", intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 101, dRadiusOfCurvature=0, accuracy=1e-4"
                                      "; Should return true because of valid AvgRadiusOfCurvature ( > avgLength + dLength)";
        TestResult result(suiteName, testName, 30, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,200,200,5,0,200,0,0, -1, -1,true,0.999,100,101,0,1e-4);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test30.tiff",false);

        return result;
    }

    static TestResult test31() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved circle fiber Specific error catch #4 - Valid AvgRadiusOfCurvature";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=0, isotropic "
                                      ", intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 101, dRadiusOfCurvature=5, accuracy=1e-4"
                                      "; Should return false because of invalid AvgRadiusOfCurvature + dRadiusOfCurvature ( > avgLength + dLength)";
        TestResult result(suiteName, testName, 31, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,200,200,5,0,200,0,0, -1, -1,true,0.8,100,101,5,1e-4);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test32() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved circle fiber Specific error catch #5 - Invalid dRadiusOfCurvature";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=0, isotropic "
                                      ", intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 200, dRadiusOfCurvature=-1, accuracy=1e-4"
                                      "; Should return false because of Invalid dRadiusOfCurvature ( negative )";
        TestResult result(suiteName, testName, 32, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,200,200,5,0,200,0,0, -1, -1,true,0.8,100,200,-1,1e-4);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test33() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved circle fiber Specific error catch #5 - Invalid dRadiusOfCurvature";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=0, isotropic "
                                      ", intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 200, dRadiusOfCurvature=200, accuracy=1e-4"
                                      "; Should return false because of Invalid dRadiusOfCurvature ( =RadiusOfCurvature )";
        TestResult result(suiteName, testName, 33, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,200,200,5,0,200,0,0, -1, -1,true,0.8,100,200,200,1e-4);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test34() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved circle fiber Specific error catch #6 - Invalid Accuraccy";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=0, isotropic "
                                      ",intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 200, dRadiusOfCurvature=0, accuracy=0"
                                      "; Should return false because of Invalid Accuraccy ( 0 )";
        TestResult result(suiteName, testName, 34, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,200,200,5,0,200,0,0, -1, -1,true,0.8,100,200,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test35() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved circle fiber Specific error catch #7 - Invalid Accuraccy";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=0, isotropic "
                                      ", intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 200, dRadiusOfCurvature=0, accuracy=-1"
                                      "; Should return false because of Invalid Accuraccy ( -1 )";
        TestResult result(suiteName, testName, 35, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(200,200,200,5,0,200,0,0, -1, -1,true,0.8,100,200,0,-1);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test36() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber Specific error catch #1 - Invalid AvgSmallRadius";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=0, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid AvgSmallRadius (0)";
        TestResult result(suiteName, testName, 36, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,100,0,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test37() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber Specific error catch #2 - Invalid AvgSmallRadius";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid AvgSmallRadius (-1)";
        TestResult result(suiteName, testName, 37, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,100,-1,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test38() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber Specific error catch #3 - Invalid dSmallRadius";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid dSmallRadius (-1)";
        TestResult result(suiteName, testName, 38, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,100,5,-1,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test39() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber Specific error catch #4 - Invalid dSmallRadius";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid dSmallRadius ( = avgSmallRadius)";
        TestResult result(suiteName, testName, 39, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,100,5,5,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test40() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber Specific error catch #4 - valid avgNumSmallFibers";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return true because of valid avgNumSmallFibers ( = 0)";
        TestResult result(suiteName, testName, 40, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,100,1,0, -1, -1,false,0.999,100,5,0,0,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test40.tiff",false);

        return result;
    }

    static TestResult test41() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber Specific error catch #5 - invalid avgNumSmallFibers";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=-1, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid avgNumSmallFibers ( -1)";
        TestResult result(suiteName, testName, 41, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,100,5,0,-1,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test42() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber Specific error catch #6 - invalid dNumSmallFibers";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=-1, dPlacement=0"
                                      "; Should return false because of invalid dNumSmallFibers ( -1)";
        TestResult result(suiteName, testName, 42, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,100,5,0,5,-1,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test43() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber Specific error catch #7 - valid dNumSmallFibers";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=5, dPlacement=0"
                                      "; Should return true because of valid dNumSmallFibers (=avgNumSmallFibers)";
        TestResult result(suiteName, testName, 43, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,100,1,0, -1, -1,false,0.999,100,5,0,5,5,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test43.tiff",false);

        return result;
    }


    static TestResult test44() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower fiber Specific error catch #8 - invalid dNumSmallFibers";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=6, dPlacement=0"
                                      "; Should return false because of invalid dNumSmallFibers ( > avgNumSmallFibers)";
        TestResult result(suiteName, testName, 44, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,100,5,0,5,6,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test45() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #1";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=0, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid AvgSmallRadius (0)";
        TestResult result(suiteName, testName, 45, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,100,200,0,1e-4,0,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test46() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #2";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid AvgSmallRadius (-1)";
        TestResult result(suiteName, testName, 46, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,100,200,0,1e-4,-1,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test47() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #3";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ",intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid dSmallRadius (-1)";
        TestResult result(suiteName, testName, 47, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,100,200,0,1e-4,5,-1,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test48() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #4";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid dSmallRadius ( = avgSmallRadius)";
        TestResult result(suiteName, testName, 48, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.9,100,200,0,1e-4,5,5,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test49() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #5";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return true because of valid avgNumSmallFibers ( = 0)";
        TestResult result(suiteName, testName, 49, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(100,100,100,5,1,20,1,0, -1, -1,false,0.999,100,200,0,1e-2,5,0,0,0,0);


        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test49.tiff",false);

        return result;
    }

    static TestResult test50() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #6";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=-1, dNumSmallFibers=0, dPlacement=0"
                                      "; Should return false because of invalid avgNumSmallFibers ( -1)";
        TestResult result(suiteName, testName, 50, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,100,200,0,1e-4,5,0,-1,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test51() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #7";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=-1, dPlacement=0"
                                      "; Should return false because of invalid dNumSmallFibers ( -1)";
        TestResult result(suiteName, testName, 51, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,100,200,0,1e-4,5,0,5,-1,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test52() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #8";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=5, dPlacement=0"
                                      "; Should return true because of valid dNumSmallFibers (=avgNumSmallFibers)";
        TestResult result(suiteName, testName, 52, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,1,100,1,0, -1, -1,false,0.999,100,200,0,1e-4,5,0,5,5,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test52.tiff",false);

        return result;
    }


    static TestResult test53() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #9";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=1.0, avgLength=5, dLength=1.0, isotropic "
                                      ", intersect=false, poro=0.8, randomSeed=100, avgSmallradius=-1, dSmallRadius=0, avgNumSmallFibers=5, dNumSmallFibers=6, dPlacement=0"
                                      "; Should return false because of invalid dNumSmallFibers ( > avgNumSmallFibers)";
        TestResult result(suiteName, testName, 53, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,1,5,1,0, -1, -1,false,0.8,100,200,0,1e-4,5,0,5,6,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }


    static TestResult test54() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #10";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=0, isotropic "
                                      ", intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 0, dRadiusOfCurvature=0, accuracy=1e-4"
                                      "; Should return false because of invalid AvgRadiusOfCurvature (0)";
        TestResult result(suiteName, testName, 54, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,0,200,0,0, -1, -1,true,0.8,100,0,0,1e-4,5,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test55() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #11";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=5, isotropic "
                                      ", intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 101, dRadiusOfCurvature=0, accuracy=1e-4"
                                      "; Should return false because of invalid AvgRadiusOfCurvature ( < avgLength + dLength)";
        TestResult result(suiteName, testName, 55, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,0,200,5,0, -1, -1,true,0.8,100,101,0,1e-4,5,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test56() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #12";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=0, isotropic "
                                      ", intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 101, dRadiusOfCurvature=0, accuracy=1e-4"
                                      "; Should return true because of valid AvgRadiusOfCurvature ( > avgLength + dLength)";
        TestResult result(suiteName, testName, 56, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,0,200,0,0, -1, -1,true,0.999,100,101,0,1e-4,5,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test57() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #13";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=0, isotropic "
                                      ",intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 101, dRadiusOfCurvature=5, accuracy=1e-4"
                                      "; Should return false because of invalid AvgRadiusOfCurvature + dRadiusOfCurvature ( > avgLength + dLength)";
        TestResult result(suiteName, testName, 57, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,0,200,0,0, -1, -1,true,0.8,100,101,5,1e-4,5,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test58() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #14";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=0, isotropic "
                                      ",intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 200, dRadiusOfCurvature=-1, accuracy=1e-4"
                                      "; Should return false because of Invalid dRadiusOfCurvature ( negative )";
        TestResult result(suiteName, testName, 58, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,0,200,0,0, -1, -1,true,0.8,100,200,-1,1e-4,5,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test59() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #15";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=0, isotropic "
                                      ", intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 200, dRadiusOfCurvature=200, accuracy=1e-4"
                                      "; Should return false because of Invalid dRadiusOfCurvature ( =RadiusOfCurvature )";
        TestResult result(suiteName, testName, 59, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,0,200,0,0, -1, -1,true,0.8,100,200,200,1e-4,5,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test60() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #16";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=0, isotropic "
                                      ", intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 200, dRadiusOfCurvature=0, accuracy=0"
                                      "; Should return false because of Invalid Accuraccy ( 0 )";
        TestResult result(suiteName, testName, 60, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,0,200,0,0, -1, -1,true,0.8,100,200,0,0,5,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test61() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower fiber Specific error catch #17";
        std::string testDescription = "Generate_RandomFibers_Test: xsize=200, ysize=200, zsize=200, avgRadius=5.0, dRadius=-1, avgLength=200.0, dLength=0, isotropic "
                                      ", intersect=true, poro=0.8, randomSeed=100, avgRadiusOfCurvature = 200, dRadiusOfCurvature=0, accuracy=-1"
                                      "; Should return false because of Invalid Accuraccy ( -1 )";
        TestResult result(suiteName, testName, 61, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(200,200,200,5,0,200,0,0, -1, -1,true,0.8,100,200,0,-1,5,0,5,0,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(false,success,&result)) {
            return result;
        }

        return result;
    }

    static TestResult test62() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Circle Fiber - Nominal Behavior #1";
        std::string testDescription = " 200^3, slight preference for angle in XY. non-Intersecting, not-hollow";
        TestResult result(suiteName, testName, 62, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightCircle(200,200,200,5,0,200,0,1,15,2,false,0.9,100);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test62.tiff",false);

        return result;
    }

    static TestResult test63() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Circle Fiber - Nominal Behavior #2";
        std::string testDescription = " 200^3, slight preference for angle in XY. intersecting, not-hollow";
        TestResult result(suiteName, testName, 63, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightCircle(200,200,200,5,0,200,0,1,15,2,true,0.8,100);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test63.tiff",false);

        return result;
    }


    static TestResult test64() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Circle Fiber - Nominal Behavior #1";
        std::string testDescription = " 200^3, slight preference for angle in XY. non-Intersecting, not-hollow";
        TestResult result(suiteName, testName, 64, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(100,100,100,5,0,50,0,1,15,2,false,0.95,100,150,0,1e-3);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test64.tiff",false);

        return result;
    }

    static TestResult test65() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Circle Fiber - Nominal Behavior #2";
        std::string testDescription = " 200^3, slight preference for angle in XY. intersecting, not-hollow";
        TestResult result(suiteName, testName, 65, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedCircle(100,100,100,5,0,50,0,1,15,2,true,0.98,100,150,0,1e-3);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test65.tiff",false);

        return result;
    }

    static TestResult test66() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower Fiber - Nominal Behavior #1";
        std::string testDescription = " 200^3, slight preference for angle in XY. non-Intersecting, not-hollow";
        TestResult result(suiteName, testName, 66, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,0,200,0,1,15,2,false,0.98,100,4,1,5,2,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test66.tiff",false);

        return result;
    }

    static TestResult test67() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower Fiber - Nominal Behavior #1";
        std::string testDescription = " 200^3, slight preference for angle in XY. non-Intersecting, hollow";
        TestResult result(suiteName, testName, 67, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower_Hollow(200,200,200,5,0,200,0,1,15,2,false,0.98,100,4,1,5,2,0,1,2.5,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test67.tiff",false);

        return result;
    }

    static TestResult test68() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower Fiber - Nominal Behavior #1";
        std::string testDescription = " 200^3, slight preference for angle in XY. Intersecting, not-hollow";
        TestResult result(suiteName, testName, 68, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower(200,200,200,5,0,200,0,1,15,2,true,0.98,100,4,1,5,2,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test68.tiff",false);

        return result;
    }

    static TestResult test69() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Straight Flower Fiber - Nominal Behavior #1";
        std::string testDescription = " 200^3, slight preference for angle in XY. Intersecting, hollow";
        TestResult result(suiteName, testName, 69, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.straightFlower_Hollow(200,200,200,5,0,200,0,1,15,2,true,0.98,100,4,1,5,2,0,1,2.5,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test69.tiff",false);

        return result;
    }


    static TestResult test70() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower Fiber - Nominal Behavior #1";
        std::string testDescription = " 100^3, slight preference for angle in XY. non-Intersecting, not-hollow";
        TestResult result(suiteName, testName, 70, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(100,100,100,5,0,100,0,1,15,2,false,0.98,100,120,0,1e-2,4,1,5,2,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test70.tiff",false);

        return result;
    }

    static TestResult test71() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower Fiber - Nominal Behavior #1";
        std::string testDescription = " 100^3, slight preference for angle in XY. non-Intersecting, hollow";
        TestResult result(suiteName, testName, 71, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower_Hollow(100,100,100,5,0,100,0,1,15,2,false,0.98,100,120,0,1e-2,4,1,5,2,0,1,2.5,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test71.tiff",false);

        return result;
    }

    static TestResult test72() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower Fiber - Nominal Behavior #1";
        std::string testDescription = " 100^3, slight preference for angle in XY. Intersecting, not-hollow";
        TestResult result(suiteName, testName, 72, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower(100,100,100,5,0,100,0,1,15,2,true,0.98,100,120,0,1e-2,4,1,5,2,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test72.tiff",false);

        return result;
    }

    static TestResult test73() {

        std::string suiteName = "Generate_RandomFibers_Test";
        std::string testName = "Curved Flower Fiber - Nominal Behavior #1";
        std::string testDescription = " 100^3, slight preference for angle in XY. Intersecting, hollow";
        TestResult result(suiteName, testName, 73, testDescription);

        puma::Workspace grayWS(1e-6,false);

        RandomFibersInput input;
        input.curvedFlower_Hollow(100,100,100,5,0,100,0,1,15,2,true,0.98,100,120,0,1e-2,4,1,5,2,0,1,2.5,0);

        input.print = false; bool success = puma::generateRandomFibers(&grayWS, input);

        if(!assertEquals(true,success,&result)) {
            return result;
        }

        puma::export_3DTiff(&grayWS,"cpp/test/out/tiff/RandomFibers_Test73.tiff",false);

        return result;
    }





};
