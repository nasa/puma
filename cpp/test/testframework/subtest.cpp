#include "subtest.h"
#include "toString.h"
#include "color.h"


SubTest::SubTest() {
    testSuiteName = "";
}

int SubTest::numTests() {
    return tests.size();
}

void SubTest::addTest(test t) {
    tests.push_back(t);
}

std::vector<TestResult> SubTest::runAllTests() {

    Color::Modifier red(Color::FG_RED);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);

    std::vector<TestResult> failedTests;

    for(unsigned int i=0;i<tests.size();i++) {

        TestResult result = (tests[i])();

        if(result.failed == 0) {
            std::cout << "[-- " << testSuiteName << " -- Test " << i+1 << " / " << tests.size() <<  ": " << green << "Success" << def << " --]"  << std::endl;
        } else {
            failedTests.push_back(result);
            std::cout << red << "[-- " << testSuiteName << " -- Test " << i+1 << " / " << tests.size() <<  ": FAILED  --]" << std::endl;
            std::cout << "   [-- Expected: " << result.expected << " -- Actual: " << result.actual << " --]" << def << std::endl;
        }
    }

    return failedTests;
}


bool SubTest::assertEquals(int valExpected, int valActual, TestResult *result) {

    if(valExpected==valActual){
        result->failed=0;
    }else{
        result->failed=1;
    }
    result->expected=toString::convert(valExpected);
    result->actual=toString::convert(valActual);

    return !result->failed;
}

bool SubTest::assertEquals(long valExpected, long valActual, TestResult *result) {

    if(valExpected==valActual){
        result->failed=0;
    }else{
        result->failed=1;
    }
    result->expected=toString::convert(valExpected);
    result->actual=toString::convert(valActual);

    return !result->failed;
}

bool SubTest::assertEquals(float valExpected, float valActual, TestResult *result) {

    if(valActual==0||valExpected==0) {
        if(fabs(valExpected-valActual)<1e-6) {
            result->failed=0;
        }
        else {
            result->failed=1;
        }
    }
    else {

        if(valExpected < 1e-6 || valActual < 1e-6) {
            if(fabs(1-valExpected/valActual)<1e-4) {
                result->failed=0;
            }
            else {
                result->failed=1;
            }
        }
        else {
            if(fabs(1-valExpected/valActual)<1e-6) {
                result->failed=0;
            }
            else {
                result->failed=1;
            }
        }
    }
    result->expected=toString::convert(valExpected);
    result->actual=toString::convert(valActual);

    return !result->failed;
}

bool SubTest::assertEquals(double valExpected, double valActual, TestResult *result) {



    if(valActual==0||valExpected==0) {
        if(fabs(valExpected-valActual)<1e-6) {
            result->failed=0;
        }
        else {
            result->failed=1;
        }
    }
    else {

        if(valExpected < 1e-6 || valActual < 1e-6) {
            if(fabs(1-valExpected/valActual)<1e-4) {
                result->failed=0;
            }
            else {
                result->failed=1;
            }
        }
        else {
            if(fabs(1-valExpected/valActual)<1e-6) {
                result->failed=0;
            }
            else {
                result->failed=1;
            }
        }
    }
    result->expected=toString::convert(valExpected);
    result->actual=toString::convert(valActual);

    return !result->failed;
}

bool SubTest::assertEquals(float valExpected, float valActual, float tolerance, TestResult *result) {

    if(fabs(valExpected-valActual)<tolerance) {
        result->failed=0;
    }
    else {
        result->failed=1;
    }

    result->expected=toString::convert(valExpected);
    result->actual=toString::convert(valActual);

    return !result->failed;
}

bool SubTest::assertEquals(double valExpected, double valActual, double tolerance, TestResult *result) {

    if(fabs(valExpected-valActual)<tolerance) {
        result->failed=0;
    }
    else {
        result->failed=1;
    }

    result->expected=toString::convert(valExpected);
    result->actual=toString::convert(valActual);

    return !result->failed;
}


bool SubTest::assertEquals(std::string valExpected, std::string valActual, TestResult *result) {

    if(valExpected==valActual){
        result->failed=0;
    }else{
        result->failed=1;
    }
    result->expected=valExpected;
    result->actual=valActual;

    return !result->failed;
}

bool SubTest::assertEquals(char valExpected, char valActual, TestResult *result) {

    if(valExpected==valActual){
        result->failed=0;
    }else{
        result->failed=1;
    }
    result->expected=toString::convert(valExpected);
    result->actual=toString::convert(valActual);

    return !result->failed;
}

bool SubTest::assertEquals(bool valExpected, bool valActual, TestResult *result) {

    if(valExpected==valActual){
        result->failed=0;
    }else{
        result->failed=1;
    }
    result->expected=toString::convert(valExpected);
    result->actual=toString::convert(valActual);

    return !result->failed;
}
