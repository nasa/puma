#ifndef MASTERTEST_H
#define MASTERTEST_H

#include "testframework/subtest.h"
#include "testframework/testresult.h"
#include "testframework/color.h"

#include <set>
#include <iostream>


class MasterTest
{
public:
    MasterTest();
    void printTestSuites();
    std::vector<TestResult> runAllTests();
    std::vector<TestResult> runTests(std::vector<int> indices);
    int numTests();
    int numTests(const std::vector<int>& indices);
    bool validSubTests(const std::vector<int>& indices);

private:
    std::vector<SubTest> testSuites;
};

#endif // MASTERTEST_H
