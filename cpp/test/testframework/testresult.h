#ifndef TESTRESULT_H
#define TESTRESULT_H

#include "toString.h"


class TestResult {

public:

    int failed;
    std::string expected;
    std::string actual;

    std::string suiteName;
    int testNumber;
    std::string testTitle;
    std::string testDescription;

    TestResult() {
        failed = 0;
        this->testNumber = 0;
    }

    TestResult(std::string suiteName) {
        failed = 0;
        this->suiteName = toString::splice(suiteName);
        this->testNumber = 0;
    }

    TestResult(std::string suiteName, std::string testTitle) {
        failed = 0;
        this->suiteName = toString::splice(suiteName);
        this->testTitle = toString::splice(testTitle);
        this->testNumber = 0;
    }

    TestResult(std::string suiteName, std::string testTitle, int testNumber) {
        failed = 0;
        this->suiteName = toString::splice(suiteName);
        this->testTitle = toString::splice(testTitle);
        this->testNumber = testNumber;
    }

    TestResult(std::string suiteName,  std::string testTitle,  std::string testDescription) {
        failed = 0;
        this->suiteName = toString::splice(suiteName);
        this->testTitle = toString::splice(testTitle);
        this->testDescription = toString::splice(testDescription);
        this->testNumber = 0;
    }

    TestResult(std::string suiteName,  std::string testTitle, int testNumber, std::string testDescription) {
        failed = 0;
        this->suiteName = toString::splice(suiteName);
        this->testTitle = toString::splice(testTitle);
        this->testDescription = toString::splice(testDescription);
        this->testNumber = testNumber;
    }

    TestResult(int failed, std::string suiteName,  std::string testTitle, int testNumber, std::string testDescription) {
        this->failed = failed;
        this->suiteName = toString::splice(suiteName);
        this->testTitle = toString::splice(testTitle);
        this->testDescription = toString::splice(testDescription);
        this->testNumber = testNumber;
    }

};



#endif // TESTRESULT_H
