#ifndef SUBTEST_H
#define SUBTEST_H

#include "testresult.h"

#include <vector>
#include <cmath>


class SubTest;
typedef TestResult (*test)();

class SubTest
{

public:

    SubTest();
    std::vector<TestResult> runAllTests();
    int numTests();
    void addTest(test t);
    std::string testSuiteName;

protected:

    std::vector<test> tests;

    static bool assertEquals(int valExpected, int valActual, TestResult *result);
    static bool assertEquals(long valExpected, long valActual, TestResult *result);
    static bool assertEquals(float valExpected, float valActual, TestResult *result);
    static bool assertEquals(float valExpected, float valActual, float tolerance, TestResult *result);
    static bool assertEquals(double valExpected, double valActual, TestResult *result);
    static bool assertEquals(double valExpected, double valActual, double tolerance, TestResult *result);
    static bool assertEquals(std::string valExpected, std::string valActual, TestResult *result);
    static bool assertEquals(char valExpected, char valActual, TestResult *result);
    static bool assertEquals(bool valExpected, bool valActual, TestResult *result);

};

#endif // SUBTEST_H
