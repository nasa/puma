#include "../testframework/subtest.h"
#include "puma.h"

#include <cmath>


class Matrix_Test : public SubTest
{
public:

    Matrix_Test() {
        testSuiteName = "Matrix_Test";
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
        //tests.push_back(test13);
        //tests.push_back(test14);
        tests.push_back(test15);
        tests.push_back(test16);
        tests.push_back(test17);
        //tests.push_back(test18);
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
        //tests.push_back(test49);
        //tests.push_back(test50);
        tests.push_back(test51);
        tests.push_back(test52);
        tests.push_back(test53);
        tests.push_back(test54);
        tests.push_back(test55);
        tests.push_back(test56);
        tests.push_back(test57);
        tests.push_back(test58);
        tests.push_back(test59);
        tests.push_back(test60);
        //tests.push_back(test61);
        //tests.push_back(test62);
        tests.push_back(test63);
        tests.push_back(test64);
        tests.push_back(test65);
        tests.push_back(test66);
        tests.push_back(test67);
        tests.push_back(test68);
        //tests.push_back(test69);
        //tests.push_back(test70);
        tests.push_back(test71);
        tests.push_back(test72);
        tests.push_back(test73);
        tests.push_back(test74);
        tests.push_back(test75);
        tests.push_back(test76);
        //tests.push_back(test77);
        //tests.push_back(test78);
        tests.push_back(test79);
        tests.push_back(test80);
        tests.push_back(test81);
        tests.push_back(test82);
        tests.push_back(test83);
        tests.push_back(test84);
        tests.push_back(test85);
        tests.push_back(test86);
        //tests.push_back(test87);
        //tests.push_back(test88);

    }

    static TestResult test1() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Creating and empty puma::Matrix<double>";
        std::string testDescription = "Testing to make sure that an empty matrix of type double can be made. The resulting size, and dimensions in each direction are tested";
        TestResult result(suiteName, testName, 1, testDescription);

        puma::Matrix<double> m;

        if(!assertEquals((long)0,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.ZY(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test2() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Creating and empty puma::Matrix<int>";
        std::string testDescription = "Testing to make sure that an empty matrix of type int can be made. The resulting size, and dimensions in each direction are tested";
        TestResult result(suiteName,testName,2,testDescription);

        puma::Matrix<int> m;

        if(!assertEquals((long)0,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.ZY(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test3() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Creating puma::Matrix<double> of size 100^3 w/o a default value";
        std::string testDescription = "Testing that all dimensions are appropriately defined, and that the matrix locations can be accessed";
        TestResult result(suiteName,testName,3,testDescription);

        puma::Matrix<double> m(100,100,100);

        if(!assertEquals((long)100*100*100,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100,(long)m.ZY(), &result)) {
            return result;
        }

        try {
            double testDouble = 1;
            testDouble = m(10,10,10);
            m(10,10,10) = testDouble;
        }
        catch (std::exception e) {
            result.failed=1;
            result.actual="exception";
            result.expected="no exception";
        }

        return result;
    }

    static TestResult test4() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Creating puma::Matrix<double> of size 100^3 with a default value of 10";
        std::string testDescription = "Testing that all dimensions are appropriately defined, and that the default value is stored to all point in the matrix";
        TestResult result(suiteName,testName,4,testDescription);

        puma::Matrix<double> m(100,100,100, 10.0);

        if(!assertEquals((long)100*100*100,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100,(long)m.ZY(), &result)) {
            return result;
        }

        for(int i=0;i<m.X();i++){
            for(int j=0;j<m.Y();j++){
                for(int k=0;k<m.Z();k++){
                    if(fabs(m(i,j,k)-10)>1e-8){
                        result.failed=1;
                        result.expected=toString::convert((double)10);
                        result.actual=toString::convert(m(i,j,k));
                        return result;
                    }
                }
            }
        }

        return result;
    }


    static TestResult test5() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Creating puma::Matrix<double> of size 100^3 with a default value of 0";
        std::string testDescription = "Testing that all dimensions are appropriately defined, and that the default value is stored to all point in the matrix";
        TestResult result(suiteName,testName,5,testDescription);

        puma::Matrix<double> m(100,100,100, 0);

        if(!assertEquals((long)100*100*100,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100,(long)m.ZY(), &result)) {
            return result;
        }

        for(int i=0;i<m.X();i++){
            for(int j=0;j<m.Y();j++){
                for(int k=0;k<m.Z();k++){
                    if(fabs(m(i,j,k)-0)>1e-8){
                        result.failed=1;
                        result.expected=toString::convert((double)0);
                        result.actual=toString::convert(m(i,j,k));
                        return result;
                    }
                }
            }
        }

        return result;
    }

    static TestResult test6() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Testing set(T) function of matrix class";
        std::string testDescription = "Creating a 100^3 matrix with no default value, then assigning a value at every point with set(T) function. Testing to make sure that the dimensions are correct and the values have been assigned correctly";
        TestResult result(suiteName,testName,6,testDescription);

        puma::Matrix<double> m(98,99,100);

        if(!assertEquals((long)98*99*100,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)98,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)99,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*99,(long)m.ZY(), &result)) {
            return result;
        }

        m.set(10.0);

        for(int i=0;i<m.X();i++){
            for(int j=0;j<m.Y();j++){
                for(int k=0;k<m.Z();k++){
                    if(fabs(m(i,j,k)-10.0)>1e-8){
                        result.failed=1;
                        result.expected=toString::convert((double)10);
                        result.actual=toString::convert(m(i,j,k));
                        return result;
                    }
                }
            }
        }

        return result;
    }

