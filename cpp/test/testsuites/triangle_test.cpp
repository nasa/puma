#include "../testframework/subtest.h"
#include "puma.h"


class Triangle_Test : public SubTest {
public:

    Triangle_Test() {
        testSuiteName = "Triangle_Test";

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


    }

    static TestResult test1() {
        std::string suiteName = "Triangle Test";
        std::string testName = "Triangle_Test: Test 1 - Default Constructor";
        std::string testDescription = "Default constructor test with float";
        TestResult result(suiteName, testName, 1, testDescription);

        puma::Triangle<float> tri;


        if(!assertEquals((float)0,tri.normZ, &result)) {
            return result;
        }
        if(!myAssertEquals(puma::Vec3<float>(0,0,0),tri.p0, &result)) {
            return result;
        }
        if(!myAssertEquals(puma::Vec3<float>(0,0,0),tri.p1, &result)) {
            return result;
        }
        if(!myAssertEquals(puma::Vec3<float>(0,0,0),tri.p1, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {
        std::string suiteName = "Triangle Test";
        std::string testName = "Triangle_Test: Test 1 - Default Constructor";
        std::string testDescription = "Default constructor test with double";
        TestResult result(suiteName, testName, 2, testDescription);

        puma::Triangle<double> tri;


        if(!assertEquals((double)0,tri.normZ, &result)) {
            return result;
        }
        if(!myAssertEquals(puma::Vec3<double>(0,0,0),tri.p0, &result)) {
            return result;
        }
        if(!myAssertEquals(puma::Vec3<double>(0,0,0),tri.p1, &result)) {
            return result;
        }
        if(!myAssertEquals(puma::Vec3<double>(0,0,0),tri.p1, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 3 - Constructor 2 Test";
        std::string testDescription = "Constructor 2 Test - float/zeros";
        TestResult result(suiteName, testName, 3, testDescription);

        puma::Triangle<float> tri1;
        puma::Triangle<float> tri2(puma::Vec3<float>(0,0,0),puma::Vec3<float>(0,0,0),puma::Vec3<float>(0,0,0),0);

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test4() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 4 - Constructor 2 Test";
        std::string testDescription = "Constructor 2 Test - float/non-zeros";
        TestResult result(suiteName, testName, 4, testDescription);

        puma::Triangle<float> tri1;
        tri1.p0.x = 1.1;
        tri1.p0.y = 1.2;
        tri1.p0.z = 1.3;
        tri1.p1.x = 1.4;
        tri1.p1.y = 1.5;
        tri1.p1.z = 1.6;
        tri1.p2.x = 1.7;
        tri1.p2.y = 1.8;
        tri1.p2.z = 1.9;
        tri1.normZ = 2.0;
        puma::Triangle<float> tri2(puma::Vec3<float>(1.1,1.2,1.3),puma::Vec3<float>(1.4,1.5,1.6),puma::Vec3<float>(1.7,1.8,1.9),2.0);

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 5 - Constructor 2 Test";
        std::string testDescription = "Constructor 2 Test - double/zeros";
        TestResult result(suiteName, testName, 5, testDescription);

        puma::Triangle<double> tri1;
        puma::Triangle<double> tri2(puma::Vec3<double>(0,0,0),puma::Vec3<double>(0,0,0),puma::Vec3<double>(0,0,0),0);

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test6() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 6 - Constructor 2 Test";
        std::string testDescription = "Constructor 2 Test - double/non-zeros";
        TestResult result(suiteName, testName, 6, testDescription);

        puma::Triangle<double> tri1;
        tri1.p0.x = 1.1;
        tri1.p0.y = 1.2;
        tri1.p0.z = 1.3;
        tri1.p1.x = 1.4;
        tri1.p1.y = 1.5;
        tri1.p1.z = 1.6;
        tri1.p2.x = 1.7;
        tri1.p2.y = 1.8;
        tri1.p2.z = 1.9;
        tri1.normZ = 2.0;
        puma::Triangle<double> tri2(puma::Vec3<double>(1.1,1.2,1.3),puma::Vec3<double>(1.4,1.5,1.6),puma::Vec3<double>(1.7,1.8,1.9),2.0);

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test7() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 7 - Copy constructor test";
        std::string testDescription = "Copy constructor Test - double";
        TestResult result(suiteName, testName, 7, testDescription);

        puma::Triangle<double> tri1;
        tri1.p0.x = 1.1;
        tri1.p0.y = 1.2;
        tri1.p0.z = 1.3;
        tri1.p1.x = 1.4;
        tri1.p1.y = 1.5;
        tri1.p1.z = 1.6;
        tri1.p2.x = 1.7;
        tri1.p2.y = 1.8;
        tri1.p2.z = 1.9;
        tri1.normZ = 2.0;
        puma::Triangle<double> tri2(tri1);

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test8() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 8 - Copy constructor test";
        std::string testDescription = "Copy constructor Test - float";
        TestResult result(suiteName, testName, 8, testDescription);

        puma::Triangle<float> tri1;
        tri1.p0.x = 1.1;
        tri1.p0.y = 1.2;
        tri1.p0.z = 1.3;
        tri1.p1.x = 1.4;
        tri1.p1.y = 1.5;
        tri1.p1.z = 1.6;
        tri1.p2.x = 1.7;
        tri1.p2.y = 1.8;
        tri1.p2.z = 1.9;
        tri1.normZ = 2.0;
        puma::Triangle<float> tri2(tri1);

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test9() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 9 = operator";
        std::string testDescription = "= operator test - double";
        TestResult result(suiteName, testName, 9, testDescription);

        puma::Triangle<double> tri1;
        tri1.p0.x = 1.1;
        tri1.p0.y = 1.2;
        tri1.p0.z = 1.3;
        tri1.p1.x = 1.4;
        tri1.p1.y = 1.5;
        tri1.p1.z = 1.6;
        tri1.p2.x = 1.7;
        tri1.p2.y = 1.8;
        tri1.p2.z = 1.9;
        tri1.normZ = 2.0;
        puma::Triangle<double> tri2;

        if(myAssertEquals(tri1,tri2, &result)) {
            result.failed=1;
            return result;
        }

        tri2=tri1;

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test10() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 10 = operator";
        std::string testDescription = "= operator test - float";
        TestResult result(suiteName, testName, 10, testDescription);

        puma::Triangle<float> tri1;
        tri1.p0.x = 1.1;
        tri1.p0.y = 1.2;
        tri1.p0.z = 1.3;
        tri1.p1.x = 1.4;
        tri1.p1.y = 1.5;
        tri1.p1.z = 1.6;
        tri1.p2.x = 1.7;
        tri1.p2.y = 1.8;
        tri1.p2.z = 1.9;
        tri1.normZ = 2.0;
        puma::Triangle<float> tri2;

        if(myAssertEquals(tri1,tri2, &result)) {
            result.failed=1;
            return result;
        }

        tri2=tri1;

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test11() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 11 - operator";
        std::string testDescription = "- operator test - double";
        TestResult result(suiteName, testName, 11, testDescription);

        puma::Triangle<double> tri1;
        tri1.p0.x = 1.1;
        tri1.p0.y = 2.2;
        tri1.p0.z = 0.3;
        tri1.p1.x = 0.4;
        tri1.p1.y = 1.5;
        tri1.p1.z = -0.4;
        tri1.p2.x = 2.7;
        tri1.p2.y = 3.8;
        tri1.p2.z = 1.9;
        tri1.normZ = 2.0;
        puma::Triangle<double> tri2;
        tri2.p0.x = 2.1;
        tri2.p0.y = 2.2;
        tri2.p0.z = 2.3;
        tri2.p1.x = 1.4;
        tri2.p1.y = 1.5;
        tri2.p1.z = 1.6;
        tri2.p2.x = 3.7;
        tri2.p2.y = 3.8;
        tri2.p2.z = 3.9;
        tri2.normZ = 2.0;

        puma::Vec3<double> subtract(1,0,2);

        tri2=(tri2-subtract);

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test12() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 12 - operator";
        std::string testDescription = "- operator test - float";
        TestResult result(suiteName, testName, 12, testDescription);

        puma::Triangle<float> tri1;
        tri1.p0.x = 1.1;
        tri1.p0.y = 2.2;
        tri1.p0.z = 0.3;
        tri1.p1.x = 0.4;
        tri1.p1.y = 1.5;
        tri1.p1.z = -0.4;
        tri1.p2.x = 2.7;
        tri1.p2.y = 3.8;
        tri1.p2.z = 1.9;
        tri1.normZ = 2.0;
        puma::Triangle<float> tri2;
        tri2.p0.x = 2.1;
        tri2.p0.y = 2.2;
        tri2.p0.z = 2.3;
        tri2.p1.x = 1.4;
        tri2.p1.y = 1.5;
        tri2.p1.z = 1.6;
        tri2.p2.x = 3.7;
        tri2.p2.y = 3.8;
        tri2.p2.z = 3.9;
        tri2.normZ = 2.0;

        puma::Vec3<float> subtract(1,0,2);

        tri2=tri2-subtract;

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test13() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 13 + operator";
        std::string testDescription = "- operator test + double";
        TestResult result(suiteName, testName, 13, testDescription);

        puma::Triangle<double> tri1;
        tri1.p0.x = 1.1;
        tri1.p0.y = 2.2;
        tri1.p0.z = 0.3;
        tri1.p1.x = 0.4;
        tri1.p1.y = 1.5;
        tri1.p1.z = -0.4;
        tri1.p2.x = 2.7;
        tri1.p2.y = 3.8;
        tri1.p2.z = 1.9;
        tri1.normZ = 2.0;
        puma::Triangle<double> tri2;
        tri2.p0.x = 2.1;
        tri2.p0.y = 2.2;
        tri2.p0.z = 2.3;
        tri2.p1.x = 1.4;
        tri2.p1.y = 1.5;
        tri2.p1.z = 1.6;
        tri2.p2.x = 3.7;
        tri2.p2.y = 3.8;
        tri2.p2.z = 3.9;
        tri2.normZ = 2.0;

        tri1=tri1+puma::Vec3<double>(1,0,2);

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test14() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 14 + operator";
        std::string testDescription = "- operator test + float";
        TestResult result(suiteName, testName, 14, testDescription);

        puma::Triangle<float> tri1;
        tri1.p0.x = 1.1;
        tri1.p0.y = 2.2;
        tri1.p0.z = 0.3;
        tri1.p1.x = 0.4;
        tri1.p1.y = 1.5;
        tri1.p1.z = -0.4;
        tri1.p2.x = 2.7;
        tri1.p2.y = 3.8;
        tri1.p2.z = 1.9;
        tri1.normZ = 2.0;
        puma::Triangle<float> tri2;
        tri2.p0.x = 2.1;
        tri2.p0.y = 2.2;
        tri2.p0.z = 2.3;
        tri2.p1.x = 1.4;
        tri2.p1.y = 1.5;
        tri2.p1.z = 1.6;
        tri2.p2.x = 3.7;
        tri2.p2.y = 3.8;
        tri2.p2.z = 3.9;
        tri2.normZ = 2.0;

        tri1=tri1+puma::Vec3<float>(1,0,2);


        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test15() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 15 - operator <T>";
        std::string testDescription = "- operator test - <T> double";
        TestResult result(suiteName, testName, 15, testDescription);

        puma::Triangle<double> tri1;
        tri1.p0.x = 1.1;
        tri1.p0.y = 1.2;
        tri1.p0.z = 1.3;
        tri1.p1.x = 1.4;
        tri1.p1.y = 1.5;
        tri1.p1.z = 1.6;
        tri1.p2.x = 1.7;
        tri1.p2.y = 1.8;
        tri1.p2.z = 1.9;
        tri1.normZ = 2.0;

        puma::Triangle<double> tri2;
        tri2.p0.x=2.2;
        tri2.p0.y=2.3;
        tri2.p0.z=2.4;
        tri2.p1.x=2.5;
        tri2.p1.y=2.6;
        tri2.p1.z=2.7;
        tri2.p2.x=2.8;
        tri2.p2.y=2.9;
        tri2.p2.z=3.0;
        tri2.normZ=2.0;

        tri2 = tri2 - 1.1;

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test16() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 16 - operator <T>";
        std::string testDescription = "- operator test - <T> float";
        TestResult result(suiteName, testName, 16, testDescription);

        puma::Triangle<float> tri1;
        tri1.p0.x = 1.1;
        tri1.p0.y = 1.2;
        tri1.p0.z = 1.3;
        tri1.p1.x = 1.4;
        tri1.p1.y = 1.5;
        tri1.p1.z = 1.6;
        tri1.p2.x = 1.7;
        tri1.p2.y = 1.8;
        tri1.p2.z = 1.9;
        tri1.normZ = 2.0;

        puma::Triangle<float> tri2;
        tri2.p0.x=2.2;
        tri2.p0.y=2.3;
        tri2.p0.z=2.4;
        tri2.p1.x=2.5;
        tri2.p1.y=2.6;
        tri2.p1.z=2.7;
        tri2.p2.x=2.8;
        tri2.p2.y=2.9;
        tri2.p2.z=3.0;
        tri2.normZ=2.0;

        tri2 = tri2 - 1.1;

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test17() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 17 + operator <T>";
        std::string testDescription = "- operator test + <T> double";
        TestResult result(suiteName, testName, 17, testDescription);

        puma::Triangle<double> tri1;
        tri1.p0.x = 1.1;
        tri1.p0.y = 1.2;
        tri1.p0.z = 1.3;
        tri1.p1.x = 1.4;
        tri1.p1.y = 1.5;
        tri1.p1.z = 1.6;
        tri1.p2.x = 1.7;
        tri1.p2.y = 1.8;
        tri1.p2.z = 1.9;
        tri1.normZ = 2.0;

        puma::Triangle<double> tri2;
        tri2.p0.x=2.2;
        tri2.p0.y=2.3;
        tri2.p0.z=2.4;
        tri2.p1.x=2.5;
        tri2.p1.y=2.6;
        tri2.p1.z=2.7;
        tri2.p2.x=2.8;
        tri2.p2.y=2.9;
        tri2.p2.z=3.0;
        tri2.normZ=2.0;

        tri1 = tri1 + 1.1;

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test18() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 18 + operator <T>";
        std::string testDescription = "- operator test + <T> float";
        TestResult result(suiteName, testName, 18, testDescription);

        puma::Triangle<float> tri1;
        tri1.p0.x = 1.1;
        tri1.p0.y = 1.2;
        tri1.p0.z = 1.3;
        tri1.p1.x = 1.4;
        tri1.p1.y = 1.5;
        tri1.p1.z = 1.6;
        tri1.p2.x = 1.7;
        tri1.p2.y = 1.8;
        tri1.p2.z = 1.9;
        tri1.normZ = 2.0;

        puma::Triangle<float> tri2;
        tri2.p0.x=2.2;
        tri2.p0.y=2.3;
        tri2.p0.z=2.4;
        tri2.p1.x=2.5;
        tri2.p1.y=2.6;
        tri2.p1.z=2.7;
        tri2.p2.x=2.8;
        tri2.p2.y=2.9;
        tri2.p2.z=3.0;
        tri2.normZ=2.0;

        tri1 = tri1 + 1.1;

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test19() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 19 * operator <T>";
        std::string testDescription = "- operator test * <T> double";
        TestResult result(suiteName, testName, 19, testDescription);

        puma::Triangle<double> tri1;
        tri1.p0.x = 2.2;
        tri1.p0.y = 2.4;
        tri1.p0.z = 2.6;
        tri1.p1.x = 2.8;
        tri1.p1.y = 3.0;
        tri1.p1.z = 3.2;
        tri1.p2.x = 3.4;
        tri1.p2.y = 3.6;
        tri1.p2.z = 3.8;
        tri1.normZ = 2.0;

        puma::Triangle<double> tri2;
        tri2.p0.x=1.1;
        tri2.p0.y=1.2;
        tri2.p0.z=1.3;
        tri2.p1.x=1.4;
        tri2.p1.y=1.5;
        tri2.p1.z=1.6;
        tri2.p2.x=1.7;
        tri2.p2.y=1.8;
        tri2.p2.z=1.9;
        tri2.normZ=2.0;

        tri2 = tri2 * 2.0;

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test20() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 20 * operator <T>";
        std::string testDescription = "- operator test * <T> float";
        TestResult result(suiteName, testName, 20, testDescription);

        puma::Triangle<float> tri1;
        tri1.p0.x = 2.2;
        tri1.p0.y = 2.4;
        tri1.p0.z = 2.6;
        tri1.p1.x = 2.8;
        tri1.p1.y = 3.0;
        tri1.p1.z = 3.2;
        tri1.p2.x = 3.4;
        tri1.p2.y = 3.6;
        tri1.p2.z = 3.8;
        tri1.normZ = 2.0;

        puma::Triangle<float> tri2;
        tri2.p0.x=1.1;
        tri2.p0.y=1.2;
        tri2.p0.z=1.3;
        tri2.p1.x=1.4;
        tri2.p1.y=1.5;
        tri2.p1.z=1.6;
        tri2.p2.x=1.7;
        tri2.p2.y=1.8;
        tri2.p2.z=1.9;
        tri2.normZ=2.0;

        tri2 = tri2 * 2.0;

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test21() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 21 / operator <T>";
        std::string testDescription = "- operator test / <T> double";
        TestResult result(suiteName, testName, 21, testDescription);

        puma::Triangle<double> tri1;
        tri1.p0.x = 2.2;
        tri1.p0.y = 2.4;
        tri1.p0.z = 2.6;
        tri1.p1.x = 2.8;
        tri1.p1.y = 3.0;
        tri1.p1.z = 3.2;
        tri1.p2.x = 3.4;
        tri1.p2.y = 3.6;
        tri1.p2.z = 3.8;
        tri1.normZ = 2.0;

        puma::Triangle<double> tri2;
        tri2.p0.x=1.1;
        tri2.p0.y=1.2;
        tri2.p0.z=1.3;
        tri2.p1.x=1.4;
        tri2.p1.y=1.5;
        tri2.p1.z=1.6;
        tri2.p2.x=1.7;
        tri2.p2.y=1.8;
        tri2.p2.z=1.9;
        tri2.normZ=2.0;

        tri1 = tri1 / 2.0;

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test22() {
        std::string suiteName = "Triangle_Test";
        std::string testName = "Triangle_Test: Test 22 / operator <T>";
        std::string testDescription = "- operator test / <T> float";
        TestResult result(suiteName, testName, 22, testDescription);

        puma::Triangle<float> tri1;
        tri1.p0.x = 2.2;
        tri1.p0.y = 2.4;
        tri1.p0.z = 2.6;
        tri1.p1.x = 2.8;
        tri1.p1.y = 3.0;
        tri1.p1.z = 3.2;
        tri1.p2.x = 3.4;
        tri1.p2.y = 3.6;
        tri1.p2.z = 3.8;
        tri1.normZ = 2.0;

        puma::Triangle<float> tri2;
        tri2.p0.x=1.1;
        tri2.p0.y=1.2;
        tri2.p0.z=1.3;
        tri2.p1.x=1.4;
        tri2.p1.y=1.5;
        tri2.p1.z=1.6;
        tri2.p2.x=1.7;
        tri2.p2.y=1.8;
        tri2.p2.z=1.9;
        tri2.normZ=2.0;

        tri1 = tri1 / 2.0;

        if(!myAssertEquals(tri1,tri2, &result)) {
            return result;
        }

        return result;
    }





    static bool myAssertEquals(puma::Triangle<float> valExpected, puma::Triangle<float> valActual, TestResult *result) {

        if( myAssertEquals(valExpected.p0, valActual.p0, result)
                && myAssertEquals(valExpected.p1, valActual.p1, result)
                && myAssertEquals(valExpected.p2, valActual.p2, result)
                && assertEquals(valExpected.normZ, valActual.normZ, result)) {
            result->failed = 0;
        } else {
            result->failed = 1;
        }

        result->expected="<";
        result->expected.append(toString::convert(valExpected.p0.x));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.p0.y));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.p0.z));
        result->expected.append(">,");
        result->expected.append("<");
        result->expected.append(toString::convert(valExpected.p1.x));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.p1.y));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.p1.z));
        result->expected.append(">,");
        result->expected.append(toString::convert(valExpected.p2.x));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.p2.y));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.p2.z));
        result->expected.append(">");

        result->actual="<";
        result->actual.append(toString::convert(valActual.p0.x));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.p0.y));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.p0.z));
        result->actual.append(">,");
        result->actual.append("<");
        result->actual.append(toString::convert(valActual.p1.x));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.p1.y));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.p1.z));
        result->actual.append(">,");
        result->actual.append(toString::convert(valActual.p2.x));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.p2.y));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.p2.z));
        result->actual.append(">");

        return !result->failed;
    }

    static bool myAssertEquals(puma::Triangle<double> valExpected, puma::Triangle<double> valActual, TestResult *result) {

        if( myAssertEquals(valExpected.p0, valActual.p0, result)
                && myAssertEquals(valExpected.p1, valActual.p1, result)
                && myAssertEquals(valExpected.p2, valActual.p2, result)
                && assertEquals(valExpected.normZ, valActual.normZ, result)) {
            result->failed = 0;
        } else {
            result->failed = 1;
        }

        result->expected="<";
        result->expected.append(toString::convert(valExpected.p0.x));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.p0.y));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.p0.z));
        result->expected.append(">,");
        result->expected.append("<");
        result->expected.append(toString::convert(valExpected.p1.x));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.p1.y));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.p1.z));
        result->expected.append(">,");
        result->expected.append(toString::convert(valExpected.p2.x));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.p2.y));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.p2.z));
        result->expected.append(">");

        result->actual="<";
        result->actual.append(toString::convert(valActual.p0.x));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.p0.y));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.p0.z));
        result->actual.append(">,");
        result->actual.append("<");
        result->actual.append(toString::convert(valActual.p1.x));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.p1.y));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.p1.z));
        result->actual.append(">,");
        result->actual.append(toString::convert(valActual.p2.x));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.p2.y));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.p2.z));
        result->actual.append(">");

        return !result->failed;
    }

    static bool myAssertEquals(puma::Vec3<float> valExpected, puma::Vec3<float> valActual, TestResult *result) {

        if(valActual.x==0||valExpected.x==0||valActual.y==0||valExpected.y==0||valActual.z==0||valExpected.z==0) {
            if(fabs(valExpected.x-valActual.x)<1e-6 && fabs(valExpected.y-valActual.y)<1e-6 && fabs(valExpected.z-valActual.z)<1e-6) {
                result->failed=0;
            }
            else {
                result->failed=1;
            }
        }
        else {
            if(1-fabs(valExpected.x/valActual.x)<1e-6 && 1-fabs(valExpected.y/valActual.y)<1e-6 && 1-fabs(valExpected.z/valActual.z)<1e-6) {
                result->failed=0;
            }
            else {
                result->failed=1;
            }
        }
        result->expected="<";
        result->expected.append(toString::convert(valExpected.x));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.y));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.z));
        result->expected.append(">");
        result->actual="<";
        result->actual.append(toString::convert(valActual.x));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.y));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.z));
        result->actual.append(">");

        return !result->failed;
    }

    static bool myAssertEquals(puma::Vec3<double> valExpected, puma::Vec3<double> valActual, TestResult *result) {

        if(valActual.x==0||valExpected.x==0||valActual.y==0||valExpected.y==0||valActual.z==0||valExpected.z==0) {
            if(fabs(valExpected.x-valActual.x)<1e-8 && fabs(valExpected.y-valActual.y)<1e-8 && fabs(valExpected.z-valActual.z)<1e-8) {
                result->failed=0;
            }
            else {
                result->failed=1;
            }
        }
        else {
            if(1-fabs(valExpected.x/valActual.x)<1e-8 && 1-fabs(valExpected.y/valActual.y)<1e-8 && 1-fabs(valExpected.z/valActual.z)<1e-8) {
                result->failed=0;
            }
            else {
                result->failed=1;
            }
        }
        result->expected="<";
        result->expected.append(toString::convert(valExpected.x));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.y));
        result->expected.append(",");
        result->expected.append(toString::convert(valExpected.z));
        result->expected.append(">");
        result->actual="<";
        result->actual.append(toString::convert(valActual.x));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.y));
        result->actual.append(",");
        result->actual.append(toString::convert(valActual.z));
        result->actual.append(">");

        return !result->failed;
    }

};
