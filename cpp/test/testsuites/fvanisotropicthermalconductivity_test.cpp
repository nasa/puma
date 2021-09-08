#include "../testframework/subtest.h"
#include "puma.h"

#include <vector>
#include <map>


class FVanisotropicThermalConductivity_Test : public SubTest {
public:

    FVanisotropicThermalConductivity_Test() {
        testSuiteName = "FVanisotropicThermalConductivity_Test";

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
        tests.push_back(test57); // ff100 (to make sure it doesn't change)
        tests.push_back(test58);

        // new tests
        tests.push_back(test_wrongMatcond);
        tests.push_back(test_wrongMatcond2);
        tests.push_back(test_wrongOrientationSize);
        tests.push_back(test_wrongOrientationMagnitude);
        tests.push_back(test_matCondSingleEntry);
        tests.push_back(test_wrongPrescribedBC);
        tests.push_back(test_wrongPrescribedBC2);
        tests.push_back(test_PrescribedBC);

        // Orientation
        tests.push_back(test_rotatexy);
        tests.push_back(test_rotatexz);
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Testing CG

    static TestResult test1() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 1 - homogeneous isotropic k = [1,1,1,0,0,0], Dirichlet and symmetric bc, cg, x direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 1, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond,"mpfa","symmetric","cg",'x',1e-5,10000,true);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals(1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test2() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 2 - homogeneous isotropic k = [1,1,1,0,0,0], Dirichlet and symmetric bc, cg, y direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 2, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond,"mpfa","symmetric","cg",'y',1e-5,10000,false,30);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;

