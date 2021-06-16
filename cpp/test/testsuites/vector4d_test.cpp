#include "../testframework/subtest.h"
#include "puma.h"


class Vector4D_Test : public SubTest {
public:

    Vector4D_Test() {
        testSuiteName = "Vector4D_Test";
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

    }

    static TestResult test1() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "Zero puma::Vec4<int> tests - construction";
        std::string testDescription = "Testing a zero puma::Vec4<int> using each constructor";
        TestResult result(suiteName, testName, 1, testDescription);

        puma::Vec4<int> intVec1;
        puma::Vec4<int> intVec2(0,0,0,0);
        puma::Vec4<int> intVec3(intVec1);

        if(!myAssertEquals(intVec1,intVec2, &result)) {
            return result;
        }
        if(!myAssertEquals(intVec1,intVec3, &result)) {
            return result;
        }
        if(!myAssertEquals(intVec3,intVec2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "Zero puma::Vec4<long> tests - construction";
        std::string testDescription = "Testing a zero puma::Vec4<long> using each constructor";
        TestResult result(suiteName, testName, 2, testDescription);

        puma::Vec4<long> longVec1;
        puma::Vec4<long> longVec2(0,0,0,0);
        puma::Vec4<long> longVec3(longVec1);

        if(!myAssertEquals(longVec1,longVec2, &result)) {
            return result;
        }
        if(!myAssertEquals(longVec1,longVec3, &result)) {
            return result;
        }
        if(!myAssertEquals(longVec3,longVec2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "Zero puma::Vec4<float> tests - construction";
        std::string testDescription = "Testing a zero puma::Vec4<float> using each constructor";
        TestResult result(suiteName, testName, 3, testDescription);

        puma::Vec4<float> floatVec1;
        puma::Vec4<float> floatVec2(0,0,0,0);
        puma::Vec4<float> floatVec3(floatVec1);

        if(!myAssertEquals(floatVec1,floatVec2, &result)) {
            return result;
        }
        if(!myAssertEquals(floatVec1,floatVec3, &result)) {
            return result;
        }
        if(!myAssertEquals(floatVec3,floatVec2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test4() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "Zero puma::Vec4<double> tests - construction";
        std::string testDescription = "Testing a zero puma::Vec4<double> using each constructor";
        TestResult result(suiteName, testName, 4, testDescription);

        puma::Vec4<double> doubleVec1;
        puma::Vec4<double> doubleVec2(0,0,0,0);
        puma::Vec4<double> doubleVec3(doubleVec1);
        puma::Vec4<double> doubleVec4(0,0,0);

        if(!myAssertEquals(doubleVec1,doubleVec2, &result)) {
            return result;
        }
        if(!myAssertEquals(doubleVec1,doubleVec3, &result)) {
            return result;
        }
        if(!myAssertEquals(doubleVec3,doubleVec2, &result)) {
            return result;
        }
        if(!myAssertEquals(doubleVec3,doubleVec4, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test5() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<double> tests: () operator";
        std::string testDescription;
        TestResult result(suiteName, testName, 5, testDescription);

        puma::Vec4<double> vec1;
        vec1(1.2,-1.2,1.2);
        puma::Vec4<double> vec2(1.2,-1.2,1.2);

        if(!myAssertEquals(vec1,vec2, &result)) {
            return result;
        }

        vec2.v = 1.2;
        vec1(1.2,-1.2,1.2,1.2);

        if(!myAssertEquals(vec1,vec2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test6() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<int> tests: () operator";
        std::string testDescription;
        TestResult result(suiteName, testName, 6, testDescription);

        puma::Vec4<int> vec1;
        vec1(1.2,-1.2,1.2);
        puma::Vec4<int> vec2(1.2,-1.2,1.2);

        if(!myAssertEquals(vec1,vec2, &result)) {
            return result;
        }

        puma::Vec4<int> vec3;
        vec3(1,-1,1);
        puma::Vec4<int> vec4(1,-1,1);

        if(!myAssertEquals(vec3,vec4, &result)) {
            return result;
        }

        puma::Vec4<int> vec5;
        vec5(1,-1,1);
        puma::Vec4<int> vec6(1.2,-1.2,1.2);

        if(!myAssertEquals(vec5,vec6, &result)) {
            return result;
        }

        puma::Vec4<int> vec7;
        vec7(1.2,-1.2,1.2);
        puma::Vec4<int> vec8(1,-1,1);

        if(!myAssertEquals(vec7,vec8, &result)) {
            return result;
        }

        puma::Vec4<int> vec9;
        vec9(1.2,-1.2,1.2,1.2);
        puma::Vec4<int> vec10(1.2,-1.2,1.2,1.2);

        if(!myAssertEquals(vec9,vec10, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test7() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<double> tests: - operator";
        std::string testDescription;
        TestResult result(suiteName, testName, 7, testDescription);

        puma::Vec4<double> vec1;
        vec1(1.2,-1.2,1.2);
        puma::Vec4<double> vec2(1.3,-1.3,1.3);

        puma::Vec4<double> vec3(0.1,-0.1,0.1);

        if(!myAssertEquals(vec2-vec1, vec3, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test8() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<int> tests: - operator";
        std::string testDescription;
        TestResult result(suiteName, testName, 8, testDescription);

        puma::Vec4<int> vec1;
        vec1(1,1,1);
        puma::Vec4<int> vec2(4,-5,8);

        puma::Vec4<int> vec3(3,-6,7);

        if(!myAssertEquals(vec2-vec1, vec3, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test9() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<double> tests: + operator";
        std::string testDescription;
        TestResult result(suiteName, testName, 9, testDescription);

        puma::Vec4<double> vec1;
        vec1(1.2,-1.2,1.2);
        puma::Vec4<double> vec2(1.3,-1.3,1.3);

        puma::Vec4<double> vec3(2.5,-2.5,2.5);

        if(!myAssertEquals(vec2+vec1, vec3, &result)) {
            return result;
        }
        if(!myAssertEquals(vec1+vec2, vec3, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test10() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<int> tests: - operator";
        std::string testDescription;
        TestResult result(suiteName, testName, 10, testDescription);

        puma::Vec4<int> vec1;
        vec1(1,1,1);
        puma::Vec4<int> vec2(4,-5,8);

        puma::Vec4<int> vec3(5,-4,9);

        if(!myAssertEquals(vec2+vec1, vec3, &result)) {
            return result;
        }
        if(!myAssertEquals(vec1+vec2, vec3, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test11() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<double> tests: * operator (positive double)";
        std::string testDescription;
        TestResult result(suiteName, testName, 11, testDescription);

        puma::Vec4<double> vec1;
        vec1(1.2,-1.2,1.2);
        puma::Vec4<double> vec2(3,-3,3);

        if(!myAssertEquals(vec1*2.5, vec2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test12() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<double> tests: * operator (negative double)";
        std::string testDescription;
        TestResult result(suiteName, testName, 12, testDescription);

        puma::Vec4<double> vec1;
        vec1(1.2,-1.2,1.2);
        puma::Vec4<double> vec2(-3,3,-3);

        if(!myAssertEquals(vec1*-2.5, vec2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test13() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<int> tests: * operator (positive int)";
        std::string testDescription;
        TestResult result(suiteName, testName, 13, testDescription);

        puma::Vec4<int> vec1;
        vec1(5,-6,7);
        puma::Vec4<int> vec2(10,-12,14);

        if(!myAssertEquals(vec1*2, vec2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test14() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<int> tests: * operator (negative int)";
        std::string testDescription;
        TestResult result(suiteName, testName, 14, testDescription);

        puma::Vec4<int> vec1;
        vec1(5,-6,7);
        puma::Vec4<int> vec2(-10,12,-14);

        if(!myAssertEquals(vec1*-2, vec2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test15() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<double> tests: / operator (positive double)";
        std::string testDescription;
        TestResult result(suiteName, testName, 15, testDescription);

        puma::Vec4<double> vec1;
        vec1(1.2,-1.2,1.2);
        puma::Vec4<double> vec2(0.48,-0.48,0.48);

        if(!myAssertEquals(vec1/2.5, vec2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test16() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<double> tests: / operator (negative double)";
        std::string testDescription;
        TestResult result(suiteName, testName, 16, testDescription);

        puma::Vec4<double> vec1;
        vec1(1.2,-1.2,1.2);
        puma::Vec4<double> vec2(-0.48,0.48,-0.48);

        if(!myAssertEquals(vec1/-2.5, vec2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test17() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<int> tests: / operator (positive int)";
        std::string testDescription;
        TestResult result(suiteName, testName, 17, testDescription);

        puma::Vec4<int> vec1;
        vec1(5,-6,7);
        puma::Vec4<int> vec2(2,-3,3);

        if(!myAssertEquals(vec1/2, vec2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test18() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<int> tests: / operator (negative int)";
        std::string testDescription;
        TestResult result(suiteName, testName, 18, testDescription);

        puma::Vec4<int> vec1;
        vec1(5,-6,7);
        puma::Vec4<int> vec2(-2,3,-3);

        if(!myAssertEquals(vec1/-2, vec2, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test19() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<double> tests: = operator";
        std::string testDescription;
        TestResult result(suiteName, testName, 19, testDescription);

        puma::Vec4<double> vec1(1.2,3.4,5.6);
        puma::Vec4<double> vec2(1.3,3.5,5.7);

        vec1=vec2;

        vec2(0,0,0);

        puma::Vec4<double> vec3(1.3,3.5,5.7);

        if(!myAssertEquals(vec1, vec3, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test20() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<int> tests: = operator";
        std::string testDescription;
        TestResult result(suiteName, testName, 20, testDescription);

        puma::Vec4<int> vec1(2,4,6);
        puma::Vec4<int> vec2(3,5,7);

        vec1=vec2;

        vec2(0,0,0);

        puma::Vec4<int> vec3(3,5,7);

        if(!myAssertEquals(vec1, vec3, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test21() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<double> tests: cross product";
        std::string testDescription;
        TestResult result(suiteName, testName, 21, testDescription);

        puma::Vec4<double> vec1(1.2,-3.4,5.6);
        puma::Vec4<double> vec2(1.3,3.5,5.7);

        puma::Vec4<double> vec3(-38.98, 0.44, 8.62);

        if(!myAssertEquals(vec1.cross(vec2), vec3, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test22() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<float> tests: cross product";
        std::string testDescription;
        TestResult result(suiteName, testName, 22, testDescription);

        puma::Vec4<float> vec1(1.2,-3.4,5.6);
        puma::Vec4<float> vec2(1.3,3.5,5.7);

        puma::Vec4<float> vec3(-38.98, 0.44, 8.62);

        if(!myAssertEquals(vec1.cross(vec2), vec3, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test23() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<double> tests: dot product (values)";
        std::string testDescription;
        TestResult result(suiteName, testName, 23, testDescription);

        puma::Vec4<double> vec1(1.2,-3.4,5.6);
        puma::Vec4<double> vec2(1.3,3.5,5.7);

        double aDot = vec1.dot(vec2);
        double eDot = (1.2*1.3) + (-3.4*3.5) + (5.6*5.7);

        if(!assertEquals(eDot, aDot, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test24() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<double> tests: dot product (zeros)";
        std::string testDescription;
        TestResult result(suiteName, testName, 24, testDescription);

        puma::Vec4<double> vec1(1.2,-3.4,5.6);
        puma::Vec4<double> vec2(0,0,0);

        double aDot = vec1.dot(vec2);
        double eDot = 0;

        if(!assertEquals(eDot, aDot, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test25() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<float> tests: dot product (values)";
        std::string testDescription;
        TestResult result(suiteName, testName, 25, testDescription);

        puma::Vec4<float> vec1(1.2,-3.4,5.6);
        puma::Vec4<float> vec2(1.3,3.5,5.7);

        float aDot = vec1.dot(vec2);
        float eDot = (1.2*1.3) + (-3.4*3.5) + (5.6*5.7);

        if(!assertEquals(eDot, aDot, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test26() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<float> tests: dot product (zeros)";
        std::string testDescription;
        TestResult result(suiteName, testName, 26, testDescription);

        puma::Vec4<float> vec1(1.2,-3.4,5.6);
        puma::Vec4<float> vec2(0,0,0);

        float aDot = vec1.dot(vec2);
        float eDot = 0;

        if(!assertEquals(eDot, aDot, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test27() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<double> tests: norm";
        std::string testDescription;
        TestResult result(suiteName, testName, 24, testDescription);

        puma::Vec4<double> vec1(1,1,1);
        puma::Vec4<double> vec2 = vec1.norm();

        if(!myAssertEquals(vec1, puma::Vec4<double>(1,1,1), &result)) {
            return result;
        }
        if(!myAssertEquals(vec2, puma::Vec4<double>(1/sqrt(3),1/sqrt(3),1/sqrt(3)), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test28() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<float> tests: norm";
        std::string testDescription;
        TestResult result(suiteName, testName, 24, testDescription);

        puma::Vec4<float> vec1(1,1,1);
        puma::Vec4<float> vec2 = vec1.norm();

        if(!myAssertEquals(vec1, puma::Vec4<float>(1,1,1), &result)) {
            return result;
        }
        if(!myAssertEquals(vec2, puma::Vec4<float>(1/sqrt(3),1/sqrt(3),1/sqrt(3)), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test29() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<double> tests: magnitude";
        std::string testDescription;
        TestResult result(suiteName, testName, 24, testDescription);

        puma::Vec4<float> vec1(1,1,1);
        float mag = vec1.magnitude();

        if(!assertEquals((float)sqrt(3), mag, &result)) {
            return result;
        }

        return result;
    }

    static TestResult test30() {
        std::string suiteName = "Vector4D Tests: ";
        std::string testName = "puma::Vec4<float> tests: magnitude";
        std::string testDescription;
        TestResult result(suiteName, testName, 24, testDescription);

        puma::Vec4<float> vec1(1,1,1);
        float mag = vec1.magnitude();

        if(!assertEquals((float)sqrt(3), mag, &result)) {
            return result;
        }


        return result;
    }

    static bool myAssertEquals(puma::Vec4<int> valExpected, puma::Vec4<int> valActual, TestResult *result) {
        if(valExpected.x==valActual.x && valExpected.y==valActual.y && valExpected.z==valActual.z && valExpected.v==valActual.v) {
            result->failed=0;
        }
        else {
            result->failed=1;
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

    static bool myAssertEquals(puma::Vec4<long> valExpected, puma::Vec4<long> valActual, TestResult *result) {
        if(valExpected.x==valActual.x && valExpected.y==valActual.y && valExpected.z==valActual.z && valExpected.v==valActual.v) {
            result->failed=0;
        }
        else {
            result->failed=1;
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

    static bool myAssertEquals(puma::Vec4<float> valExpected, puma::Vec4<float> valActual, TestResult *result) {

        if(valActual.x==0||valExpected.x==0||valActual.y==0||valExpected.y==0||valActual.z==0||valExpected.z==0||valActual.v==0||valExpected.v==0) {
            if(fabs(valExpected.x-valActual.x)<1e-6 && fabs(valExpected.y-valActual.y)<1e-6 && fabs(valExpected.z-valActual.z)<1e-6 && fabs(valExpected.v-valActual.v)<1e-6) {
                result->failed=0;
            }
            else {
                result->failed=1;
            }
        }
        else {
            if(1-fabs(valExpected.x/valActual.x)<1e-6 && 1-fabs(valExpected.y/valActual.y)<1e-6 && 1-fabs(valExpected.z/valActual.z)<1e-6 && 1-fabs(valExpected.v/valActual.v)<1e-6) {
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

    static bool myAssertEquals(puma::Vec4<double> valExpected, puma::Vec4<double> valActual, TestResult *result) {

        if(valActual.x==0||valExpected.x==0||valActual.y==0||valExpected.y==0||valActual.z==0||valExpected.z==0||valActual.v==0||valExpected.v==0) {
            if(fabs(valExpected.x-valActual.x)<1e-8 && fabs(valExpected.y-valActual.y)<1e-8 && fabs(valExpected.z-valActual.z)<1e-8 && fabs(valExpected.v-valActual.v)<1e-8) {
                result->failed=0;
            }
            else {
                result->failed=1;
            }
        }
        else {
            if(1-fabs(valExpected.x/valActual.x)<1e-8 && 1-fabs(valExpected.y/valActual.y)<1e-8 && 1-fabs(valExpected.z/valActual.z)<1e-8 && 1-fabs(valExpected.v/valActual.v)<1e-8) {
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



