#include "../testframework/subtest.h"
#include "puma.h"

#include <vector>
#include <map>


class Orientation_Test : public SubTest {
public:

    Orientation_Test() {
        testSuiteName = "Orientation_Test";

        //        tests.push_back(test0); // for quick tests

        tests.push_back(test_compute_orientationComparison_dims);
        tests.push_back(test_compute_orientationComparison_application1);
        tests.push_back(test_compute_orientationComparison_application2);
        tests.push_back(test_compute_orientationComparison_application3);
        tests.push_back(test_compute_orientationComparison_application4);
        tests.push_back(test_compute_orientationComparison_application5);
        tests.push_back(test_compute_orientationComparison_application6);
        tests.push_back(test_compute_orientationComparison_application7);
        tests.push_back(test_compute_orientationComparison_application8);
        tests.push_back(test_compute_orientationComparison_application9);
        tests.push_back(test_compute_orientationComparison_application10);
        tests.push_back(test_compute_orientationComparison_application11);
        tests.push_back(test_compute_orientationComparison_application12);
        tests.push_back(test_compute_orientationComparison_application13);
        tests.push_back(test_compute_orientationComparison_application14);
        tests.push_back(test_compute_orientationComparison_application15);
        tests.push_back(test_compute_orientationComparison_application16);
        tests.push_back(test_compute_orientationComparison_application17);
        tests.push_back(test_compute_orientationComparison_application18);
        tests.push_back(test_compute_orientationComparison_application19);

        tests.push_back(test_RC_emptyWS);
        tests.push_back(test_RC_degreeSep0);
        tests.push_back(test_RC_degreeSepNotdividendof360);
        tests.push_back(test_RC_refinedNotExactDivider);
        tests.push_back(test_RC_wrongCutoffs);
        tests.push_back(test_RC_application1);
        tests.push_back(test_RC_application2);
        tests.push_back(test_RC_application3);
        tests.push_back(test_RC_application4);
        tests.push_back(test_RC_application5);
        tests.push_back(test_RC_application6);
        tests.push_back(test_RC_applicationGeneratedRandomFibers);

        tests.push_back(test_AF_emptyWS);
        tests.push_back(test_AF_application1);
        tests.push_back(test_AF_applicationGeneratedRandomFibers);

        tests.push_back(test_ST_emptyWS);
        tests.push_back(test_ST_wrongCutoffs);
        tests.push_back(test_ST_wrongKernelSizes);
        tests.push_back(test_ST_application1);
        tests.push_back(test_ST_applicationGeneratedRandomFibers);
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//    static TestResult test0() {
//
//        std::string suiteName = "Orientation_Test";
//        std::string testName = "Test 0: For quick tests";
//        std::string testDescription;
//        TestResult result(suiteName, testName, 0, testDescription);
//
//        return result;
//    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Testing Orientation Comparison function

    static TestResult test_compute_orientationComparison_dims() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison dimension check";
        TestResult result(suiteName, testName, 1);

        puma::Workspace segWS(10,10,10, 0, 1e-5,false);
        segWS.matrix.set(4,4,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(10,10,10);
        puma::MatVec3<double> dirs2(122,10,10);
        dirs.set(4,4,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(1,0,0));
        dirs.set(4,4,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(1,0,0));

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(-1., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(-1., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(-1., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(-1., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application1() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison no error";
        TestResult result(suiteName, testName, 2);

        puma::Workspace segWS(10,10,10, 0, 1e-5,false);
        segWS.matrix.set(4,4,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(10,10,10);
        puma::MatVec3<double> dirs2(10,10,10);
        dirs.set(4,4,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(1.,0.,0.));
        dirs2.set(4,4,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(1.,0.,0.));

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));

        if(!assertEquals(0., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application2() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error xy plane, first unit vector pointing in x";
        TestResult result(suiteName, testName, 3);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(1,0,0));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(1,1,0).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(45., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(45., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application3() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error xz plane, first unit vector pointing in x";
        TestResult result(suiteName, testName, 4);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(1,0,0));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(1,0,1).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(45., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(45., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application4() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error yz plane, first unit vector pointing in x";
        TestResult result(suiteName, testName, 5);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(1,0,0));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(0,1,1).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(90., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(90., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application5() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error xy plane, first unit vector pointing in y";
        TestResult result(suiteName, testName, 6);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(0,1,0));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(1,1,0).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(45., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(45., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application6() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error xz plane, first unit vector pointing in y";
        TestResult result(suiteName, testName, 7);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(0,1,0));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(1,0,1).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(90., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(90., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application7() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error yz plane, first unit vector pointing in y";
        TestResult result(suiteName, testName, 8);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(0,1,0));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(0,1,1).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(45., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(45., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application8() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error xy plane, first unit vector pointing in z";
        TestResult result(suiteName, testName, 9);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(0,0,1));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(1,1,0).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(90., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(90., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application9() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error xz plane, first unit vector pointing in z";
        TestResult result(suiteName, testName, 10);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(0,0,1));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(1,0,1).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(45., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(45., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application10() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error yz plane, first unit vector pointing in z";
        TestResult result(suiteName, testName, 11);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(0,0,1));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(0,1,1).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(45., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(45., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application11() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error xy plane, first unit vector pointing in -x";
        TestResult result(suiteName, testName, 12);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(-1,0,0));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(1,1,0).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(45., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(45., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application12() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error xz plane, first unit vector pointing in -x";
        TestResult result(suiteName, testName, 13);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(-1,0,0));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(1,0,1).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(45., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(45., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application13() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error yz plane, first unit vector pointing in -x";
        TestResult result(suiteName, testName, 14);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(-1,0,0));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(0,1,1).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(90., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(90., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application14() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error xy plane, first unit vector pointing in -y";
        TestResult result(suiteName, testName, 15);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(0,-1,0));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(1,1,0).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(45., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(45., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application15() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error xz plane, first unit vector pointing in -y";
        TestResult result(suiteName, testName, 16);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(0,-1,0));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(1,0,1).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(90., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(90., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application16() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error yz plane, first unit vector pointing in -y";
        TestResult result(suiteName, testName, 17);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(0,-1,0));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(0,1,1).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(45., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(45., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application17() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error xy plane, first unit vector pointing in -z";
        TestResult result(suiteName, testName, 18);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(0,0,-1));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(1,1,0).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(90., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(90., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application18() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error xz plane, first unit vector pointing in -z";
        TestResult result(suiteName, testName, 19);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(0,0,-1));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(1,0,1).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(45., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(45., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_compute_orientationComparison_application19() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - Orientation Comparison 45 degree error yz plane, first unit vector pointing in -z";
        TestResult result(suiteName, testName, 20);

        puma::Workspace segWS(20,20,20, 0, 1e-5,false);
        segWS.matrix.set(10,10,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs(20,20,20);
        puma::MatVec3<double> dirs2(20,20,20);
        dirs.set(10,10,0,dirs.Y()-1,0,dirs.Z()-1, puma::Vec3<double>(0,0,-1));
        dirs2.set(10,10,0,dirs2.Y()-1,0,dirs2.Z()-1, puma::Vec3<double>(0,1,1).norm());

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &dirs2, &error, puma::Cutoff(1,1));
        std::pair <double, double> MeanSD2 = puma::compute_orientationComparison(&segWS, &dirs2, &dirs, &error, puma::Cutoff(1,1));

        if(!assertEquals(45., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }
        if(!assertEquals(45., MeanSD2.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD2.second, &result)) {
            return result;
        }

        return result;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Testing Orientation Ray Casting (RC)

    static TestResult test_RC_emptyWS() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - RC feeding an empty workspace";
        TestResult result(suiteName, testName, 21);

        puma::Workspace segWS(1e-6,false);
        puma::MatVec3<double> dirs;

        bool check = puma::compute_orientationRC(&segWS, puma::Cutoff(1, 1),  5, &dirs);


        if(!assertEquals(false, check, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_RC_degreeSep0() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - RC degree separation 0";
        TestResult result(suiteName, testName, 22);

        puma::Workspace segWS(10,10,10, 0, 1e-5,false);
        segWS.matrix.set(4,4,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs;

        bool check = puma::compute_orientationRC(&segWS, puma::Cutoff(1, 1),  0, &dirs);

        if(!assertEquals(false, check, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_RC_degreeSepNotdividendof360() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - RC degree separation not divider of 360";
        TestResult result(suiteName, testName, 23);

        puma::Workspace segWS(10,10,10, 0, 1e-5,false);
        segWS.matrix.set(4,4,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs;

        bool check1 = puma::compute_orientationRC(&segWS, puma::Cutoff(1, 1),  17, &dirs);

        if(!assertEquals(false, check1, &result)) {
            return result;
        }

        bool check2 = puma::compute_orientationRC(&segWS, puma::Cutoff(1, 1), 17, 1, &dirs);

        if(!assertEquals(false, check2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_RC_refinedNotExactDivider() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - RC refined angle not divider of initial";
        TestResult result(suiteName, testName, 24);

        puma::Workspace segWS(10,10,10, 0, 1e-5,false);
        segWS.matrix.set(4,4,0,segWS.Y()-1,0,segWS.Z()-1, 1);
        puma::MatVec3<double> dirs;

        bool check = puma::compute_orientationRC(&segWS, puma::Cutoff(1, 1), 15, 2, &dirs);

        if(!assertEquals(false, check, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_RC_wrongCutoffs() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - RC wrong cutoffs";
        TestResult result(suiteName, testName, 25);

        puma::Workspace grayWS(10,10,10, 0, 1e-5,false);
        grayWS.matrix.set(4,4,0,grayWS.Y()-1,0,grayWS.Z()-1, 128);
        puma::MatVec3<double> dirs;

        bool check1 = puma::compute_orientationRC(&grayWS, puma::Cutoff(128, 127), 12, 2, &dirs);

        if(!assertEquals(false, check1, &result)) {
            return result;
        }

        bool check2 = puma::compute_orientationRC(&grayWS, puma::Cutoff(5, 500000), 12, 2, &dirs);

        if(!assertEquals(false, check2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_RC_application1() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - RC single straight fiber pointing in x";
        TestResult result(suiteName, testName, 26);

        int scale = 50;

        puma::Workspace segWS(scale,scale,scale, 0, 1e-5,false);
        puma::MatVec3<double> tangents(scale,scale,scale);
        segWS.matrix.set(0,scale-1, scale/2,scale/2, scale/2,scale/2, 1);
        tangents.set(0,scale-1, scale/2,scale/2, scale/2,scale/2, puma::Vec3<double>(1,0,0));

        puma::MatVec3<double> dirs;
        puma::compute_orientationRC(&segWS, puma::Cutoff(1, 1), 10, &dirs, false);

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

        if(!assertEquals(0., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_RC_application2() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - RC single straight fiber pointing in y";
        TestResult result(suiteName, testName, 27);

        int scale = 50;

        puma::Workspace segWS(scale,scale,scale, 0, 1e-5,false);
        puma::MatVec3<double> tangents(scale,scale,scale);
        segWS.matrix.set(scale/2,scale/2, 0,scale-1, scale/2,scale/2, 1);
        tangents.set(scale/2,scale/2, 0,scale-1, scale/2,scale/2, puma::Vec3<double>(0,1,0));

        puma::MatVec3<double> dirs;
        puma::compute_orientationRC(&segWS, puma::Cutoff(1, 1), 10, &dirs, false);

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

        if(!assertEquals(0., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_RC_application3() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - RC single straight fiber pointing in z";
        TestResult result(suiteName, testName, 28);

        int scale = 50;

        puma::Workspace segWS(scale,scale,scale, 0, 1e-5,false);
        puma::MatVec3<double> tangents(scale,scale,scale);
        segWS.matrix.set(scale/2,scale/2, scale/2,scale/2, 0,scale-1, 1);
        tangents.set(scale/2,scale/2, scale/2,scale/2, 0,scale-1, puma::Vec3<double>(0,0,1));

        puma::MatVec3<double> dirs;
        puma::compute_orientationRC(&segWS, puma::Cutoff(1, 1), 10, &dirs, false);

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

        if(!assertEquals(0., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_RC_application4() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - RC single straight fiber pointing in x with Refinement";
        TestResult result(suiteName, testName, 29);

        int scale = 50;

        puma::Workspace segWS(scale,scale,scale, 0, 1e-5,false);
        puma::MatVec3<double> tangents(scale,scale,scale);
        segWS.matrix.set(0,scale-1, scale/2,scale/2, scale/2,scale/2, 1);
        tangents.set(0,scale-1, scale/2,scale/2, scale/2,scale/2, puma::Vec3<double>(1,0,0));

        puma::MatVec3<double> dirs;
        puma::compute_orientationRC(&segWS, puma::Cutoff(1, 1), 10, 5, &dirs, false);

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

        if(!assertEquals(0., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_RC_application5() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - RC single straight fiber pointing in y with Refinement";
        TestResult result(suiteName, testName, 30);

        int scale = 50;

        puma::Workspace segWS(scale,scale,scale, 0, 1e-5,false);
        puma::MatVec3<double> tangents(scale,scale,scale);
        segWS.matrix.set(scale/2,scale/2, 0,scale-1, scale/2,scale/2, 1);
        tangents.set(scale/2,scale/2, 0,scale-1, scale/2,scale/2, puma::Vec3<double>(0,1,0));

        puma::MatVec3<double> dirs;
        puma::compute_orientationRC(&segWS, puma::Cutoff(1, 1), 10, 5, &dirs, false);

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

        if(!assertEquals(0., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_RC_application6() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - RC single straight fiber pointing in z with Refinement";
        TestResult result(suiteName, testName, 31);

        int scale = 50;

        puma::Workspace segWS(scale,scale,scale, 0, 1e-5,false);
        puma::MatVec3<double> tangents(scale,scale,scale);
        segWS.matrix.set(scale/2,scale/2, scale/2,scale/2, 0,scale-1, 1);
        tangents.set(scale/2,scale/2, scale/2,scale/2, 0,scale-1, puma::Vec3<double>(0,0,1));

        puma::MatVec3<double> dirs;
        puma::compute_orientationRC(&segWS, puma::Cutoff(1, 1), 10, 5, &dirs, false);

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

        if(!assertEquals(0., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_RC_applicationGeneratedRandomFibers() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - RC application to artificially generated straight fibers";
        TestResult result(suiteName, testName, 32);

        int scale = 50;

        puma::Workspace grayWS(1e-5,false);
        puma::MatVec3<double> tangents;
        RandomFibersInput input;

        input.straightCircle(scale,scale,scale,2,0,scale,0,0, -1, -1,false,0.9,1);
        input.print = false; puma::generateRandomFibers(&grayWS,&tangents,input);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS, puma::Cutoff(0,127),0);
        segWS.setMaterialID(&grayWS, puma::Cutoff(128,255),1);

        puma::MatVec3<double> dirs;
        puma::compute_orientationRC(&segWS, puma::Cutoff(1, 1), 15,5, &dirs, false);

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

        if(!assertEquals(7.93948, MeanSD.first, 1e-2, &result)) {
            return result;
        }
        if(!assertEquals(9.73364, MeanSD.second, 1e-1, &result)) {
            return result;
        }

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Testing Orientation Artificial Flux (AF)

    static TestResult test_AF_emptyWS() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - AF feeding an empty workspace";
        TestResult result(suiteName, testName, 34);

        puma::Workspace segWS(1e-6,false);
        puma::MatVec3<double> dirs;

        bool check = puma::compute_orientationAF(&segWS, &dirs, puma::Cutoff(1, 1), 1e-5, 10000);

        if(!assertEquals(false, check, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_AF_application1() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - AF single straight fiber pointing in x";
        TestResult result(suiteName, testName, 35);

        int scale = 50;

        puma::Workspace segWS(scale,scale,scale, 0, 1e-5,false);
        puma::MatVec3<double> tangents(scale,scale,scale);
        segWS.matrix.set(0,scale-1, scale/2,scale/2, scale/2,scale/2, 1);
        tangents.set(0,scale-1, scale/2,scale/2, scale/2,scale/2, puma::Vec3<double>(1,0,0));

        puma::MatVec3<double> dirs;
        puma::compute_orientationAF(&segWS, &dirs, puma::Cutoff(1, 1), 1e-5, 10000);

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

        if(!assertEquals(0., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_AF_applicationGeneratedRandomFibers() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - AF application to artificially generated straight fibers";
        TestResult result(suiteName, testName, 36);

        int scale = 50;

        puma::Workspace grayWS(1e-6,false);
        puma::MatVec3<double> tangents;
        RandomFibersInput input;

        input.straightCircle(scale,scale,scale,2,0,scale,0,0, -1, -1,false,0.9,1);
        input.print = false; puma::generateRandomFibers(&grayWS,&tangents,input);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS,puma::Cutoff(0,127),0);
        segWS.setMaterialID(&grayWS,puma::Cutoff(128,255),1);

        puma::MatVec3<double> dirs;
        puma::compute_orientationAF(&segWS, &dirs, puma::Cutoff(1, 1), 1e-5, 10000,false);

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

        if(!assertEquals(24.3758, MeanSD.first, 1., &result)) {  // AF is intrinsically bad for straight fiber
            return result;
        }
        if(!assertEquals(21.1581 , MeanSD.second, 2., &result)) {
            return result;
        }

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Testing Orientation Structure Tensor (ST)

    static TestResult test_ST_emptyWS() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - ST feeding an empty workspace";
        TestResult result(suiteName, testName, 38);

        puma::Workspace segWS(1e-6,false);
        puma::MatVec3<double> dirs;

        bool check = puma::compute_orientationST(&segWS, 0.7, 1.1, puma::Cutoff(1, 1), &dirs,false);

        if(!assertEquals(false, check, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_ST_wrongCutoffs() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - ST feeding an empty workspace";
        TestResult result(suiteName, testName, 39);

        int scale = 50;

        puma::Workspace segWS(scale,scale,scale, 0, 1e-5,false);
        puma::MatVec3<double> tangents(scale,scale,scale);
        segWS.matrix.set(0,scale-1, scale/2,scale/2, scale/2,scale/2, 1);
        tangents.set(0,scale-1, scale/2,scale/2, scale/2,scale/2, puma::Vec3<double>(1,0,0));

        puma::MatVec3<double> dirs;
        bool check = puma::compute_orientationST(&segWS, 0.7, 1.1, puma::Cutoff(2, 1), &dirs,false);

        if(!assertEquals(false, check, &result)) {
            return result;
        }

        check = puma::compute_orientationST(&segWS, 0.7, 1.1, puma::Cutoff(-1, 1), &dirs,false);

        if(!assertEquals(false, check, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_ST_wrongKernelSizes() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - ST feeding an empty workspace";
        TestResult result(suiteName, testName, 40);

        int scale = 50;

        puma::Workspace segWS(scale,scale,scale, 0, 1e-5,false);
        puma::MatVec3<double> tangents(scale,scale,scale);
        segWS.matrix.set(0,scale-1, scale/2,scale/2, scale/2,scale/2, 1);
        tangents.set(0,scale-1, scale/2,scale/2, scale/2,scale/2, puma::Vec3<double>(1,0,0));

        puma::MatVec3<double> dirs;
        bool check = puma::compute_orientationST(&segWS, -0.1, 1.1, puma::Cutoff(1, 1), &dirs,false);

        if(!assertEquals(false, check, &result)) {
            return result;
        }

        check = puma::compute_orientationST(&segWS, 0.7, -0.1, puma::Cutoff(1, 1), &dirs,false);

        if(!assertEquals(false, check, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_ST_application1() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - ST single straight fiber pointing in x";
        TestResult result(suiteName, testName, 41);

        int scale = 50;

        puma::Workspace segWS(scale,scale,scale, 0, 1e-5,false);
        puma::MatVec3<double> tangents(scale,scale,scale);
        segWS.matrix.set(0,scale-1, scale/2,scale/2, scale/2,scale/2, 1);
        tangents.set(0,scale-1, scale/2,scale/2, scale/2,scale/2, puma::Vec3<double>(1,0,0));

        puma::MatVec3<double> dirs;
        puma::compute_orientationST(&segWS, 0.7, 1.1, puma::Cutoff(1, 1), &dirs,false);

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

        if(!assertEquals(0., MeanSD.first, &result)) {
            return result;
        }
        if(!assertEquals(0., MeanSD.second, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test_ST_applicationGeneratedRandomFibers() {

        std::string suiteName = "Orientation_Test";
        std::string testName = "Test - ST application to artificially generated straight fibers";
        TestResult result(suiteName, testName, 42);

        int scale = 50;

        puma::Workspace grayWS(1e-5,false);
        puma::MatVec3<double> tangents;
        RandomFibersInput input;

        input.straightCircle(scale,scale,scale,2,0,scale,0,0, -1, -1,false,0.9,1);
        input.print = false; puma::generateRandomFibers(&grayWS,&tangents,input);

        puma::Workspace segWS(grayWS.shape(), false);
        segWS.setMaterialID(&grayWS, puma::Cutoff(0,127),0);
        segWS.setMaterialID(&grayWS, puma::Cutoff(128,255),1);

        puma::MatVec3<double> dirs;
        puma::compute_orientationST(&segWS, 0.7, 1.1, puma::Cutoff(1, 1), &dirs,false);

        puma::Matrix<double> error;
        std::pair <double, double> MeanSD = puma::compute_orientationComparison(&segWS, &dirs, &tangents, &error, puma::Cutoff(1,1));

        if(!assertEquals(5.80349, MeanSD.first, 1e-5, &result)) {
            return result;
        }
        if(!assertEquals(9.34072, MeanSD.second, 1e-4, &result)) {
            return result;
        }

        return result;
    }
};
