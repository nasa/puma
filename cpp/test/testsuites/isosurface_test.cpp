#include "../testframework/subtest.h"
#include "puma.h"

#include <vector>
#include <iostream>


class IsoSurface_Test : public SubTest {
public:

    IsoSurface_Test() {
        testSuiteName = "IsoSurface_Test";

        tests.push_back(test1);
        //tests.push_back(test2);
        tests.push_back(test3);
        tests.push_back(test4);
        tests.push_back(test5);
        tests.push_back(test6);
        tests.push_back(test7);
        tests.push_back(test8);
        tests.push_back(test9);
        //tests.push_back(test10);
        tests.push_back(test11);
        tests.push_back(test12);
        tests.push_back(test13);
        tests.push_back(test14);
        //tests.push_back(test15);
        //tests.push_back(test16);
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
        tests.push_back(test33);
        tests.push_back(test34);
        tests.push_back(test35);
        //tests.push_back(test36);
        //tests.push_back(test37);
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
    }


    static TestResult test1() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "IsoSurfaceHelper::InterpolateVerts - Test 1";
        std::string testDescription = "Empty Matrix";
        TestResult result(suiteName, testName, 1, testDescription);

        puma::Matrix<short> testMatrix;
        puma::Matrix<float> newMatrix;

        bool success = IsoSurfaceHelper::InterpolateVerts(&testMatrix,&newMatrix,8);

        if(!assertEquals(false,success, &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)testMatrix.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)newMatrix.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "IsoSurfaceHelper::InterpolateVerts - Test 2";
        std::string testDescription = "Constant Value";
        TestResult result(suiteName, testName, 2, testDescription);

        puma::Matrix<short> testMatrix(1300,1300,1300,10);
        puma::Matrix<float> newMatrix;

        bool success = IsoSurfaceHelper::InterpolateVerts(&testMatrix,&newMatrix,8);

        if(!assertEquals(true,success, &result)) {
            return result;
        }

