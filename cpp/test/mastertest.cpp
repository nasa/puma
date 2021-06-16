#include "mastertest.h"


// INCLUDE NEW TEST SUITES HERE - DO NOT REMOVE THIS LINE

#include "testsuites/example_test.cpp"
#include "testsuites/volumefraction_test.cpp"
#include "testsuites/matrix_test.cpp"
#include "testsuites/vector3d_test.cpp"
#include "testsuites/vector4d_test.cpp"
#include "testsuites/triangle_test.cpp"
#include "testsuites/isosurface_test.cpp"
#include "testsuites/import3dtiff_test.cpp"
#include "testsuites/export3dtiff_test.cpp"
#include "testsuites/exportstl_test.cpp"
#include "testsuites/exporttextfile_test.cpp"
#include "testsuites/surfacearea_test.cpp"
#include "testsuites/medianfilter3d_test.cpp"
#include "testsuites/meanfilter3d_test.cpp"
#include "testsuites/bilateralfilter_test.cpp"
#include "testsuites/generaterandomfibers_test.cpp"
#include "testsuites/generaterandomspheres_test.cpp"
#include "testsuites/generatetpms_test.cpp"
#include "testsuites/porespace_test.cpp"
#include "testsuites/meaninterceptlength_test.cpp"
#include "testsuites/pstring_test.cpp"
#include "testsuites/fvthermalconductivity_test.cpp"
#include "testsuites/fvanisotropicthermalconductivity_test.cpp"
#include "testsuites/fvelectricalconductivity_test.cpp"
#include "testsuites/ejthermalconductivity_test.cpp"
#include "testsuites/ejelectricalconductivity_test.cpp"
#include "testsuites/exportbin_test.cpp"
#include "testsuites/importbin_test.cpp"
#include "testsuites/exportvtk_test.cpp"
#include "testsuites/particlescuberilletortuosity_test.cpp"
#include "testsuites/orientation_test.cpp"

MasterTest::MasterTest() {

    //ADD NEW TEST SUITES HERE - DO NOT REMOVE THIS LINE
    testSuites.push_back(Example_Test());
    testSuites.push_back(Matrix_Test());
    testSuites.push_back(Vector3D_Test());
    testSuites.push_back(Vector4D_Test());
    testSuites.push_back(VolumeFraction_Test());
    testSuites.push_back(Triangle_Test());
    testSuites.push_back(IsoSurface_Test());
    testSuites.push_back(Import3DTiff_Test());
    testSuites.push_back(Export3DTiff_Test());
    testSuites.push_back(ExportSTL_Test());
    testSuites.push_back(ExportTextfile_Test());
    testSuites.push_back(SurfaceArea_Test());
    testSuites.push_back(MedianFilter3D_Test());
    testSuites.push_back(MeanFilter3D_Test());
    testSuites.push_back(BilateralFilter_Test());
    testSuites.push_back(Generate_RandomFibers_Test());
    testSuites.push_back(Generate_RandomSpheres_Test());
    testSuites.push_back(Generate_TPMS_Test());
    testSuites.push_back(PoreSpace_Test());
    testSuites.push_back(MeanInterceptLength_Test());
    testSuites.push_back(PString_Test());
    testSuites.push_back(FVThermalConductivity_Test());
    testSuites.push_back(FVanisotropicThermalConductivity_Test());
    testSuites.push_back(FVElectricalConductivity_Test());
    testSuites.push_back(EJThermalConductivity_Test());
    testSuites.push_back(EJElectricalConductivity_Test());
    testSuites.push_back(ImportBin_Test());
    testSuites.push_back(ExportBin_Test());
    testSuites.push_back(ExportVTK_Test());
    testSuites.push_back(ParticlesCuberilleTortuosity_Test());
    testSuites.push_back(Orientation_Test());
}

std::vector<TestResult> MasterTest::runAllTests() {

    std::vector<TestResult> failedTests;

    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);

    for(unsigned int i = 0; i < testSuites.size(); i++ ) {
        std::cout << std::endl << "[** Test Suite " << green << testSuites[i].testSuiteName << def << ' ' << i+1 << " / " << testSuites.size() << " **]"  << std::endl;
        std::vector<TestResult> testSuiteFailedTests = testSuites[i].runAllTests();
        failedTests.insert(failedTests.end(), testSuiteFailedTests.begin(), testSuiteFailedTests.end()) ;
    }

    return failedTests;
}

std::vector<TestResult> MasterTest::runTests(std::vector<int> indices) {

    std::vector<TestResult> failedTests;

    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);

    for(unsigned int i = 0; i < indices.size(); i++ ) {
        std::cout << std::endl << "[** Test Suite " << green << testSuites[indices[i]].testSuiteName << def << ' ' << i+1 << " / " << indices.size() << " **]"  << std::endl;
        std::vector<TestResult> testSuiteFailedTests = testSuites[indices[i]].runAllTests();
        failedTests.insert(failedTests.end(), testSuiteFailedTests.begin(), testSuiteFailedTests.end()) ;
    }

    return failedTests;
}

void MasterTest::printTestSuites() {
    std::cout << "To run all tests, run without arguments or with '-a'" << std::endl;
    std::cout << "To run subset of test suites, list suite indexes as \n    individual arguments or grouped (eg. '0-3')" << std::endl;
    std::cout << "Available Test Suites: " << std::endl;

    for(unsigned int i=0; i<testSuites.size(); i++) {
        std::cout << i << ": " << testSuites[i].testSuiteName << std::endl;
    }
}

int MasterTest::numTests() {
    int total = 0;
    for(auto & testSuite : testSuites) {
        total += testSuite.numTests();
    }

    return total;
}

int MasterTest::numTests(const std::vector<int>& indices) {
    int total = 0;
    for(int indice : indices) {
        total += testSuites[indice].numTests();
    }

    return total;
}

bool MasterTest::validSubTests(const std::vector<int>& indices) {
    for(int indice : indices) {
        if(indice<0||indice>=(int)testSuites.size()) {
            return false;
        }
    }

    return true;
}