    static TestResult test7() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Testing set(T) function of matrix class";
        std::string testDescription = "Creating a 100^3 matrix with a default value of 0, then assigning a value at every point with set(T) function. Testing to make sure that the dimensions are correct and the values have been assigned correctly both before and after set(T)";
        TestResult result(suiteName,testName,7,testDescription);

        puma::Matrix<double> m(98,99,100,0);

        if(!assertEquals((long)98*99*100,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)98,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)99,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*99,(long)m.ZY(), &result)) {
            return result;
        }

        for(int i=0;i<m.X();i++){
            for(int j=0;j<m.Y();j++){
                for(int k=0;k<m.Z();k++){
                    if(fabs(m(i,j,k)-0)>1e-8){
                        result.failed=1;
                        result.expected=toString::convert((double)0);
                        result.actual=toString::convert(m(i,j,k));
                        return result;
                    }
                }
            }
        }

        m.set(10.0);

        for(int i=0;i<m.X();i++){
            for(int j=0;j<m.Y();j++){
                for(int k=0;k<m.Z();k++){
                    if(fabs(m(i,j,k)-10.0)>1e-8){
                        result.failed=1;
                        result.expected=toString::convert((double)10);
                        result.actual=toString::convert(m(i,j,k));
                        return result;
                    }
                }
            }
        }

        return result;
    }

    static TestResult test8() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Testing resize function of matrix class to size 0^3";
        std::string testDescription = "Creating a 100^3 matrix with a default value of 10, then resizing the matrix to 0^3 with no default value. Testing to make sure that the dimensions are correct";
        TestResult result(suiteName,testName,8,testDescription);


        puma::Matrix<double> m(100,100,100, 10);

        if(!assertEquals((long)100*100*100,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100,(long)m.ZY(), &result)) {
            return result;
        }

        m.resize(0,0,0);

        if(!assertEquals((long)0*0*0,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)0*0,(long)m.ZY(), &result)) {
            return result;
        }

        m.resize(0,0,0,5);

        if(!assertEquals((long)0*0*0,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)0*0,(long)m.ZY(), &result)) {
            return result;
        }

        return result;
    }

    static TestResult test9() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Testing resize function of matrix class from size 0^3 to size 100^3 and size 98,99,100";
        std::string testDescription = "Creating a 0^3 matrix w/o a default value, then resizing the matrix to 100^3 with no default value and finally to 98*99*100 with a default value of 1e-5. Testing to make sure that the dimensions are correct and that the value is correct (if applicable)";
        TestResult result(suiteName,testName,9,testDescription);

        puma::Matrix<double> m(0,0,0);

        if(!assertEquals((long)0*0*0,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)0*0,(long)m.ZY(), &result)) {
            return result;
        }

        m.resize(100,100,100);

        if(!assertEquals((long)100*100*100,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100,(long)m.ZY(), &result)) {
            return result;
        }

        m.set(1e-5);

        if(!assertEquals((long)100*100*100,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*100,(long)m.ZY(), &result)) {
            return result;
        }

        for(int i=0;i<m.X();i++){
            for(int j=0;j<m.Y();j++){
                for(int k=0;k<m.Z();k++){
                    if(fabs(m(i,j,k)-1e-5)>1e-8){
                        result.failed=1;
                        result.expected=toString::convert((double)1e-5);
                        result.actual=toString::convert(m(i,j,k));
                        return result;
                    }
                }
            }
        }


        m.resize(0,0,0);

        if(!assertEquals((long)0*0*0,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)0,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)0*0,(long)m.ZY(), &result)) {
            return result;
        }

        m.resize(98,99,100,1e-5);

        if(!assertEquals((long)98*99*100,(long)m.size(), &result)) {
            return result;
        }

        if(!assertEquals((long)98,(long)m.X(), &result)) {
            return result;
        }

        if(!assertEquals((long)99,(long)m.Y(), &result)) {
            return result;
        }

        if(!assertEquals((long)100,(long)m.Z(), &result)) {
            return result;
        }

        if(!assertEquals((long)100*99,(long)m.ZY(), &result)) {
            return result;
        }

        for(int i=0;i<m.X();i++){
            for(int j=0;j<m.Y();j++){
                for(int k=0;k<m.Z();k++){
                    if(fabs(m(i,j,k)-1e-5)>1e-8){
                        result.failed=1;
                        result.expected=toString::convert((double)1e-5);
                        result.actual=toString::convert(m(i,j,k));
                        return result;
                    }
                }
            }
        }

        return result;
    }

    static TestResult test10() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Testing matrix reduce without any matrix values";
        std::string testDescription = "Creating a 0^3 matrix and testing the matrix reduce function";
        TestResult result(suiteName,testName,10,testDescription);

        puma::Matrix<double> m(0,0,0);

        assertEquals((double)0, m.reduce(),&result);

        return result;
    }

    static TestResult test11() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Testing matrix reduce with constant values";
        std::string testDescription = "Creating a 100^3 matrix with constant value and testing the matrix reduce function";
        TestResult result(suiteName,testName,11,testDescription);

        puma::Matrix<double> m(100,100,100,10);

        assertEquals((double)10*m.size(), m.reduce(),&result);

        return result;
    }

    static TestResult test12() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Testing matrix reduce with constant values";
        std::string testDescription = "Creating a 100^3 matrix with non-constant values and testing the matrix reduce function";
        TestResult result(suiteName,testName,12,testDescription);

        puma::Matrix<double> m(100,100,100,0);

        for(int i=0; i<m.X()/2; i++) {
            for(int j=0; j<m.Y(); j++) {
                for(int k=0; k<m.Z(); k++) {
                    m(i,j,k)=10;
                }
            }
        }

        for(int i=m.X()/2; i<m.X(); i++) {
            for(int j=0; j<m.Y(); j++) {
                for(int k=0; k<m.Z(); k++) {
                    m(i,j,k)=5;
                }
            }
        }

        assertEquals((double)10*m.size()/2+(double)5*m.size()/2, m.reduce(),&result);

        return result;
    }

    static TestResult test13() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Testing matrix of size 1400^3 and reduce function";
        std::string testDescription = "Creating a 1400^3 matrix and testing its reduce function. Test is to ensure that matrix can handle more than maxInt elements";
        TestResult result(suiteName,testName,13,testDescription);

        puma::Matrix<double> m(1400,1400,1400,10);

        assertEquals(m.size()*(double)10, m.reduce(),&result);

        return result;
    }

    static TestResult test14() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Average - Test 1";
        std::string testDescription = "Int values, > 2.1b, variable values";
        TestResult result(suiteName,testName,14,testDescription);

        puma::Matrix<int> m(1400,1400,1400,10);

        for(int i=m.X()/2; i<m.X(); i++) {
            for(int j=0; j<m.Y(); j++) {
                for(int k=0; k<m.Z(); k++) {
                    m(i,j,k)=0;
                }
            }
        }

        assertEquals((double)5, m.average(),&result);

        return result;
    }

    static TestResult test15() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Average - Test 1";
        std::string testDescription = "Int values, < 2.1b, constant values";
        TestResult result(suiteName,testName,15,testDescription);

        puma::Matrix<int> m(200,200,200,10);

        assertEquals((double)10, m.average(),&result);

        return result;
    }

    static TestResult test16() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Average - Test 1";
        std::string testDescription = "Int values, < 2.1b, variable values";
        TestResult result(suiteName,testName,16,testDescription);

        puma::Matrix<short> m(200,200,200,10);

        for(int i=m.X()/2; i<m.X(); i++) {
            for(int j=0; j<m.Y(); j++) {
                for(int k=0; k<m.Z(); k++) {
                    m(i,j,k)=0;
                }
            }
        }

        assertEquals((double)5, m.average(),&result);

        return result;
    }

    static TestResult test17() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Average - Test 1";
        std::string testDescription = "Int values, empty";
        TestResult result(suiteName,testName,17,testDescription);

        puma::Matrix<int> m;

        assertEquals((double)0, m.average(),&result);

        return result;
    }

    static TestResult test18() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Average - Test 1";
        std::string testDescription = "double values, > 2.1b, variable values";
        TestResult result(suiteName,testName,18,testDescription);

        puma::Matrix<double> m(1400,1400,1400,10);

        for(int i=m.X()/2; i<m.X(); i++) {
            for(int j=0; j<m.Y(); j++) {
                for(int k=0; k<m.Z(); k++) {
                    m(i,j,k)=0;
                }
            }
        }

        assertEquals((double)5, m.average(),&result);

        return result;
    }

    static TestResult test19() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Average - Test 1";
        std::string testDescription = "double values, < 2.1b, constant values";
        TestResult result(suiteName,testName,19,testDescription);

        puma::Matrix<double> m(200,200,200,10);

        assertEquals((double)10, m.average(),&result);

        return result;
    }

    static TestResult test20() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Average - Test 1";
        std::string testDescription = "double values, < 2.1b, variable values";
        TestResult result(suiteName,testName,20,testDescription);


        puma::Matrix<double> m(200,200,200,10);

        for(int i=m.X()/2; i<m.X(); i++) {
            for(int j=0; j<m.Y(); j++) {
                for(int k=0; k<m.Z(); k++) {
                    m(i,j,k)=0;
                }
            }
        }

        assertEquals((double)5, m.average(),&result);

        return result;
    }

    static TestResult test21() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Average - Test 1";
        std::string testDescription = "double values, empty";
        TestResult result(suiteName,testName,21,testDescription);

        puma::Matrix<double> m;

        assertEquals((double)0, m.average(),&result);

        return result;
    }

    static TestResult test22() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 1";
        std::string testDescription = "Simpel Test case";
        TestResult result(suiteName,testName,22,testDescription);

        puma::Matrix<double> m(100,100,100,10);
        m.set(50,99,0,99,0,99,5);

        assertEquals((double)7.5, m.average(), &result);

        return result;
    }

    static TestResult test23() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 2";
        std::string testDescription = "Testing the range error catching";
        TestResult result(suiteName,testName,23,testDescription);

        puma::Matrix<double> m(100,100,100,10);

        bool success = m.set(50,99,0,99,0,99,5);
        assertEquals(true, success, &result);
        return result;

    }

    static TestResult test24() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 2";
        std::string testDescription = "Testing the range error catching";
        TestResult result(suiteName,testName,24,testDescription);

        puma::Matrix<double> m(100,100,100,10);

        bool success = m.set(99,99,0,99,0,99,5);
        assertEquals(true, success, &result);
        return result;

    }

    static TestResult test25() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 2";
        std::string testDescription = "Testing the range error catching";
        TestResult result(suiteName,testName,25,testDescription);

        puma::Matrix<double> m(100,100,100,10);

        bool success = m.set(99,99,99,99,99,99,5);
        assertEquals(true, success, &result);
        return result;

    }

    static TestResult test26() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 2";
        std::string testDescription = "Testing the range error catching";
        TestResult result(suiteName,testName,26,testDescription);

        puma::Matrix<double> m(100,100,100,10);

        bool success = m.set(2,99,99,99,99,99,5);
        assertEquals(true, success, &result);
        return result;

    }

    static TestResult test27() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 2";
        std::string testDescription = "Testing the range error catching";
        TestResult result(suiteName,testName,27,testDescription);

        puma::Matrix<double> m(100,100,100,10);

        bool success = m.set(-1,99,99,99,99,99,5);
        assertEquals(false, success, &result);
        return result;

    }

    static TestResult test28() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 2";
        std::string testDescription = "Testing the range error catching";
        TestResult result(suiteName,testName,28,testDescription);

        puma::Matrix<double> m(100,100,100,10);

        bool success = m.set(0,100,99,99,99,99,5);
        assertEquals(false, success, &result);
        return result;

    }

    static TestResult test29() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 2";
        std::string testDescription = "Testing the range error catching";
        TestResult result(suiteName,testName,29,testDescription);

        puma::Matrix<double> m(100,100,100,10);

        bool success = m.set(51,20,99,99,99,99,5);
        assertEquals(false, success, &result);
        return result;

    }

    static TestResult test30() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 2";
        std::string testDescription = "Testing the range error catching";
        TestResult result(suiteName,testName,30,testDescription);

        puma::Matrix<double> m(100,100,100,10);

        bool success = m.set(0,0,-1,99,99,99,5);
        assertEquals(false, success, &result);
        return result;

    }

    static TestResult test31() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 2";
        std::string testDescription = "Testing the range error catching";
        TestResult result(suiteName,testName,31,testDescription);

        puma::Matrix<double> m(100,100,100,10);

        bool success = m.set(0,0,0,100,99,99,5);
        assertEquals(false, success, &result);
        return result;

    }

    static TestResult test32() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 2";
        std::string testDescription = "Testing the range error catching";
        TestResult result(suiteName,testName,32,testDescription);

        puma::Matrix<double> m(100,100,100,10);

        bool success = m.set(0,0,51,20,99,99,5);
        assertEquals(false, success, &result);
        return result;

    }

    static TestResult test33() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 2";
        std::string testDescription = "Testing the range error catching";
        TestResult result(suiteName,testName,33,testDescription);

        puma::Matrix<double> m(100,100,100,10);

        bool success = m.set(0,0,0,0,-1,99,5);
        assertEquals(false, success, &result);
        return result;

    }

    static TestResult test34() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 2";
        std::string testDescription = "Testing the range error catching";
        TestResult result(suiteName,testName,34,testDescription);

        puma::Matrix<double> m(100,100,100,10);

        bool success = m.set(0,0,0,0,0,100,5);
        assertEquals(false, success, &result);
        return result;

    }

    static TestResult test35() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 2";
        std::string testDescription = "Testing the range error catching";
        TestResult result(suiteName,testName,35,testDescription);

        puma::Matrix<double> m(100,100,100,10);

        bool success = m.set(0,0,0,0,51,20,5);
        assertEquals(false, success, &result);
        return result;

    }

    static TestResult test36() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Set Range: Test 2";
        std::string testDescription = "Testing setting a single value";
        TestResult result(suiteName,testName,36,testDescription);

        puma::Matrix<double> m(100,100,100,0);

        m.set(0,0,0,0,0,0,10);
        assertEquals((double)10, m(0,0,0) , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)10, m.reduce() , &result);
        if(result.failed) {
            return result;
        }

        puma::Matrix<double> m2(&m);
        assertEquals((double)10, m2.reduce() , &result);
        if(result.failed) {
            return result;
        }

        puma::Matrix<double> m3(10,10,10,2);
        m.copy(&m3);
        assertEquals((double)2*m.size(), m.reduce() , &result);
        if(result.failed) {
            return result;
        }

        m3.copy(&m2);
        assertEquals((double)10, m3.reduce() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test37() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Copy Test 2";
        std::string testDescription = "Empty Matrix - short";
        TestResult result(suiteName,testName,37,testDescription);

        puma::Matrix<short> m1;
        puma::Matrix<short> m2(100,100,100,0);

        bool success = m2.copy(&m1);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((long)0, m1.size() , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((long)0, m2.size() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test38() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Copy Test 3";
        std::string testDescription = "Empty Matrix - float";
        TestResult result(suiteName,testName,38,testDescription);

        puma::Matrix<float> m1;
        puma::Matrix<float> m2(100,100,100,0);

        bool success = m2.copy(&m1);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((long)0, m1.size() , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((long)0, m2.size() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test39() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue(self) Test 1";
        std::string testDescription = "Empty Matrix - short";
        TestResult result(suiteName,testName,39,testDescription);

        puma::Matrix<short> m1;

        bool success = m1.flipAroundValue(0.5);

        assertEquals(false, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((long)0, m1.size() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test40() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue Test 2";
        std::string testDescription = "Empty Matrix - float";
        TestResult result(suiteName,testName,40,testDescription);

        puma::Matrix<float> m1;

        bool success = m1.flipAroundValue(0.5);

        assertEquals(false, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((long)0, m1.size() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test41() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue Test 3";
        std::string testDescription = "Flip Around 0 - short";
        TestResult result(suiteName,testName,41,testDescription);

        puma::Matrix<short> m1(100,100,100,5);
        m1.set(0,49,0,99,0,99,10);

        bool success = m1.flipAroundValue(0);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)-7.5, m1.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test42() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue Test 4";
        std::string testDescription = "Flip Around 0 - float";
        TestResult result(suiteName,testName,42,testDescription);

        puma::Matrix<float> m1(100,100,100,5);
        m1.set(0,49,0,99,0,99,10.2);

        bool success = m1.flipAroundValue(0);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)-7.6, (double)m1.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test43() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue Test 5";
        std::string testDescription = "Flip Around min - short";
        TestResult result(suiteName,testName,43,testDescription);

        puma::Matrix<short> m1(100,100,100,5);
        m1.set(0,49,0,99,0,99,10);

        double flip = m1.min();

        bool success = m1.flipAroundValue(flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)2.5, m1.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test44() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue Test 6";
        std::string testDescription = "Flip Around min - float";
        TestResult result(suiteName,testName,44,testDescription);

        puma::Matrix<float> m1(100,100,100,5);
        m1.set(0,49,0,99,0,99,5.5);

        double flip = m1.min();

        bool success = m1.flipAroundValue(flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)4.75, m1.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test45() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue Test 7";
        std::string testDescription = "Flip Around max - short";
        TestResult result(suiteName,testName,45,testDescription);

        puma::Matrix<short> m1(100,100,100,5);
        m1.set(0,49,0,99,0,99,10);

        double flip = m1.max();

        bool success = m1.flipAroundValue(flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)7.5, m1.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test46() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue Test 8";
        std::string testDescription = "Flip Around max - float";
        TestResult result(suiteName,testName,46,testDescription);

        puma::Matrix<float> m1(100,100,100,5);
        m1.set(0,49,0,99,0,99,5.5);

        double flip = m1.max();

        bool success = m1.flipAroundValue(flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)5.25, m1.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test47() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue Test 9";
        std::string testDescription = "Flip Around middle - short";
        TestResult result(suiteName,testName,47,testDescription);

        puma::Matrix<short> m1(100,100,100,-5);
        m1.set(0,49,0,99,0,99,-10);

        double flip = (m1.max()+m1.min())/2.0;

        bool success = m1.flipAroundValue(flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)-10, m1.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test48() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue Test 10";
        std::string testDescription = "Flip Around middle - float";
        TestResult result(suiteName,testName,48,testDescription);

        puma::Matrix<float> m1(100,100,100,-5);
        m1.set(0,49,0,99,0,99,-5.5);

        double flip = (m1.max()+m1.min())/2.0;

        bool success = m1.flipAroundValue(flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)-5.5, m1.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test49() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue Test 11";
        std::string testDescription = "Flip Around middle - short > 2.1b";
        TestResult result(suiteName,testName,49,testDescription);

        puma::Matrix<short> m1(1300,1300,1300,-5);
        m1.set(0,49,0,99,0,99,-10);

        double flip = (m1.max()+m1.min())/2.0;

        bool success = m1.flipAroundValue(flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)-10, m1.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test50() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue Test 12";
        std::string testDescription = "Flip Around middle - foat > 2.1b";
        TestResult result(suiteName,testName,50,testDescription);

        puma::Matrix<float> m1(1300,1300,1300,-5);
        m1.set(0,49,0,99,0,99,-5.5);

        double flip = (m1.max()+m1.min())/2.0;

        bool success = m1.flipAroundValue(flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)-5.5, m1.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test51() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue(other) Test 1";
        std::string testDescription = "Empty Matrix - short";
        TestResult result(suiteName,testName,51,testDescription);

        puma::Matrix<short> m1;
        puma::Matrix<float> m2;

        bool success = m1.flipAroundValue(&m2,0.5);

        assertEquals(false, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((long)0, m2.size() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test52() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue(other) Test 2";
        std::string testDescription = "Empty Matrix - float";
        TestResult result(suiteName,testName,52,testDescription);

        puma::Matrix<float> m1;
        puma::Matrix<float> m2;

        bool success = m1.flipAroundValue(&m2,0.5);

        assertEquals(false, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((long)0, m2.size() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test53() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue(other) Test 3";
        std::string testDescription = "Flip Around 0 - short";
        TestResult result(suiteName,testName,53,testDescription);

        puma::Matrix<short> m1(100,100,100,5);
        m1.set(0,49,0,99,0,99,10);

        puma::Matrix<float> m2;

        bool success = m1.flipAroundValue(&m2,0);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)-7.5, m2.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test54() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue(other) Test 4";
        std::string testDescription = "Flip Around 0 - float";
        TestResult result(suiteName,testName,54,testDescription);

        puma::Matrix<float> m1(100,100,100,5);
        m1.set(0,49,0,99,0,99,10.2);

        puma::Matrix<float> m2;

        bool success = m1.flipAroundValue(&m2,0);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)-7.6, (double)m2.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test55() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue(other) Test 5";
        std::string testDescription = "Flip Around min - short";
        TestResult result(suiteName,testName,55,testDescription);

        puma::Matrix<short> m1(100,100,100,5);
        m1.set(0,49,0,99,0,99,10);

        puma::Matrix<float> m2;

        double flip = m1.min();

        bool success = m1.flipAroundValue(&m2,flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)2.5, m2.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test56() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue(other) Test 6";
        std::string testDescription = "Flip Around min - float";
        TestResult result(suiteName,testName,56,testDescription);

        puma::Matrix<float> m1(100,100,100,5);
        m1.set(0,49,0,99,0,99,5.5);

        puma::Matrix<float> m2;

        double flip = m1.min();

        bool success = m1.flipAroundValue(&m2,flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)4.75, m2.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test57() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue(other) Test 7";
        std::string testDescription = "Flip Around max - short";
        TestResult result(suiteName,testName,57,testDescription);

        puma::Matrix<short> m1(100,100,100,5);
        m1.set(0,49,0,99,0,99,10);

        puma::Matrix<float> m2;

        double flip = m1.max();

        bool success = m1.flipAroundValue(&m2,flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)7.5, m2.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test58() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue(other) Test 8";
        std::string testDescription = "Flip Around max - float";
        TestResult result(suiteName,testName,58,testDescription);

        puma::Matrix<float> m1(100,100,100,5);
        m1.set(0,49,0,99,0,99,5.5);

        puma::Matrix<float> m2;

        double flip = m1.max();

        bool success = m1.flipAroundValue(&m2,flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)5.25, m2.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test59() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue(other) Test 9";
        std::string testDescription = "Flip Around middle - short";
        TestResult result(suiteName,testName,59,testDescription);

        puma::Matrix<short> m1(100,100,100,-5);
        m1.set(0,49,0,99,0,99,-10);

        puma::Matrix<float> m2;

        double flip = (m1.max()+m1.min())/2.0;

        bool success = m1.flipAroundValue(&m2,flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)-10, m2.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test60() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue(other) Test 10";
        std::string testDescription = "Flip Around middle - float";
        TestResult result(suiteName,testName,60,testDescription);

        puma::Matrix<float> m1(100,100,100,-5);
        m1.set(0,49,0,99,0,99,-5.5);

        puma::Matrix<float> m2;

        double flip = (m1.max()+m1.min())/2.0;

        bool success = m1.flipAroundValue(&m2,flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)-5.5, m2.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test61() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue(other) Test 11";
        std::string testDescription = "Flip Around middle - short > 2.1b";
        TestResult result(suiteName,testName,61,testDescription);

        puma::Matrix<short> m1(1300,1300,1300,-5);
        m1.set(0,49,0,99,0,99,-10);

        puma::Matrix<float> m2;

        double flip = (m1.max()+m1.min())/2.0;

        bool success = m1.flipAroundValue(&m2,flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)-10, m2.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test62() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "FlipAroundValue(other) Test 12";
        std::string testDescription = "Flip Around middle - foat > 2.1b";
        TestResult result(suiteName,testName,62,testDescription);

        puma::Matrix<float> m1(1300,1300,1300,-5);
        m1.set(0,49,0,99,0,99,-5.5);

        puma::Matrix<float> m2;

        double flip = (m1.max()+m1.min())/2.0;

        bool success = m1.flipAroundValue(&m2,flip);

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)-5.5, m2.average() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test63() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize Test 1";
        std::string testDescription = "Empty Matrix - short";
        TestResult result(suiteName,testName,63,testDescription);

        puma::Matrix<short> m1;

        bool success = m1.normalize(0,255);

        assertEquals(false, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((long) 0, m1.size() , &result);
        if(result.failed) {
            return result;
        }


        return result;
    }

    static TestResult test64() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize Test 2";
        std::string testDescription = "Empty Matrix - float";
        TestResult result(suiteName,testName,64,testDescription);

        puma::Matrix<float> m1;

        bool success = m1.normalize(0,255);

        assertEquals(false, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((long) 0, m1.size() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test65() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize Test 3";
        std::string testDescription = "Normalize to min,max - short";
        TestResult result(suiteName,testName,65,testDescription);

        puma::Matrix<short> m1(100,100,100,-5);
        m1.set(0,49,0,99,0,99,-10);

        double avg1 = m1.average();
        double min = m1.min();
        double max = m1.max();

        bool success = m1.normalize(min,max);

        double avg2 = m1.average();

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals(avg1, avg2 , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test66() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize Test 4";
        std::string testDescription = "Normalize to min,max - float";
        TestResult result(suiteName,testName,66,testDescription);

        puma::Matrix<float> m1(100,100,100,-5);
        m1.set(0,49,0,99,0,99,-5.5);

        double avg1 = m1.average();
        double min = m1.min();
        double max = m1.max();

        bool success = m1.normalize(min,max);

        double avg2 = m1.average();

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals(avg1, avg2 , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test67() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize Test 5";
        std::string testDescription = "Normalize to 8-bit - short";
        TestResult result(suiteName,testName,67,testDescription);

        puma::Matrix<short> m1(100,100,100,-5);
        m1.set(0,49,0,99,0,99,-10);

        bool success = m1.normalize(0,255);

        double min = m1.min();
        double max = m1.max();


        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)0, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)255, max , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test68() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize Test 6";
        std::string testDescription = "Normalize to 8-bit - float";
        TestResult result(suiteName,testName,68,testDescription);

        puma::Matrix<float> m1(100,100,100,-5);
        m1.set(0,49,0,99,0,99,-5.5);

        bool success = m1.normalize(0,255);

        double min = m1.min();
        double max = m1.max();


        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)0, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)255, max , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test69() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize Test 6";
        std::string testDescription = "Normalize to 8-bit - short > 2.1b";
        TestResult result(suiteName,testName,69,testDescription);

        puma::Matrix<short> m1(1300,1300,1300,-5);
        m1.set(0,49,0,99,0,99,-10);

        bool success = m1.normalize(0,255);

        double min = m1.min();
        double max = m1.max();


        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)0, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)255, max , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test70() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize Test 8";
        std::string testDescription = "Normalize to 8-bit - float >2.1b";
        TestResult result(suiteName,testName,70,testDescription);

        puma::Matrix<float> m1(1300,1300,1300,-5);
        m1.set(0,49,0,99,0,99,-5.5);

        bool success = m1.normalize(0,255);

        double min = m1.min();
        double max = m1.max();


        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)0, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)255, max , &result);
        if(result.failed) {
            return result;
        }


        return result;
    }

    static TestResult test71() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize (other) Test 1";
        std::string testDescription = "Empty Matrix - short";
        TestResult result(suiteName,testName,71,testDescription);

        puma::Matrix<short> m1;
        puma::Matrix<float> m2;

        bool success = m1.normalize(&m2,0,255);

        assertEquals(false, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((long) 0, m2.size() , &result);
        if(result.failed) {
            return result;
        }


        return result;
    }

    static TestResult test72() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize(other) Test 2";
        std::string testDescription = "Empty Matrix - float";
        TestResult result(suiteName,testName,72,testDescription);

        puma::Matrix<float> m1;
        puma::Matrix<float> m2;

        bool success = m1.normalize(&m2,0,255);

        assertEquals(false, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((long) 0, m2.size() , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test73() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize(other) Test 3";
        std::string testDescription = "Normalize to min,max - short";
        TestResult result(suiteName,testName,73,testDescription);

        puma::Matrix<short> m1(100,100,100,-5);
        m1.set(0,49,0,99,0,99,-10);

        puma::Matrix<float> m2;

        double avg1 = m1.average();
        double min = m1.min();
        double max = m1.max();

        bool success = m1.normalize(&m2, min,max);

        double avg2 = m2.average();

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals(avg1, avg2 , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test74() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize(other) Test 4";
        std::string testDescription = "Normalize to min,max - float";
        TestResult result(suiteName,testName,74,testDescription);

        puma::Matrix<float> m1(100,100,100,-5);
        m1.set(0,49,0,99,0,99,-5.5);

        puma::Matrix<float> m2;

        double avg1 = m1.average();
        double min = m1.min();
        double max = m1.max();

        bool success = m1.normalize(&m2,min,max);

        double avg2 = m2.average();

        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals(avg1, avg2 , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test75() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize(other) Test 5";
        std::string testDescription = "Normalize to 8-bit - short";
        TestResult result(suiteName,testName,75,testDescription);

        puma::Matrix<short> m1(100,100,100,-5);
        m1.set(0,49,0,99,0,99,-10);

        puma::Matrix<float> m2;

        bool success = m1.normalize(&m2,0,255);

        double min = m2.min();
        double max = m2.max();


        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)0, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)255, max , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test76() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize(other) Test 6";
        std::string testDescription = "Normalize to 8-bit - float";
        TestResult result(suiteName,testName,76,testDescription);

        puma::Matrix<float> m1(100,100,100,-5);
        m1.set(0,49,0,99,0,99,-5.5);

        puma::Matrix<float> m2;

        bool success = m1.normalize(&m2,0,255);

        double min = m2.min();
        double max = m2.max();


        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)0, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)255, max , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test77() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize(other) Test 6";
        std::string testDescription = "Normalize to 8-bit - short > 2.1b";
        TestResult result(suiteName,testName,77,testDescription);

        puma::Matrix<short> m1(1300,1300,1300,-5);
        m1.set(0,49,0,99,0,99,-10);

        puma::Matrix<float> m2;

        bool success = m1.normalize(&m2,0,255);

        double min = m2.min();
        double max = m2.max();


        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)0, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)255, max , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test78() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Matrix Normalize(other) Test 8";
        std::string testDescription = "Normalize to 8-bit - float >2.1b";
        TestResult result(suiteName,testName,78,testDescription);

        puma::Matrix<float> m1(1300,1300,1300,-5);
        m1.set(0,49,0,99,0,99,-5.5);

        puma::Matrix<float> m2;

        bool success = m1.normalize(&m2,0,255);

        double min = m2.min();
        double max = m2.max();


        assertEquals(true, success , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)0, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)255, max , &result);
        if(result.failed) {
            return result;
        }


        return result;
    }

    static TestResult test79() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Min & Max Test 1";
        std::string testDescription = "Empty Matrix - short";
        TestResult result(suiteName,testName,79,testDescription);

        puma::Matrix<short> m1;

        short min = m1.min();
        short max = m1.max();

        assertEquals((short)0, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((short)0, max , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test80() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Min & Max Test 2";
        std::string testDescription = "Empty Matrix - float";
        TestResult result(suiteName,testName,80,testDescription);

        puma::Matrix<float> m1;

        double min = m1.min();
        double max = m1.max();

        assertEquals((double)0, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((double)0, max , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test81() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Min & Max Test 3";
        std::string testDescription = "Positive Min - short";
        TestResult result(suiteName,testName,81,testDescription);

        puma::Matrix<short> m1(100,100,100,5);
        m1.set(0,49,0,99,0,99,6);

        short min = m1.min();
        short max = m1.max();

        assertEquals((short)5, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((short)6, max , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test82() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Min & Max Test 4";
        std::string testDescription = "Positive Min - float";
        TestResult result(suiteName,testName,82,testDescription);

        puma::Matrix<float> m1(100,100,100,4.2);
        m1.set(0,49,0,99,0,99,4.3);

        float min = m1.min();
        float max = m1.max();

        assertEquals((float)4.2, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((float)4.3, max , &result);
        if(result.failed) {
            return result;
        }


        return result;
    }

    static TestResult test83() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Min & Max Test 5";
        std::string testDescription = "Negative Min - short";
        TestResult result(suiteName,testName,83,testDescription);

        puma::Matrix<short> m1(100,100,100,-5);
        m1.set(0,49,0,99,0,99,-6);

        short min = m1.min();
        short max = m1.max();

        assertEquals((short)-6, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((short)-5, max , &result);
        if(result.failed) {
            return result;
        }

        return result;

    }

    static TestResult test84() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Min & Max Test 6";
        std::string testDescription = "Negative Min - float";
        TestResult result(suiteName,testName,84,testDescription);

        puma::Matrix<float> m1(100,100,100,-4.2);
        m1.set(0,49,0,99,0,99,-4.3);

        float min = m1.min();
        float max = m1.max();

        assertEquals((float)-4.3, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((float)-4.2, max , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test85() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Min & Max Test 7";
        std::string testDescription = "Constant Value - short";
        TestResult result(suiteName,testName,85,testDescription);

        puma::Matrix<short> m1(100,100,100,-5);

        short min = m1.min();
        short max = m1.max();

        assertEquals((short)-5, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((short)-5, max , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test86() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Min & Max Test 8";
        std::string testDescription = "Constant Value - float";
        TestResult result(suiteName,testName,86,testDescription);

        puma::Matrix<float> m1(100,100,100,-4.2);

        float min = m1.min();
        float max = m1.max();

        assertEquals((float)-4.2, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((float)-4.2, max , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test87() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Min & Max Test 9";
        std::string testDescription = "Negative Min - short >2.1b";
        TestResult result(suiteName,testName,87,testDescription);

        puma::Matrix<short> m1(1300,1300,1300,-5);
        m1.set(0,49,0,99,0,99,-6);

        short min = m1.min();
        short max = m1.max();

        assertEquals((short)-6, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((short)-5, max , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

    static TestResult test88() {
        std::string suiteName = "Matrix Tests: ";
        std::string testName = "Min & Max Test 10";
        std::string testDescription = "Negative Min - float >2.1b";
        TestResult result(suiteName,testName,88,testDescription);

        puma::Matrix<float> m1(1300,1300,1300,-4.2);
        m1.set(0,49,0,99,0,99,-4.3);

        float min = m1.min();
        float max = m1.max();

        assertEquals((float)-4.3, min , &result);
        if(result.failed) {
            return result;
        }

        assertEquals((float)-4.2, max , &result);
        if(result.failed) {
            return result;
        }

        return result;
    }

};