        if(!assertEquals((long)(1300+1)*(long)(1300+1)*(long)(1300+1),(long)newMatrix.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)(1300)*(long)(1300)*(long)(1300),(long)testMatrix.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)10,newMatrix.average(), &result)) {
            return result;
        }

        if(!assertEquals((double)10,testMatrix.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "IsoSurfaceHelper::InterpolateVerts - Test 3";
        std::string testDescription = "Block in Series";
        TestResult result(suiteName, testName, 3, testDescription);

        puma::Matrix<short> testMatrix(100,50,25,10);
        testMatrix.set(0,49,0,49,0,24,1);

        puma::Matrix<float> newMatrix;

        bool success = IsoSurfaceHelper::InterpolateVerts(&testMatrix,&newMatrix,8);

        if(!assertEquals(true,success, &result)) {
            return result;
        }

        if(!assertEquals((long)101*51*26,newMatrix.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*50*25,testMatrix.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)5.5,newMatrix.average(), &result)) {
            return result;
        }

        if(!assertEquals((double)5.5,testMatrix.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test4() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "IsoSurfaceHelper::InterpolateVerts - Test 4";
        std::string testDescription = "Sphere";
        TestResult result(suiteName, testName, 4, testDescription);

        puma::Workspace grayWS(1e-6,false);

        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),10);

        puma::Matrix<float> newMatrix;

        bool success = IsoSurfaceHelper::InterpolateVerts(&grayWS.matrix,&newMatrix,8);

        if(!assertEquals(true,success, &result)) {
            return result;
        }

        if(!assertEquals((long)101*101*101,newMatrix.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.matrix.size(), &result)) {
            return result;
        }


        if(!assertEquals((double)99.374211,newMatrix.average(), &result)) {
            return result;
        }

        if(!assertEquals((double)100.811828,grayWS.matrix.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "IsoSurfaceHelper::InterpolateVerts - Test 5";
        std::string testDescription = "Sphere of X != Y != Z";
        TestResult result(suiteName, testName, 5, testDescription);


        puma::Workspace grayWS(1e-6,false);

        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),10);
        grayWS.crop(0,98,0,97,1,96);

        puma::Matrix<float> newMatrix;

        bool success = IsoSurfaceHelper::InterpolateVerts(&grayWS.matrix,&newMatrix,8);

        if(!assertEquals(true,success, &result)) {
            return result;
        }

        if(!assertEquals((long)100*99*97,newMatrix.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)99*98*96,grayWS.matrix.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)102.819277,newMatrix.average(), &result)) {
            return result;
        }

        if(!assertEquals((double)104.25627,grayWS.matrix.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test6() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "IsoSurfaceHelper::DownScale - Test 1";
        std::string testDescription = "Empty Matrix";
        TestResult result(suiteName, testName, 6, testDescription);

        puma::Matrix<short> testMatrix;

        puma::Matrix<float> newMatrix;

        bool success = IsoSurfaceHelper::DownScale(&testMatrix,&newMatrix,2,8);

        if(!assertEquals(false,success, &result)) {
            return result;
        }

        if(!assertEquals((long)0,newMatrix.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,testMatrix.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test7() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "IsoSurfaceHelper::DownScale - Test 2";
        std::string testDescription = "Factor = 0";
        TestResult result(suiteName, testName, 7, testDescription);

        puma::Matrix<short> testMatrix(100,100,100,10);

        puma::Matrix<float> newMatrix;

        bool success = IsoSurfaceHelper::DownScale(&testMatrix,&newMatrix,0,8);

        if(!assertEquals(false, success, &result)) {
            return result;
        }

        if(!assertEquals((long)0,newMatrix.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,testMatrix.size(), &result)) {
            return result;
        }

        return result;

    }

    static TestResult test8() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "IsoSurfaceHelper::DownScale - Test 3";
        std::string testDescription = "negative factor";
        TestResult result(suiteName, testName, 8, testDescription);

        puma::Matrix<short> testMatrix(100,100,100,10);

        puma::Matrix<float> newMatrix;

        bool success = IsoSurfaceHelper::DownScale(&testMatrix,&newMatrix,-1,8);

        if(!assertEquals(false, success, &result)) {
            return result;
        }

        if(!assertEquals((long)0,newMatrix.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,testMatrix.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test9() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "IsoSurfaceHelper::DownScale - Test 4";
        std::string testDescription = "Downsample with %factor = 0 and X=Y=Z";
        TestResult result(suiteName, testName, 9, testDescription);

        puma::Workspace grayWS(1e-6,false);

        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),10);
        puma::Matrix<float> newMatrix;

        bool success = IsoSurfaceHelper::DownScale(&grayWS.matrix,&newMatrix, 4,8);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((long)25*25*25,newMatrix.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.matrix.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)100.59072, newMatrix.average(), &result)) {
            return result;
        }

        if(!assertEquals((double)100.811828, grayWS.matrix.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test10() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "IsoSurfaceHelper::DownScale - Test 5";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        puma::Matrix<short> testMatrix(1300,1299,1298);
#pragma omp parallel for
        for(long i = 0;i< 1300; i++) {
            for(long j = 0;j< 1299; j++) {
                for(long k = 0;k< 1298; k++) {
                    testMatrix(i,j,k) = i/10;
                }
            }
        }

        puma::Matrix<float> newMatrix;

        bool success = IsoSurfaceHelper::DownScale(&testMatrix,&newMatrix, 4,8);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((long)1300*1299*1298,testMatrix.size(), &result)) {
            return result;
        }

        long newX = 1300/4;
        long newY = 1299/4;
        long newZ = 1298/4;

        if(!assertEquals(newX*newY*newZ,newMatrix.size(), &result)) {
            return result;
        }

        if(!assertEquals((double)64.4, newMatrix.average(), &result)) {
            return result;
        }

        if(!assertEquals((double)64.5, testMatrix.average(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test11() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 1";
        std::string testDescription = "Empty Matrix, emptry triangles";
        TestResult result(suiteName, testName, 11, testDescription);

        puma::Workspace grayWS(1e-6,false);
        std::vector< puma::Triangle<float> > tris;

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(0,99),true,0,true,false);

        if(!assertEquals(false, success, &result)) {
            return result;
        }

        if(!assertEquals((int)0,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test12() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 2";
        std::string testDescription = "Empty Matrix, Existing Triangles";
        TestResult result(suiteName, testName, 12, testDescription);

        puma::Workspace grayWS(1e-6,false);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(0,99),true,0,true,false);

        if(!assertEquals(false, success, &result)) {
            return result;
        }

        if(!assertEquals((int)0,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test13() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 3";
        std::string testDescription = "Box with closed edges";
        TestResult result(suiteName, testName, 13, testDescription);

        puma::Workspace grayWS(100,100,100,255, 1e-6,false);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(128,255),true,1,false,10);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)119996,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test14() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 4";
        std::string testDescription = "Box with Open Edges";
        TestResult result(suiteName, testName, 14, testDescription);

        puma::Workspace grayWS(100,100,100,255, 1e-6,false);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(128,255),false,1,false,10);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)0,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test15() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 5";
        std::string testDescription = "1300^3 box with closed edges";
        TestResult result(suiteName, testName, 15, testDescription);

        puma::Workspace grayWS(1300,1300,1300,255, 1e-6,false);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(128,255),true,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)20279996,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)1300*1300*1300,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test16() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 6";
        std::string testDescription = "1300^3 box with open edges";
        TestResult result(suiteName, testName, 16, testDescription);

        puma::Workspace grayWS(1300,1300,1300,255, 1e-6,false);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(128,255),false,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)0,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)1300*1300*1300,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test17() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 7";
        std::string testDescription = "Sphere with X = Y = Z";
        TestResult result(suiteName, testName, 17, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(128,255),true,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)59384,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test18() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 8";
        std::string testDescription = "Sphere with X!= Y != Z";
        TestResult result(suiteName, testName, 18, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        grayWS.crop(0,99,0,98,1,97);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(128,255),false,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)59384,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*99*97,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test19() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 9";
        std::string testDescription = "Cylinder with Closed Edges";
        TestResult result(suiteName, testName, 19, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_cylinder_r40.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(128,255),true,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)83868,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test20() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 10";
        std::string testDescription = "Cylinder with Open Edges";
        TestResult result(suiteName, testName, 20, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_cylinder_r40.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(128,255),false,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)63360,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test21() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 11";
        std::string testDescription = "Outisde of Cylinder (capillary) with closed Edges";
        TestResult result(suiteName, testName, 21, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_cylinder_r40.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(0,127),true,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)164128,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test22() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 12";
        std::string testDescription = "Outisde of Cylinder (capillary) with open edges";
        TestResult result(suiteName, testName, 22, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_cylinder_r40.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(0,127),false,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)63360,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test23() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 13";
        std::string testDescription = "Outisde of Cylinder (capillary) with closed edges";
        TestResult result(suiteName, testName, 23, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_cylinder_r40.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(0,127),true,1,true,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)166480,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test24() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 14";
        std::string testDescription = "FiberForm with open edges";
        TestResult result(suiteName, testName, 24, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(90,255),false,1,false,0);


        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)1222755,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }


        return result;
    }

    static TestResult test25() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 15";
        std::string testDescription = "FiberForm with closed edges";
        TestResult result(suiteName, testName, 25, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(90,255),true,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)1330756,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test26() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 16";
        std::string testDescription = "FiberForm with open edges - interpolated verts";
        TestResult result(suiteName, testName, 26, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(90,255),false,1,true,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)1212147,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test27() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 17";
        std::string testDescription = "FiberForm with closed edges - interpolatedVerts";
        TestResult result(suiteName, testName, 27, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(90,255),true,1,true,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)1321224,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test28() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 18";
        std::string testDescription = "FiberForm with open edges - interpolated verts, factor of 2";
        TestResult result(suiteName, testName, 28, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(90,255),false,2,true,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)291947,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test29() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 19";
        std::string testDescription = "FiberForm with closed edges - interpolatedVerts, factor of 2";
        TestResult result(suiteName, testName, 29, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(90,255),true,2,true,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)321800,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test30() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 20";
        std::string testDescription = "FiberForm with open edges, factor of 2";
        TestResult result(suiteName, testName, 30, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(90,255),false,2,false,0);


        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)297445,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test31() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 21";
        std::string testDescription = "FiberForm with closed edges, factor of 2";
        TestResult result(suiteName, testName, 31, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(90,255),true,2,false,0);


        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)327300,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test32() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 1";
        std::string testDescription = "Empty Matrix, emptry triangles";
        TestResult result(suiteName, testName, 32, testDescription);

        puma::Workspace segWS(1e-6,false);
        std::vector< puma::Triangle<float> > tris;

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(0,0),true,0,true,false);

        if(!assertEquals(false, success, &result)) {
            return result;
        }

        if(!assertEquals((int)0,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,segWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test33() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 2";
        std::string testDescription = "Empty Matrix, Existing Triangles";
        TestResult result(suiteName, testName, 33, testDescription);

        puma::Workspace segWS(1e-6,false);
        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(0,0),true,0,true,false);

        if(!assertEquals(false, success, &result)) {
            return result;
        }

        if(!assertEquals((int)0,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,segWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test34() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 3";
        std::string testDescription = "Box with closed edges";
        TestResult result(suiteName, testName, 34, testDescription);

        puma::Workspace grayWS(100,100,100,255, 1e-6,false);
        grayWS.setMaterialID(puma::Cutoff(255,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(1,1),true,1,false);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)119996,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test35() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 4";
        std::string testDescription = "Box with Open Edges";
        TestResult result(suiteName, testName, 35, testDescription);

        puma::Workspace grayWS(100,100,100,255, 1e-6,false);
        grayWS.setMaterialID(puma::Cutoff(255,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(1,1),false,1,false);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)0,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test36() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 5";
        std::string testDescription = "1300^3 box with closed edges";
        TestResult result(suiteName, testName, 36, testDescription);

        puma::Workspace grayWS(1300,1300,1300,255, 1e-6,false);
        grayWS.setMaterialID(puma::Cutoff(255,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(1,1),true,1,false);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)20279996,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)1300*1300*1300,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test37() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 6";
        std::string testDescription = "1300^3 box with open edges";
        TestResult result(suiteName, testName, 37, testDescription);

        puma::Workspace grayWS(1300,1300,1300,255, 1e-6,false);
        grayWS.setMaterialID(puma::Cutoff(255,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&grayWS,puma::Cutoff(1,1),false,1,false);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)0,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)1300*1300*1300,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test38() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 7";
        std::string testDescription = "Sphere with X = Y = Z";
        TestResult result(suiteName, testName, 38, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),0);


        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(128,255),2);


        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(1,2),true,1,false);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)59384,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,segWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test39() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 8";
        std::string testDescription = "Sphere with X!= Y != Z";
        TestResult result(suiteName, testName, 39, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_sphere_r40.tif"),0);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(128,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        grayWS.crop(0,99,0,98,1,97);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(1,1),false,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)59384,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*99*97,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test40() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 9";
        std::string testDescription = "Cylinder with Closed Edges";
        TestResult result(suiteName, testName, 40, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_cylinder_r40.tif"),0);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(128,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(1,1),true,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)83868,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test41() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 10";
        std::string testDescription = "Cylinder with Open Edges";
        TestResult result(suiteName, testName,41, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_cylinder_r40.tif"),0);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(128,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(1,1),false,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)63360,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test42() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 11";
        std::string testDescription = "Outisde of Cylinder (capillary) with closed Edges";
        TestResult result(suiteName, testName, 42, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_cylinder_r40.tif"),0);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(128,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(0,0),true,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)164128,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test43() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 12";
        std::string testDescription = "Outisde of Cylinder (capillary) with open edges";
        TestResult result(suiteName, testName, 43, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_cylinder_r40.tif"),0);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(128,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(0,0),false,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)63360,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test44() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 13";
        std::string testDescription = "Outisde of Cylinder (capillary) with closed edges";
        TestResult result(suiteName, testName, 44, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_cylinder_r40.tif"),0);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(128,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(0,0),true,1,true,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)166640,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100*100,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test45() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 14";
        std::string testDescription = "FiberForm with open edges";
        TestResult result(suiteName, testName, 45, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(90,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(1,1),false,1,false,0);


        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)1222755,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }


        return result;
    }

    static TestResult test46() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 15";
        std::string testDescription = "FiberForm with closed edges";
        TestResult result(suiteName, testName, 46, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(90,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(1,1),true,1,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)1330756,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test47() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 16";
        std::string testDescription = "FiberForm with open edges - interpolated verts";
        TestResult result(suiteName, testName, 47, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(90,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(1,1),false,1,true,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)1219769,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test48() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 17";
        std::string testDescription = "FiberForm with closed edges - interpolatedVerts";
        TestResult result(suiteName, testName, 48, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(90,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(1,1),true,1,true,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)1333196,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test49() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 18";
        std::string testDescription = "FiberForm with open edges - interpolated verts, factor of 2";
        TestResult result(suiteName, testName, 49, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(90,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(1,1),false,2,true,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)293417,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test50() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 19";
        std::string testDescription = "FiberForm with closed edges - interpolatedVerts, factor of 2";
        TestResult result(suiteName, testName, 50, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(90,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(1,1),true,2,true,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)324208,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test51() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 20";
        std::string testDescription = "FiberForm with open edges, factor of 2";
        TestResult result(suiteName, testName, 51, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(90,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(1,1),false,2,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)297445,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test52() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (MaterialsWorkspace) - Test 21";
        std::string testDescription = "FiberForm with closed edges, factor of 2";
        TestResult result(suiteName, testName, 52, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("200_fiberform.tif"),0);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(90,255),1);

        std::vector< puma::Triangle<float> > tris(100);

        bool success = puma::isosurface_MarchingCubes(&tris,&segWS,puma::Cutoff(1,1),true,2,false,0);

        if(!assertEquals(true, success, &result)) {
            return result;
        }

        if(!assertEquals((int)327300,(int)tris.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)200*200*200,grayWS.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test53() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 53";
        std::string testDescription = "Tests images with large jumps";
        TestResult result(suiteName, testName, 53, testDescription);

        puma::Workspace grayWS1(3,3,3,0,1e-6,false);
        grayWS1.matrix(1,1,1) = 255;

        puma::Workspace grayWS2(3,3,3,255,1e-6,false);
        grayWS2.matrix(1,1,1) = 0;

        std::vector< puma::Triangle<float> > tris1(3);
        std::vector< puma::Triangle<float> > tris2(3);

        bool success1 = puma::isosurface_MarchingCubes(&tris1,&grayWS1,puma::Cutoff(255,255),true,1,false,0);
        bool success2 = puma::isosurface_MarchingCubes(&tris2,&grayWS2,puma::Cutoff(0,0),true,1,false,0);

        if(!assertEquals(true, success1, &result)) {
            return result;
        }

        if(!assertEquals(true, success2, &result)) {
            return result;
        }

        if(!assertEquals((int)tris1.size(),(int)tris2.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test54() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 54";
        std::string testDescription = "Tests images with large jumps, interpreted vertices";
        TestResult result(suiteName, testName, 54, testDescription);

        puma::Workspace grayWS1(3,3,3,0,1e-6,false);
        grayWS1.matrix(1,1,1) = 255;

        puma::Workspace grayWS2(3,3,3,255,1e-6,false);
        grayWS2.matrix(1,1,1) = 0;

        std::vector< puma::Triangle<float> > tris1(3);
        std::vector< puma::Triangle<float> > tris2(3);

        bool success1 = puma::isosurface_MarchingCubes(&tris1,&grayWS1,puma::Cutoff(255,255),true,1,true,0);
        bool success2 = puma::isosurface_MarchingCubes(&tris2,&grayWS2,puma::Cutoff(0,0),true,1,true,0);

        if(!assertEquals(true, success1, &result)) {
            return result;
        }

        if(!assertEquals(true, success2, &result)) {
            return result;
        }

        if(!assertEquals((int)tris1.size(),(int)tris2.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test55() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 55";
        std::string testDescription = "Tests images with large jumps";
        TestResult result(suiteName, testName, 55, testDescription);

        puma::Workspace segWS1(3,3,3,0,1e-6,false);
        segWS1.matrix(1,1,1) = 255;

        puma::Workspace segWS2(3,3,3,255,1e-6,false);
        segWS2.matrix(1,1,1) = 0;

        std::vector< puma::Triangle<float> > tris1(3);
        std::vector< puma::Triangle<float> > tris2(3);

        bool success1 = puma::isosurface_MarchingCubes(&tris1,&segWS1,puma::Cutoff(255,255),true,1,false,0);
        bool success2 = puma::isosurface_MarchingCubes(&tris2,&segWS2,puma::Cutoff(0,0),true,1,false,0);

        if(!assertEquals(true, success1, &result)) {
            return result;
        }

        if(!assertEquals(true, success2, &result)) {
            return result;
        }

        if(!assertEquals((int)tris1.size(),(int)tris2.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test56() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 56";
        std::string testDescription = "Tests images with large jumps, interpreted vertices";
        TestResult result(suiteName, testName, 56, testDescription);

        puma::Workspace segWS1(3,3,3,0,1e-6,false);
        segWS1.matrix(1,1,1) = 255;

        puma::Workspace segWS2(3,3,3,255,1e-6,false);
        segWS2.matrix(1,1,1) = 0;

        std::vector< puma::Triangle<float> > tris1(3);
        std::vector< puma::Triangle<float> > tris2(3);

        bool success1 = puma::isosurface_MarchingCubes(&tris1,&segWS1,puma::Cutoff(255,255),true,1,true,0);
        bool success2 = puma::isosurface_MarchingCubes(&tris2,&segWS2,puma::Cutoff(0,0),true,1,true,0);

        if(!assertEquals(true, success1, &result)) {
            return result;
        }

        if(!assertEquals(true, success2, &result)) {
            return result;
        }

        if(!assertEquals((int)tris1.size(),(int)tris2.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test57() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 57";
        std::string testDescription = "Tests that marching cubes interprets segmented workspaces correctly";
        TestResult result(suiteName, testName, 57, testDescription);

        puma::Workspace segWS1(3,3,3,0,1e-6,false);
        segWS1.matrix(1,1,1) = 255;

        puma::Workspace segWS2(3,3,3,1,1e-6,false);
        segWS2.matrix(1,1,1) = 0;

        puma::Workspace grayWS(3,3,3,0,1e-6,false);
        grayWS.matrix(1,1,1) = 1;

        std::vector< puma::Triangle<float> > tris1(3);
        std::vector< puma::Triangle<float> > tris2(3);
        std::vector< puma::Triangle<float> > tris3(3);

        bool success1 = puma::isosurface_MarchingCubes(&tris1,&segWS1,puma::Cutoff(255,255),true,1,false,0);
        bool success2 = puma::isosurface_MarchingCubes(&tris2,&segWS2,puma::Cutoff(0,0),true,1,false,0);
        bool success3 = puma::isosurface_MarchingCubes(&tris3,&grayWS,puma::Cutoff(1,1),true,1,false,0);

        if(!assertEquals(true, success1, &result)) {
            return result;
        }

        if(!assertEquals(true, success2, &result)) {
            return result;
        }

        if(!assertEquals(true, success3, &result)) {
            return result;
        }

        if(!assertEquals((int)tris1.size(),(int)tris2.size(), &result)) {
            return result;
        }

        if(!assertEquals((int)tris2.size(),(int)tris3.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test58() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 58";
        std::string testDescription = "Tests that marching cubes interprets segmented workspaces correctly with interpolated vertices";
        TestResult result(suiteName, testName, 58, testDescription);

        puma::Workspace segWS1(3,3,3,0,1e-6,false);
        segWS1.matrix(1,1,1) = 255;

        puma::Workspace segWS2(3,3,3,1,1e-6,false);
        segWS2.matrix(1,1,1) = 0;

        puma::Workspace grayWS(3,3,3,0,1e-6,false);
        grayWS.matrix(1,1,1) = 1;

        std::vector< puma::Triangle<float> > tris1(3);
        std::vector< puma::Triangle<float> > tris2(3);
        std::vector< puma::Triangle<float> > tris3(3);

        bool success1 = puma::isosurface_MarchingCubes(&tris1,&segWS1,puma::Cutoff(255,255),true,1,true,0);
        bool success2 = puma::isosurface_MarchingCubes(&tris2,&segWS2,puma::Cutoff(0,0),true,1,true,0);
        bool success3 = puma::isosurface_MarchingCubes(&tris3,&grayWS,puma::Cutoff(1,1),true,1,true,0);

        if(!assertEquals(true, success1, &result)) {
            return result;
        }

        if(!assertEquals(true, success2, &result)) {
            return result;
        }

        if(!assertEquals(true, success3, &result)) {
            return result;
        }

        if(!assertEquals((int)tris1.size(),(int)tris2.size(), &result)) {
            return result;
        }

        if(!assertEquals((int)tris2.size(),(int)tris3.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test59() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 59";
        std::string testDescription = "Tests images with large jumps";
        TestResult result(suiteName, testName, 59, testDescription);

        puma::Workspace grayWS1(3,3,3,0,1e-6,false);
        grayWS1.matrix(1,1,1) = 255;

        puma::Workspace grayWS2(3,3,3,255,1e-6,false);
        grayWS2.matrix(1,1,1) = 0;

        std::vector< puma::Triangle<float> > tris1(3);
        std::vector< puma::Triangle<float> > tris2(3);

        bool success1 = puma::isosurface_MarchingCubes(&tris1,&grayWS1,puma::Cutoff(128,255),true,1,false,0);
        bool success2 = puma::isosurface_MarchingCubes(&tris2,&grayWS2,puma::Cutoff(0,127),true,1,false,0);

        if(!assertEquals(true, success1, &result)) {
            return result;
        }

        if(!assertEquals(true, success2, &result)) {
            return result;
        }

        if(!assertEquals((int)tris1.size(),(int)tris2.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test60() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 60";
        std::string testDescription = "Tests images with large jumps, interpreted vertices";
        TestResult result(suiteName, testName, 60, testDescription);

        puma::Workspace grayWS1(3,3,3,0,1e-6,false);
        grayWS1.matrix(1,1,1) = 255;

        puma::Workspace grayWS2(3,3,3,255,1e-6,false);
        grayWS2.matrix(1,1,1) = 0;

        std::vector< puma::Triangle<float> > tris1(3);
        std::vector< puma::Triangle<float> > tris2(3);

        bool success1 = puma::isosurface_MarchingCubes(&tris1,&grayWS1,puma::Cutoff(128,255),true,1,true,0);
        bool success2 = puma::isosurface_MarchingCubes(&tris2,&grayWS2,puma::Cutoff(0,127),true,1,true,0);

        if(!assertEquals(true, success1, &result)) {
            return result;
        }

        if(!assertEquals(true, success2, &result)) {
            return result;
        }

        if(!assertEquals((int)tris1.size(),(int)tris2.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test61() {
        std::string suiteName = "IsoSurface_Test";
        std::string testName = "puma::MarchingCubes (Workspace) - Test 61";
        std::string testDescription = "Testing number of triangles generated";
        TestResult result(suiteName, testName, 61, testDescription);

        puma::Workspace grayWS(1e-6,false);
        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_cylinder_r40.tif"));

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(0,127),0);
        segWS.setMaterialID(&grayWS,puma::Cutoff(128,255),1);

        std::vector< puma::Triangle<float> > gray_solid_open, gray_solid_closed, gray_void_open, gray_void_closed;

        puma::isosurface_MarchingCubes(&gray_solid_open,&grayWS,puma::Cutoff(128,255),false,1,false,0);
        puma::isosurface_MarchingCubes(&gray_solid_closed,&grayWS,puma::Cutoff(128,255),true,1,false,0);
        puma::isosurface_MarchingCubes(&gray_void_open,&grayWS,puma::Cutoff(0,127),false,1,false,0);
        puma::isosurface_MarchingCubes(&gray_void_closed,&grayWS,puma::Cutoff(0,127),true,1,false,0);
        //puma::export_STL(&gray_solid_open, false, "cpp/test/out/stl/stltest/gray_solid_open.stl");
        //puma::export_STL(&gray_solid_closed, false, "cpp/test/out/stl/stltest/gray_solid_closed.stl");
        //puma::export_STL(&gray_void_open, false, "cpp/test/out/stl/stltest/gray_void_open.stl");
        //puma::export_STL(&gray_void_closed, false, "cpp/test/out/stl/stltest/gray_void_closed.stl");

        if(!assertEquals(63360,gray_solid_open.size(), &result)) {
            return result;
        }
        if(!assertEquals(83868,gray_solid_closed.size(), &result)) {
            return result;
        }
        if(!assertEquals(63360,gray_void_open.size(), &result)) {
            return result;
        }
        if(!assertEquals(164128,gray_void_closed.size(), &result)) {
            return result;
        }


        std::vector< puma::Triangle<float> > seg_solid_open, seg_solid_closed, seg_void_open, seg_void_closed;

        puma::isosurface_MarchingCubes(&seg_solid_open,&segWS,puma::Cutoff(1,1),false,1,false,0);
        puma::isosurface_MarchingCubes(&seg_solid_closed,&segWS,puma::Cutoff(1,1),true,1,false,0);
        puma::isosurface_MarchingCubes(&seg_void_open,&segWS,puma::Cutoff(0,0),false,1,false,0);
        puma::isosurface_MarchingCubes(&seg_void_closed,&segWS,puma::Cutoff(0,0),true,1,false,0);
        //puma::export_STL(&seg_solid_open, false, "cpp/test/out/stl/stltest/seg_solid_open.stl");
        //puma::export_STL(&seg_solid_closed, false, "cpp/test/out/stl/stltest/seg_solid_closed.stl");
        //puma::export_STL(&seg_void_open, false, "cpp/test/out/stl/stltest/seg_void_open.stl");
        //puma::export_STL(&seg_void_closed, false, "cpp/test/out/stl/stltest/seg_void_closed.stl");

        if(!assertEquals(63360,seg_solid_open.size(), &result)) {
            return result;
        }
        if(!assertEquals(83868,seg_solid_closed.size(), &result)) {
            return result;
        }
        if(!assertEquals(63360,seg_void_open.size(), &result)) {
            return result;
        }
        if(!assertEquals(164128,seg_void_closed.size(), &result)) {
            return result;
        }


        std::vector< puma::Triangle<float> > gray_solid_open_lew, gray_solid_closed_lew, gray_void_open_lew, gray_void_closed_lew;

        puma::isosurface_LewinerMarchingCubes(&gray_solid_open_lew,&grayWS,puma::Cutoff(128,255),false,1,false,0);
        puma::isosurface_LewinerMarchingCubes(&gray_solid_closed_lew,&grayWS,puma::Cutoff(128,255),true,1,false,0);
        puma::isosurface_LewinerMarchingCubes(&gray_void_open_lew,&grayWS,puma::Cutoff(0,127),false,1,false,0);
        puma::isosurface_LewinerMarchingCubes(&gray_void_closed_lew,&grayWS,puma::Cutoff(0,127),true,1,false,0);
        //puma::export_STL(&gray_solid_open_lew, false, "cpp/test/out/stl/stltest/gray_solid_open_lew.stl");
        //puma::export_STL(&gray_solid_closed_lew, false, "cpp/test/out/stl/stltest/gray_solid_closed_lew.stl");
        //puma::export_STL(&gray_void_open_lew, false, "cpp/test/out/stl/stltest/gray_void_open_lew.stl");
        //puma::export_STL(&gray_void_closed_lew, false, "cpp/test/out/stl/stltest/gray_void_closed_lew.stl");

        if(!assertEquals(63360,gray_solid_open_lew.size(), &result)) {
            return result;
        }
        if(!assertEquals(83868,gray_solid_closed_lew.size(), &result)) {
            return result;
        }
        if(!assertEquals(63360,gray_void_open_lew.size(), &result)) {
            return result;
        }
        if(!assertEquals(164128,gray_void_closed_lew.size(), &result)) {
            return result;
        }


        std::vector< puma::Triangle<float> > seg_solid_open_lew, seg_solid_closed_lew, seg_void_open_lew, seg_void_closed_lew;

        puma::isosurface_LewinerMarchingCubes(&seg_solid_open_lew,&segWS,puma::Cutoff(1,1),false,1,false,0);
        puma::isosurface_LewinerMarchingCubes(&seg_solid_closed_lew,&segWS,puma::Cutoff(1,1),true,1,false,0);
        puma::isosurface_LewinerMarchingCubes(&seg_void_open_lew,&segWS,puma::Cutoff(0,0),false,1,false,0);
        puma::isosurface_LewinerMarchingCubes(&seg_void_closed_lew,&segWS,puma::Cutoff(0,0),true,1,false,0);
        //puma::export_STL(&seg_solid_open_lew, false, "cpp/test/out/stl/stltest/seg_solid_open_lew.stl");
        //puma::export_STL(&seg_solid_closed_lew, false, "cpp/test/out/stl/stltest/seg_solid_closed_lew.stl");
        //puma::export_STL(&seg_void_open_lew, false, "cpp/test/out/stl/stltest/seg_void_open_lew.stl");
        //puma::export_STL(&seg_void_closed_lew, false, "cpp/test/out/stl/stltest/seg_void_closed_lew.stl");

        if(!assertEquals(63360,seg_solid_open_lew.size(), &result)) {
            return result;
        }
        if(!assertEquals(83868,seg_solid_closed_lew.size(), &result)) {
            return result;
        }
        if(!assertEquals(63360,seg_void_open_lew.size(), &result)) {
            return result;
        }
        if(!assertEquals(164128,seg_void_closed_lew.size(), &result)) {
            return result;
        }


        std::vector< puma::Triangle<float> > gray_solid_open_interp, gray_solid_closed_interp, gray_void_open_interp, gray_void_closed_interp;

        puma::isosurface_MarchingCubes(&gray_solid_open_interp,&grayWS,puma::Cutoff(128,255),false,1,true,0);
        puma::isosurface_MarchingCubes(&gray_solid_closed_interp,&grayWS,puma::Cutoff(128,255),true,1,true,0);
        puma::isosurface_MarchingCubes(&gray_void_open_interp,&grayWS,puma::Cutoff(0,127),false,1,true,0);
        puma::isosurface_MarchingCubes(&gray_void_closed_interp,&grayWS,puma::Cutoff(0,127),true,1,true,0);
        //puma::export_STL(&gray_solid_open_interp, false, "cpp/test/out/stl/stltest/gray_solid_open_interp.stl");
        //puma::export_STL(&gray_solid_closed_interp, false, "cpp/test/out/stl/stltest/gray_solid_closed_interp.stl");
        //puma::export_STL(&gray_void_open_interp, false, "cpp/test/out/stl/stltest/gray_void_open_interp.stl");
        //puma::export_STL(&gray_void_closed_interp, false, "cpp/test/out/stl/stltest/gray_void_closed_interp.stl");

        if(!assertEquals(63200,gray_solid_open_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(83656,gray_solid_closed_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(63200,gray_void_open_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(166480,gray_void_closed_interp.size(), &result)) {
            return result;
        }


        std::vector< puma::Triangle<float> > seg_solid_open_interp, seg_solid_closed_interp, seg_void_open_interp, seg_void_closed_interp;

        puma::isosurface_MarchingCubes(&seg_solid_open_interp,&segWS,puma::Cutoff(1,1),false,1,true,0);
        puma::isosurface_MarchingCubes(&seg_solid_closed_interp,&segWS,puma::Cutoff(1,1),true,1,true,0);
        puma::isosurface_MarchingCubes(&seg_void_open_interp,&segWS,puma::Cutoff(0,0),false,1,true,0);
        puma::isosurface_MarchingCubes(&seg_void_closed_interp,&segWS,puma::Cutoff(0,0),true,1,true,0);
        //puma::export_STL(&seg_solid_open_interp, false, "cpp/test/out/stl/stltest/seg_solid_open_interp.stl");
        //puma::export_STL(&seg_solid_closed_interp, false, "cpp/test/out/stl/stltest/seg_solid_closed_interp.stl");
        //puma::export_STL(&seg_solid_closed_interp, false, "cpp/test/out/stl/stltest/seg_solid_closed_interp.stl");
        //puma::export_STL(&seg_void_open_interp, false, "cpp/test/out/stl/stltest/seg_void_open_interp.stl");

        if(!assertEquals(64800,seg_solid_open_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(85576,seg_solid_closed_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(63200,seg_void_open_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(166640,seg_void_closed_interp.size(), &result)) {
            return result;
        }


        std::vector< puma::Triangle<float> > gray_solid_open_lew_interp, gray_solid_closed_lew_interp, gray_void_open_lew_interp, gray_void_closed_lew_interp;

        puma::isosurface_LewinerMarchingCubes(&gray_solid_open_lew_interp,&grayWS,puma::Cutoff(128,255),false,1,true,0);
        puma::isosurface_LewinerMarchingCubes(&gray_solid_closed_lew_interp,&grayWS,puma::Cutoff(128,255),true,1,true,0);
        puma::isosurface_LewinerMarchingCubes(&gray_void_open_lew_interp,&grayWS,puma::Cutoff(0,127),false,1,true,0);
        puma::isosurface_LewinerMarchingCubes(&gray_void_closed_lew_interp,&grayWS,puma::Cutoff(0,127),true,1,true,0);
        //puma::export_STL(&gray_solid_open_lew_interp, false, "cpp/test/out/stl/stltest/gray_solid_open_lew_interp.stl");
        //puma::export_STL(&gray_solid_closed_lew_interp, false, "cpp/test/out/stl/stltest/gray_solid_closed_lew_interp.stl");
        //puma::export_STL(&gray_void_open_lew_interp, false, "cpp/test/out/stl/stltest/gray_void_open_lew_interp.stl");
        //puma::export_STL(&gray_void_closed_lew_interp, false, "cpp/test/out/stl/stltest/gray_void_closed_lew_interp.stl");

        if(!assertEquals(63200,gray_solid_open_lew_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(83656,gray_solid_closed_lew_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(63200,gray_void_open_lew_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(166480,gray_void_closed_lew_interp.size(), &result)) {
            return result;
        }


        std::vector< puma::Triangle<float> > seg_solid_open_lew_interp, seg_solid_closed_lew_interp, seg_void_open_lew_interp, seg_void_closed_lew_interp;

        puma::isosurface_LewinerMarchingCubes(&seg_solid_open_lew_interp,&segWS,puma::Cutoff(1,1),false,1,true,0);
        puma::isosurface_LewinerMarchingCubes(&seg_solid_closed_lew_interp,&segWS,puma::Cutoff(1,1),true,1,true,0);
        puma::isosurface_LewinerMarchingCubes(&seg_void_open_lew_interp,&segWS,puma::Cutoff(0,0),false,1,true,0);
        puma::isosurface_LewinerMarchingCubes(&seg_void_closed_lew_interp,&segWS,puma::Cutoff(0,0),true,1,true,0);
        //puma::export_STL(&seg_solid_open_lew_interp, false, "cpp/test/out/stl/stltest/seg_solid_open_lew_interp.stl");
        //puma::export_STL(&seg_solid_closed_lew_interp, false, "cpp/test/out/stl/stltest/seg_solid_closed_lew_interp.stl");
        //puma::export_STL(&seg_void_open_lew_interp, false, "cpp/test/out/stl/stltest/seg_void_open_lew_interp.stl");
        //puma::export_STL(&seg_void_closed_lew_interp, false, "cpp/test/out/stl/stltest/seg_void_closed_lew_interp.stl");

        if(!assertEquals(64800,seg_solid_open_lew_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(85576,seg_solid_closed_lew_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(63200,seg_void_open_lew_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(166640,seg_void_closed_lew_interp.size(), &result)) {
            return result;
        }



        puma::import_3DTiff(&grayWS,puma::path_to_example_file("100_cylinder_r40.tif"));

        grayWS.setMaterialID(puma::Cutoff(0,67),0);
        grayWS.setMaterialID(puma::Cutoff(68,127),1);
        grayWS.setMaterialID(puma::Cutoff(128,191),2);
        grayWS.setMaterialID(puma::Cutoff(192,255),3);

        std::vector< puma::Triangle<float> > seg_phase0_open_interp, seg_phase0_closed_interp;
        std::vector< puma::Triangle<float> > seg_phase1_open_interp, seg_phase1_closed_interp;
        std::vector< puma::Triangle<float> > seg_phase2_open_interp, seg_phase2_closed_interp;
        std::vector< puma::Triangle<float> > seg_phase3_open_interp, seg_phase3_closed_interp;

        puma::isosurface_MarchingCubes(&seg_phase0_closed_interp,&grayWS,puma::Cutoff(0,0),true,1,true,0);
        puma::isosurface_MarchingCubes(&seg_phase1_closed_interp,&grayWS,puma::Cutoff(1,1),true,1,true,0);
        puma::isosurface_MarchingCubes(&seg_phase2_closed_interp,&grayWS,puma::Cutoff(2,2),true,1,true,0);
        puma::isosurface_MarchingCubes(&seg_phase3_closed_interp,&grayWS,puma::Cutoff(3,3),true,1,true,0);
        //puma::export_STL(&seg_phase0_closed_interp, false, "cpp/test/out/stl/stltest/seg_phase0_closed_interp.stl");
        //puma::export_STL(&seg_phase1_closed_interp, false, "cpp/test/out/stl/stltest/seg_phase1_closed_interp.stl");
        //puma::export_STL(&seg_phase2_closed_interp, false, "cpp/test/out/stl/stltest/seg_phase2_closed_interp.stl");
        //puma::export_STL(&seg_phase3_closed_interp, false, "cpp/test/out/stl/stltest/seg_phase3_closed_interp.stl");

        if(!assertEquals(65952,seg_phase0_closed_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(163792,seg_phase1_closed_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(112320,seg_phase2_closed_interp.size(), &result)) {
            return result;
        }
        if(!assertEquals(38136,seg_phase3_closed_interp.size(), &result)) {
            return result;
        }
	return result;
    }
};