        if(!assertEquals(0.,ky.x, &result)) {
            return result;
        }
        if(!assertEquals(1.,ky.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,ky.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test3() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 3 - homogeneous isotropic k = [1,1,1,0,0,0], Dirichlet and symmetric bc, cg, z direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 3, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","cg",'z',1e-5,10000,false);

        std::cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << std::endl;

        if(!assertEquals(0.,kz.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,kz.y, &result)) {
            return result;
        }
        if(!assertEquals(1.,kz.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test4() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 4 - homogeneous anisotropic k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, cg, x direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 4, testDescription);

        int X=10,Y=10,Z=10;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","cg",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals(1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test5() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 5 - homogeneous anisotropic k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, cg, y direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 5, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","cg",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;

        if(!assertEquals(0.5,ky.x, &result)) {
            return result;
        }
        if(!assertEquals(1.,ky.y, &result)) {
            return result;
        }
        if(!assertEquals(0.5,ky.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test6() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 6 - homogeneous anisotropic k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, cg, z direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 6, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","cg",'z',1e-5,10000,false);

        std::cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << std::endl;


        if(!assertEquals(0.5,kz.x, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kz.y, &result)) {
            return result;
        }
        if(!assertEquals(1.,kz.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test7() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 7 - heterogeneous isotropic (materials in series) k = [1,1,1,0,0,0], Dirichlet and symmetric bc, cg, x direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 7, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0,0,0};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","cg",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals((double)1.818181818,kx.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,kx.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test8() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 8 - heterogeneous isotropic (materials in series) k = [1,1,1,0,0,0], Dirichlet and symmetric bc, cg, y direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 8, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0,0,0};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","cg",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;

        if(!assertEquals((double)0.,ky.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,ky.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,ky.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test9() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 9 - heterogeneous isotropic (materials in series) k = [1,1,1,0,0,0], Dirichlet and symmetric bc, cg, z direction, mpfa";
        std::string testDescription = "should return 9 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 9, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0,0,0};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","cg",'z',1e-5,10000,false);

        std::cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << std::endl;

        if(!assertEquals((double)0.,kz.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,kz.y, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,kz.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test10() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 10 - heterogeneous anisotropic (materials in series) k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, cg, x direction, mpfa";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 10, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0.5,0.5,0.5};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","cg",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals((double)1.818181818,kx.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,kx.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test11() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 11 - heterogeneous anisotropic (materials in series) k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, cg, y direction, mpfa";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 11, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0.5,0.5,0.5};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","cg",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;

        if(!assertEquals((double)0.5,ky.x, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,ky.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,ky.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test12() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 12 - heterogeneous anisotropic (materials in series) k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, cg, z direction, mpfa";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 12, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0.5,0.5,0.5};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","cg",'z',1e-5,10000,false);

        std::cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << std::endl;

        if(!assertEquals((double)0.5,kz.x, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,kz.y, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,kz.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Testing BICGSTAB with the same tests

    static TestResult test13() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 13 - homogeneous isotropic k = [1,1,1,0,0,0], Dirichlet and symmetric bc, bicgstab, x direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 13, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals(1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test14() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 14 - homogeneous isotropic k = [1,1,1,0,0,0], Dirichlet and symmetric bc, bicgstab, y direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 14, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;

        if(!assertEquals(0.,ky.x, &result)) {
            return result;
        }
        if(!assertEquals(1.,ky.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,ky.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test15() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 15 - homogeneous isotropic k = [1,1,1,0,0,0], Dirichlet and symmetric bc, bicgstab, z direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 15, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'z',1e-5,10000,false);

        std::cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << std::endl;

        if(!assertEquals(0.,kz.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,kz.y, &result)) {
            return result;
        }
        if(!assertEquals(1.,kz.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test16() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 16 - homogeneous anisotropic k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, bicgstab, x direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 16, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals(1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test17() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 17 - homogeneous anisotropic k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, bicgstab', y direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 17, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals(1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test18() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 18 - homogeneous anisotropic k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, bicgstab, z direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 18, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;


        if(!assertEquals(0.5,ky.x, &result)) {
            return result;
        }
        if(!assertEquals(1.,ky.y, &result)) {
            return result;
        }
        if(!assertEquals(0.5,ky.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test19() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 19 - heterogeneous isotropic (materials in series) k = [1,1,1,0,0,0], Dirichlet and symmetric bc, bicgstab, x direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 19, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0,0,0};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals((double)1.818181818,kx.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,kx.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test20() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 20 - heterogeneous isotropic (materials in series) k = [1,1,1,0,0,0], Dirichlet and symmetric bc, bicgstab, y direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 20, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0,0,0};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;

        if(!assertEquals((double)0.,ky.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,ky.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,ky.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test21() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 21 - heterogeneous isotropic (materials in series) k = [1,1,1,0,0,0], Dirichlet and symmetric bc, bicgstab, z direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 21, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0,0,0};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'z',1e-5,10000,false);

        std::cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << std::endl;

        if(!assertEquals((double)0.,kz.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,kz.y, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,kz.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test22() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 22 - heterogeneous anisotropic (materials in series) k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, bicgstab, x direction, mpfa";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 22, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0.5,0.5,0.5};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals((double)1.818181818,kx.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,kx.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test23() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 23 - heterogeneous anisotropic (materials in series) k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, bicgstab, y direction, mpfa";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 23, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0.5,0.5,0.5};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;

        if(!assertEquals((double)0.5,ky.x, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,ky.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,ky.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test24() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 24 - heterogeneous anisotropic (materials in series) k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, bicgstab, z direction, mpfa";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 24, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0.5,0.5,0.5};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'z',1e-5,10000,false);

        std::cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << std::endl;

        if(!assertEquals((double)0.5,kz.x, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,kz.y, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,kz.z, &result)) {
            return result;
        }

        return result;
    }



    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Testing PERIODIC BC

    static TestResult test25() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 25 - homogeneous isotropic k = [1,1,1,0,0,0], Dirichlet and periodic bc, cg, x direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 25, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals(1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test26() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 26 - homogeneous isotropic k = [1,1,1,0,0,0], Dirichlet and periodic bc, cg, y direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 26, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;

        if(!assertEquals(0.,ky.x, &result)) {
            return result;
        }
        if(!assertEquals(1.,ky.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,ky.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test27() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 27 - homogeneous isotropic k = [1,1,1,0,0,0], Dirichlet and periodic bc, cg, z direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 27, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'z',1e-5,10000,false);

        std::cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << std::endl;

        if(!assertEquals(0.,kz.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,kz.y, &result)) {
            return result;
        }
        if(!assertEquals(1.,kz.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test28() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 28 - homogeneous anisotropic k = [1,1,1,0.5,0.5,0.5], Dirichlet and periodic bc, cg, x direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 28, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals(1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test29() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 29 - homogeneous anisotropic k = [1,1,1,0.5,0.5,0.5], Dirichlet and periodic bc, cg', y direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 29, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals(1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test30() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 30 - homogeneous anisotropic k = [1,1,1,0.5,0.5,0.5], Dirichlet and periodic bc, cg, z direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 30, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;


        if(!assertEquals(0.5,ky.x, &result)) {
            return result;
        }
        if(!assertEquals(1.,ky.y, &result)) {
            return result;
        }
        if(!assertEquals(0.5,ky.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test31() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 31 - heterogeneous isotropic (materials in series) k = [1,1,1,0,0,0], Dirichlet and periodic bc, cg, x direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 31, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0,0,0};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals((double)1.818181818,kx.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,kx.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test32() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 32 - heterogeneous isotropic (materials in series) k = [1,1,1,0,0,0], Dirichlet and periodic bc, cg, y direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 32, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0,0,0};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;

        if(!assertEquals((double)0.,ky.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,ky.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,ky.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test33() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 33 - heterogeneous isotropic (materials in series) k = [1,1,1,0,0,0], Dirichlet and periodic bc, cg, z direction, mpfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 33, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0,0,0};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'z',1e-5,10000,false);

        std::cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << std::endl;

        if(!assertEquals((double)0.,kz.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,kz.y, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,kz.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test34() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 34 - heterogeneous anisotropic (materials in series) k = [1,1,1,0.5,0.5,0.5], Dirichlet and periodic bc, cg, x direction, mpfa";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 34, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0.5,0.5,0.5};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals((double)1.818181818,kx.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,kx.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test35() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 35 - heterogeneous anisotropic (materials in series) k = [1,1,1,0.5,0.5,0.5], Dirichlet and periodic bc, cg, y direction, mpfa";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 35, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0.5,0.5,0.5};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;

        if(!assertEquals((double)0.5,ky.x, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,ky.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,ky.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test36() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 36 - heterogeneous anisotropic (materials in series) k = [1,1,1,0.5,0.5,0.5], Dirichlet and periodic bc, cg, z direction, mpfa";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 36, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0.5,0.5,0.5};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'z',1e-5,10000,false);

        std::cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << std::endl;

        if(!assertEquals((double)0.5,kz.x, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,kz.y, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,kz.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Testing METHOD

    static TestResult test37() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 37 - homogeneous isotropic k = [1,1,1,0,0,0], Dirichlet and symmetric bc, cg, x direction, empfa";
        std::string testDescription = "should return 1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 37, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "periodic","cg",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals(1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test38() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 38 - homogeneous isotropic k = [1,1,1,0,0,0], Dirichlet and symmetric bc, cg, y direction, empfa";
        std::string testDescription = "should return 1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 38, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "periodic","cg",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;

        if(!assertEquals(0.,ky.x, &result)) {
            return result;
        }
        if(!assertEquals(1.,ky.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,ky.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test39() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 39 - homogeneous isotropic k = [1,1,1,0,0,0], Dirichlet and symmetric bc, cg, z direction, empfa";
        std::string testDescription = "should return 1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 39, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "periodic","cg",'z',1e-5,10000,false);

        std::cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << std::endl;

        if(!assertEquals(0.,kz.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,kz.y, &result)) {
            return result;
        }
        if(!assertEquals(1.,kz.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test40() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 40 - homogeneous anisotropic k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, cg, x direction, empfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 40, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "periodic","cg",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals(1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test41() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 41 - homogeneous anisotropic k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, cg', y direction, empfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 41, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "periodic","cg",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals(1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test42() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 42 - homogeneous anisotropic k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, cg, z direction, empfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 42, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "periodic","cg",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;


        if(!assertEquals(0.5,ky.x, &result)) {
            return result;
        }
        if(!assertEquals(1.,ky.y, &result)) {
            return result;
        }
        if(!assertEquals(0.5,ky.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test43() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 43 - heterogeneous isotropic (materials in series) k = [1,1,1,0,0,0], Dirichlet and symmetric bc, cg, x direction, empfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 43, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0,0,0};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "periodic","cg",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals((double)1.818181818,kx.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,kx.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test44() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 44 - heterogeneous isotropic (materials in series) k = [1,1,1,0,0,0], Dirichlet and symmetric bc, cg, y direction, empfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 44, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0,0,0};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "periodic","cg",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;

        if(!assertEquals((double)0.,ky.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,ky.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,ky.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test45() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 45 - heterogeneous isotropic (materials in series) k = [1,1,1,0,0,0], Dirichlet and symmetric bc, cg, z direction, empfa";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 45, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0,0,0};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "periodic","cg",'z',1e-5,10000,false);

        std::cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << std::endl;

        if(!assertEquals((double)0.,kz.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,kz.y, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,kz.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test46() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 46 - heterogeneous anisotropic (materials in series) k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, cg, x direction, empfa";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 46, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0.5,0.5,0.5};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "periodic","cg",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals((double)1.818181818,kx.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,kx.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,kx.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test47() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 47 - heterogeneous anisotropic (materials in series) k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, cg, y direction, empfa";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 47, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0.5,0.5,0.5};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "periodic","cg",'y',1e-5,10000,false);

        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;

        if(!assertEquals((double)0.5,ky.x, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,ky.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,ky.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test48() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 48 - heterogeneous anisotropic (materials in series) k = [1,1,1,0.5,0.5,0.5], Dirichlet and symmetric bc, cg, z direction, empfa";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 48, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz
        matCond[1] = {10,10,10,0.5,0.5,0.5};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "periodic","cg",'z',1e-5,10000,false);

        std::cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << std::endl;

        if(!assertEquals((double)0.5,kz.x, &result)) {
            return result;
        }
        if(!assertEquals((double)0.5,kz.y, &result)) {
            return result;
        }
        if(!assertEquals((double)5.5,kz.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Testing general inputs
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test49() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 49 - Negative Conductivity";
        std::string testDescription = "should return -1's";
        TestResult result(suiteName, testName, 49, testDescription);

        int X=100,Y=100,Z=100;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {-0.1,-0.1,-0.1,0.5,0.5,0.5};

        puma::Vec3<double> k = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "empfa", "periodic","cg",'z',1e-5,10000,false);

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


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test50() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 50 - Invalid Boundary Condition";
        std::string testDescription = "should return -1's";
        TestResult result(suiteName, testName, 50, testDescription);

        int X=100,Y=100,Z=100;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> k = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "per","cg",'z',1e-5,10000,false);

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


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test51() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 51 - Invalid Iterative Solver";
        std::string testDescription = "should return -1's";
        TestResult result(suiteName, testName, 51, testDescription);

        int X=100,Y=100,Z=100;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> k = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","c",'z',1e-5,10000,false);

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


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test52() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 52 - Invalid direction";
        std::string testDescription = "should return -1's";
        TestResult result(suiteName, testName, 52, testDescription);

        int X=100,Y=100,Z=100;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> k = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'w',1e-5,10000,false);

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


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test53() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 53 - Invalid method";
        std::string testDescription = "should return -1's";
        TestResult result(suiteName, testName, 53, testDescription);

        int X=100,Y=100,Z=100;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> k =  puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfaa", "periodic","cg",'x',1e-5,10000,false);

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


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test54() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 54 - Three Cells";
        std::string testDescription = "should return 0.1 in x direction and 0 in other directions";
        TestResult result(suiteName, testName, 54, testDescription);

        int X=3,Y=3,Z=3;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> k =  puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'x',1e-5,10000,false);

        if(!assertEquals(1.,k.x, &result)) {
            return result;
        }
        if(!assertEquals(0.5,k.y, &result)) {
            return result;
        }
        if(!assertEquals(0.5,k.z, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test55() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 55 - Multiple materials in series";
        std::string testDescription;
        TestResult result(suiteName, testName, 55, testDescription);

        puma::Workspace segWS(100,57,62,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;

        segWS.matrix.set(10,19,0,56,0,61,1);
        segWS.matrix.set(20,29,0,56,0,61,2);
        segWS.matrix.set(30,39,0,56,0,61,3);
        segWS.matrix.set(40,49,0,56,0,61,4);
        segWS.matrix.set(50,59,0,56,0,61,5);
        segWS.matrix.set(60,69,0,56,0,61,6);
        segWS.matrix.set(70,79,0,56,0,61,7);
        segWS.matrix.set(80,89,0,56,0,61,8);
        segWS.matrix.set(90,99,0,56,0,61,9);

        matCond[0] = {1,1,1,0,0,0};
        matCond[1] = {2,2,2,0,0,0};
        matCond[2] = {3,3,3,0,0,0};
        matCond[3] = {4,4,4,0,0,0};
        matCond[4] = {5,5,5,0,0,0};
        matCond[5] = {6,6,6,0,0,0};
        matCond[6] = {7,7,7,0,0,0};
        matCond[7] = {8,8,8,0,0,0};
        matCond[8] = {9,9,9,0,0,0};
        matCond[9] = {10,10,10,0,0,0};

        puma::Vec3<double> k = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'x',1e-5,10000,false);

        if(!assertEquals(3.44471,k.x, 1e-5, &result)) {
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


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test56() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 56 - 2x100x100, k = 0.1, periodic boundaries, cg, x direction, mpfa";
        std::string testDescription = "should return -1s";
        TestResult result(suiteName, testName, 56, testDescription);

        int X=2,Y=100,Z=100;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0,0,0}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "periodic","cg",'x',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;

        if(!assertEquals(-1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(-1.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(-1.,kx.z, &result)) {
            return result;
        }

        return result;
    }


    //    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test57() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 57 - Multiple materials in parallel";
        std::string testDescription;
        TestResult result(suiteName, testName, 57, testDescription);

        puma::Workspace ws(1e-6, false);
        puma::import_3DTiff(&ws,puma::path_to_example_file("100_fiberform.tif"));

        // Computing orientations using Structure Tensor (ST) method
        puma::MatVec3< double> tangents;
        puma::compute_orientationST(&ws, 0.7, 0.7, puma::Cutoff(89,255), &tangents, false);

        // Initializing Temperature, flux field and material conductivity
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[89] = {0.0257};
        matCond[255] = {5, 2.5};

        // Running simulation in three directions
        puma::Vec3<double> k;
        k = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond, &tangents, "mpfa", "symmetric","bicgstab",'x',1,10000,false);
        if(!assertEquals(0.253599,k.x, 1e-5, &result)) {
            return result;
        }
        if(!assertEquals(-0.0252018,k.y, 1e-5, &result)) {
            return result;
        }
        if(!assertEquals(-0.0117988,k.z, 1e-5, &result)) {
            return result;
        }

        k = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond, &tangents, "mpfa", "symmetric","bicgstab",'y',1,10000,false);
        if(!assertEquals(-0.0284796,k.x, 1e-5, &result)) {
            return result;
        }
        if(!assertEquals(0.435588,k.y, 1e-5, &result)) {
            return result;
        }
        if(!assertEquals(0.0333259,k.z, 1e-5, &result)) {
            return result;
        }

        k = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond, &tangents, "mpfa", "symmetric","bicgstab",'z',1,10000,false);
        if(!assertEquals(-0.0116534,k.x, 1e-5, &result)) {
            return result;
        }
        if(!assertEquals(0.0305338,k.y, 1e-5, &result)) {
            return result;
        }
        if(!assertEquals(0.216303,k.z, 1e-5, &result)) {
            return result;
        }
        return result;
    }


    //    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    static TestResult test58() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testName = "Test 58 - homogeneous anisotropic k = [1,1,1,0.5,0.5,0.5] with different domain sizes, running in all directions";
        std::string testDescription = "should return 1 in x direction and 0.5 in other directions";
        TestResult result(suiteName, testName, 58, testDescription);

        int X=20,Y=10,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5,0.5}; // kxx,kyy,kzz,kxy,kxz,kyz

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","cg",'x',1e-5,10000,false);
        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","cg",'y',1e-5,10000,false);
        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","cg",'z',1e-5,10000,false);

        std::cout << "kxx " << kx.x << " kxy " << kx.y << " kxz " << kx.z << std::endl;
        std::cout << "kyx " << ky.x << " kyy " << ky.y << " kyz " << ky.z << std::endl;
        std::cout << "kzx " << kz.x << " kzy " << kz.y << " kzz " << kz.z << std::endl;

        if(!assertEquals(1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kx.z, &result)) {
            return result;
        }

        if(!assertEquals(0.5,ky.x, &result)) {
            return result;
        }
        if(!assertEquals(1.,ky.y, &result)) {
            return result;
        }
        if(!assertEquals(0.5,ky.z, &result)) {
            return result;
        }

        if(!assertEquals(0.5,kz.x, &result)) {
            return result;
        }
        if(!assertEquals(0.5,kz.y, &result)) {
            return result;
        }
        if(!assertEquals(1.,kz.z, &result)) {
            return result;
        }

        return result;
    }


    static TestResult test_wrongMatcond() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testDescription = "only 5 entries in matCond";
        TestResult result(suiteName, __FUNCTION__, testDescription);

        int X=20,Y=10,Z=30;

        puma::Workspace segWS(X,Y,Z,230,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[80] = {10,10,10,0.5,0.5,0.5};
        matCond[255] = {1,1,1,0.5,0.5};

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'x',1e-5,10000,false);

        if(!assertEquals(-1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(-1.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(-1.,kx.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_wrongMatcond2() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testDescription = "only 5 entries in matCond";
        TestResult result(suiteName, __FUNCTION__, testDescription);

        int X=20,Y=10,Z=30;

        puma::Workspace segWS(X,Y,Z,230,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[80] = {10,10,10,0.5,0.5,0.5};
        matCond[255] = {1,-1,1,0.5,0.5,0.5};

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","bicgstab",'x',1e-5,10000,false);

        if(!assertEquals(-1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(-1.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(-1.,kx.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_wrongOrientationSize() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testDescription = "only 5 entries in matCond";
        TestResult result(suiteName, __FUNCTION__, testDescription);

        int X=20,Y=10,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,1,1,0.5,0.5};

        puma::MatVec3<double> orientation(X,Y, 50);

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, &orientation, "mpfa", "symmetric","bicgstab",'x',1e-5,10000,false);

        if(!assertEquals(-1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(-1.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(-1.,kx.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_wrongOrientationMagnitude() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testDescription = "only 5 entries in matCond";
        TestResult result(suiteName, __FUNCTION__, testDescription);

        int X=20,Y=10,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1,0.5};

        puma::MatVec3<double> orientation(X,Y,Z, puma::Vec3<double>(0, 0, 0));

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, &orientation, "mpfa", "symmetric","bicgstab",'x',1e-5,10000,false);

        if(!assertEquals(-1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(-1.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(-1.,kx.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_matCondSingleEntry() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        std::string testDescription = "only 5 entries in matCond";
        TestResult result(suiteName, __FUNCTION__, testDescription);

        int X=30,Y=30,Z=30;

        puma::Workspace segWS(X,Y,Z,0,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {1};
        matCond[1] = {10};

        segWS.matrix.set(0,(X-1)/2,0,Y-1,0,Z-1, 1);
        segWS.matrix.set(X/2,X-1,0,Y-1,0,Z-1, 0);

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric","cg",'x',1e-5,10000,false);

        if(!assertEquals((double)1.818181818,kx.x,0.001, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals((double)0.,kx.z, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_wrongPrescribedBC() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        TestResult result(suiteName, __FUNCTION__);

        int X=20,Y=10,Z=30;

        puma::Workspace segWS(X,Y,Z,230,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[80] = {10,10,10,0.5,0.5,0.5};
        matCond[255] = {1,1,1,0.5,0.5,0.5};

        puma::Matrix<double> prescribedBC(3, Y, Z, 0);
        prescribedBC.set(1,1, 0,-1, 0,-1, 10);

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric", &prescribedBC,
                                                                               "bicgstab",'x',1e-5,10000,false);

        if(!assertEquals(-1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(-1.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(-1.,kx.z, &result)) {
            return result;
        }
        return result;
    }

    static TestResult test_wrongPrescribedBC2() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        TestResult result(suiteName, __FUNCTION__);

        int X=20,Y=10,Z=30;

        puma::Workspace segWS(X,Y,Z,230,1e-6,false);
        puma::Matrix<double> T;
        puma::MatVec3<double> q;

        std::map<int, std::vector<double>> matCond;
        matCond[80] = {10,10,10,0.5,0.5,0.5};
        matCond[255] = {1,1,1,0.5,0.5,0.5};

        puma::Matrix<double> prescribedBC(2, Y-1, Z, 0);
        prescribedBC.set(1,1, 0,-1, 0,-1, 10);

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&segWS, &T, &q, matCond, "mpfa", "symmetric", &prescribedBC,
                                                                               "bicgstab",'x',1e-5,10000,false);

        if(!assertEquals(-1.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(-1.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(-1.,kx.z, &result)) {
            return result;
        }
        return result;
    }

    static TestResult test_PrescribedBC() {

        std::string suiteName = "FVanisotropicThermalConductivity_Test";
        TestResult result(suiteName, __FUNCTION__);

        int X=3,Y=15,Z=10;

        puma::Workspace ws(X, Y, Z, 1, 1.);
        puma::MatVec3< double> tangents(X, Y, Z, puma::Vec3<double>(1., 0., 0.));

        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {0};
        matCond[1] = {12, 1.2};

        puma::Matrix<double> prescribedBC_x(2, Y, Z, 0);
        prescribedBC_x.set(1,1, 0,-1, 0,-1, 10);

        puma::Matrix<double> prescribedBC_y(X, 2, Z, 0);
        prescribedBC_y.set(0,-1, 1,1, 0,-1, 10);

        puma::Matrix<double> prescribedBC_z(X, Y, 2, 0);
        prescribedBC_z.set(0,-1, 0,-1, 1,1, 10);

        puma::Vec3<double> kx = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents, "mpfa", "symmetric", &prescribedBC_x,
                                                                               "cg",'x',1e-5,10000,false);
        puma::Vec3<double> ky = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents, "mpfa", "symmetric", &prescribedBC_y,
                                                                               "cg",'y',1e-5,10000,false);
        puma::Vec3<double> kz = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents, "mpfa", "symmetric", &prescribedBC_z,
                                                                               "cg",'z',1e-5,10000,false);

        if(!assertEquals(120.,kx.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,kx.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,kx.z, &result)) {
            return result;
        }
        if(!assertEquals(0.,ky.x, &result)) {
            return result;
        }
        if(!assertEquals(12.,ky.y, &result)) {
            return result;
        }
        if(!assertEquals(0.,ky.z, &result)) {
            return result;
        }
        if(!assertEquals(0.,kz.x, &result)) {
            return result;
        }
        if(!assertEquals(0.,kz.y, &result)) {
            return result;
        }
        if(!assertEquals(12.,kz.z, &result)) {
            return result;
        }
        return result;
    }


    static TestResult test_rotatexy() {

        std::string suiteName = "FVanisotropicConductivity_Test";
        std::string testDescription = "rotate initial tensor with Vec3<double>(0., 1., 0.)";
        TestResult result(suiteName, __FUNCTION__, testDescription);

        int X=3,Y=15,Z=10;

        puma::Workspace ws(X, Y, Z, 1, 1.);
        puma::MatVec3< double> tangents(X, Y, Z, puma::Vec3<double>(1., 0., 0.));

        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {0};
        matCond[1] = {12, 1.2};

        puma::Vec3<double> kx1 = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents, "mpfa", "symmetric","cg",'x',1e-5,10000,false);
        puma::Vec3<double> ky1 = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents, "mpfa", "symmetric","cg",'y',1e-5,10000,false);
        puma::Vec3<double> kz1 = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents, "mpfa", "symmetric","cg",'z',1e-5,10000,false);

        puma::MatVec3< double> tangents2(X, Y, Z, puma::Vec3<double>(0., 1., 0.));
        puma::Vec3<double> kx2 = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents2, "mpfa", "symmetric","cg",'x',1e-5,10000,false);
        puma::Vec3<double> ky2 = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents2, "mpfa", "symmetric","cg",'y',1e-5,10000,false);
        puma::Vec3<double> kz2 = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents2, "mpfa", "symmetric","cg",'z',1e-5,10000,false);

        if(!assertEquals(kx1.x,ky2.y,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(kx1.y,ky2.x,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(kx1.z,ky2.z,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(ky1.x,kx2.y,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(ky1.y,kx2.x,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(ky1.z,kx2.z,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(kz1.x,kz2.y,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(kz1.y,kz2.x,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(kz1.z,kz2.z,1e-5, &result)) {
            return result;
        }
        return result;
    }

    static TestResult test_rotatexz() {

        std::string suiteName = "FVanisotropicConductivity_Test";
        std::string testDescription = "rotate initial tensor with Vec3<double>(0., 0., 1.)";
        TestResult result(suiteName, __FUNCTION__, testDescription);

        int X=3,Y=15,Z=10;

        puma::Workspace ws(X, Y, Z, 1, 1.);
        puma::MatVec3< double> tangents(X, Y, Z, puma::Vec3<double>(1., 0., 0.));

        puma::Matrix<double> T;
        puma::MatVec3<double> q;
        std::map<int, std::vector<double>> matCond;
        matCond[0] = {0};
        matCond[1] = {12, 1.2};

        puma::Vec3<double> kx1 = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents, "mpfa", "symmetric","cg",'x',1e-5,10000,false);
        puma::Vec3<double> ky1 = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents, "mpfa", "symmetric","cg",'y',1e-5,10000,false);
        puma::Vec3<double> kz1 = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents, "mpfa", "symmetric","cg",'z',1e-5,10000,false);

        puma::MatVec3< double> tangents2(X, Y, Z, puma::Vec3<double>(0., 0., 1.));
        puma::Vec3<double> kx2 = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents2, "mpfa", "symmetric","cg",'x',1e-5,10000,false);
        puma::Vec3<double> ky2 = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents2, "mpfa", "symmetric","cg",'y',1e-5,10000,false);
        puma::Vec3<double> kz2 = puma::compute_FVanisotropicThermalConductivity(&ws, &T, &q, matCond,&tangents2, "mpfa", "symmetric","cg",'z',1e-5,10000,false);

        if(!assertEquals(kx1.x,kz2.z,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(kx1.y,kz2.y,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(kx1.z,kz2.x,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(ky1.x,ky2.z,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(ky1.y,ky2.y,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(ky1.z,ky2.x,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(kz1.x,kx2.z,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(kz1.y,kx2.y,1e-5, &result)) {
            return result;
        }
        if(!assertEquals(kz1.z,kx2.x,1e-5, &result)) {
            return result;
        }
        return result;
    }

};

