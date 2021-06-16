#include "../testframework/subtest.h"

#include <vector>


class Example_Test : public SubTest {
public:

    Example_Test() {
        testSuiteName = "Example_Test";

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

    }

    static TestResult test1() {

        std::string suiteName = "Example_Test";
        std::string testName = "Example_Test: Test 1 - (couple word description)";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::vector<double> m;

        if(!assertEquals((long)0,(long)m.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {
        std::string suiteName = "Example_Test";
        std::string testName = "Example_Test: Test 2 - (couple word description)";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 2, testDescription);

        std::vector<double> m;

        if(!assertEquals((long)0,(long)m.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {
        std::string suiteName = "Example_Test";
        std::string testName = "Example_Test: Test 3 - (couple word description)";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 3, testDescription);

        std::vector<double> m;

        if(!assertEquals((long)0,(long)m.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test4() {
        std::string suiteName = "Example_Test";
        std::string testName = "Example_Test: Test 4 - (couple word description)";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 4, testDescription);

        std::vector<double> m;

        if(!assertEquals((long)0,(long)m.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {
        std::string suiteName = "Example_Test";
        std::string testName = "Example_Test: Test 5 - (couple word description)";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 5, testDescription);

        std::vector<double> m;

        if(!assertEquals((long)0,(long)m.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test6() {
        std::string suiteName = "Example_Test";
        std::string testName = "Example_Test: Test 6 - (couple word description)";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 6, testDescription);

        std::vector<double> m;

        if(!assertEquals((long)0,(long)m.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test7() {
        std::string suiteName = "Example_Test";
        std::string testName = "Example_Test: Test 7 - (couple word description)";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 7, testDescription);

        std::vector<double> m;

        if(!assertEquals((long)0,(long)m.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test8() {
        std::string suiteName = "Example_Test";
        std::string testName = "Example_Test: Test 8 - (couple word description)";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 8, testDescription);

        std::vector<double> m;

        if(!assertEquals((long)0,(long)m.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test9() {
        std::string suiteName = "Example_Test";
        std::string testName = "Example_Test: Test 9 - (couple word description)";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 9, testDescription);

        std::vector<double> m;

        if(!assertEquals((long)0,(long)m.size(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test10() {
        std::string suiteName = "Example_Test";
        std::string testName = "Example_Test: Test 10 - (couple word description)";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 10, testDescription);

        std::vector<double> m;

        if(!assertEquals((long)0,(long)m.size(), &result)) {
            return result;
        }

        return result;
    }

};
