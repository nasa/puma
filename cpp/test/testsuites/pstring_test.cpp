#include "../testframework/subtest.h"
#include "puma.h"

#include <string>
#include <vector>


class PString_Test : public SubTest {
public:

    PString_Test() {
        testSuiteName = "PString_Test";

        tests.push_back(basicIntToString);
        tests.push_back(basicNegativeIntToString);
        tests.push_back(basicDoubleToString);
        tests.push_back(basicInt0ToString);
        tests.push_back(basicDouble0ToString);
        tests.push_back(basicNegativeDoubleToString);


        tests.push_back(basicStringToInt);
        tests.push_back(basicNegativeStringToInt);
        tests.push_back(maxStringToInt);
        tests.push_back(minStringToInt);
        tests.push_back(invalidStringToInt1);
        tests.push_back(invalidStringToInt2);
        tests.push_back(invalidStringToInt3);
        tests.push_back(multipleStringToInt);
        tests.push_back(stringToMaxInt);
        tests.push_back(stringToMinInt);
        tests.push_back(stringToOverMaxInt);
        tests.push_back(stringToSmallerMinInt);


        tests.push_back(basicStringToIntWithReference);
        tests.push_back(basicNegativeStringToIntWithReference);
        tests.push_back(maxStringToIntWithReference);
        tests.push_back(minStringToIntWithReference);
        tests.push_back(invalidStringToInt1WithReference);
        tests.push_back(invalidStringToInt2WithReference);
        tests.push_back(invalidStringToInt3WithReference);
        tests.push_back(multipleStringToIntWithReference);
        tests.push_back(stringToMaxIntWithReference);
        tests.push_back(stringToMinIntWithReference);
        tests.push_back(stringToOverMaxIntWithReference);
        tests.push_back(stringToSmallerMinIntWithReference);

        tests.push_back(basicStringToShort);
        tests.push_back(basicNegativeStringToShort);
        tests.push_back(maxStringToShort);
        tests.push_back(minStringToShort);
        tests.push_back(invalidStringToShort1);
        tests.push_back(invalidStringToShort2);
        tests.push_back(invalidStringToShort3);
        tests.push_back(multipleStringToShort);
        tests.push_back(stringToMaxShort);
        tests.push_back(stringToMinShort);
        tests.push_back(stringToOverMaxShort);
        tests.push_back(stringToSmallerMinShort);


        tests.push_back(basicStringToShortWithReference);
        tests.push_back(basicNegativeStringToShortWithReference);
        tests.push_back(maxStringToShortWithReference);
        tests.push_back(minStringToShortWithReference);
        tests.push_back(invalidStringToShort1WithReference);
        tests.push_back(invalidStringToShort2WithReference);
        tests.push_back(invalidStringToShort3WithReference);
        tests.push_back(multipleStringToShortWithReference);
        tests.push_back(stringToMaxShortWithReference);
        tests.push_back(stringToMinShortWithReference);
        tests.push_back(stringToOverMaxShortWithReference);
        tests.push_back(stringToSmallerMinShortWithReference);

        tests.push_back(basicStringToLong);
        tests.push_back(basicNegativeStringToLong);
        tests.push_back(invalidStringToLong1);
        tests.push_back(invalidStringToLong2);
        tests.push_back(invalidStringToLong3);
        tests.push_back(multipleStringToLong);


        tests.push_back(basicStringToLongWithReference);
        tests.push_back(basicNegativeStringToLongWithReference);
        tests.push_back(invalidStringToLong1WithReference);
        tests.push_back(invalidStringToLong2WithReference);
        tests.push_back(invalidStringToLong3WithReference);
        tests.push_back(multipleStringToLongWithReference);

        tests.push_back(splitStringEmpty);
        tests.push_back(splitStringEmptyWithDelimiter);
        tests.push_back(splitStringSmallWithDelimiter1);
        tests.push_back(splitStringSmallWithDelimiter2);
        tests.push_back(splitStringSmallWithDelimiter3);
        tests.push_back(splitStringSmallWithDelimiter4);
        tests.push_back(splitStringSmallWithDelimiter5);
        tests.push_back(splitStringSmallWithDelimiter6);
        tests.push_back(splitStringSmallWithDelimiter7);
        tests.push_back(splitStringSmallWithDelimiter8);

        tests.push_back(splitStringUniqueEmpty);
        tests.push_back(splitStringUniqueEmptyWithDelimiter);
        tests.push_back(splitStringUniqueSmallWithDelimiter1);
        tests.push_back(splitStringUniqueSmallWithDelimiter2);
        tests.push_back(splitStringUniqueSmallWithDelimiter3);
        tests.push_back(splitStringUniqueSmallWithDelimiter4);
        tests.push_back(splitStringUniqueSmallWithDelimiter5);
        tests.push_back(splitStringUniqueSmallWithDelimiter6);
        tests.push_back(splitStringUniqueSmallWithDelimiter7);
        tests.push_back(splitStringUniqueSmallWithDelimiter8);

        tests.push_back(trimNewLineBack);
        tests.push_back(trimNewLineFront);
        tests.push_back(trimNewLineFrontAndBack);
        tests.push_back(trimNewLineMiddle);
        tests.push_back(trimTabBack);
        tests.push_back(trimTabFront);
        tests.push_back(trimTabFrontAndBack);
        tests.push_back(trimTabMiddle);
        tests.push_back(trimReturnBack);
        tests.push_back(trimReturnFront);
        tests.push_back(trimReturnFrontAndBack);
        tests.push_back(trimReturnMiddle);
        tests.push_back(trimSpaceBack);
        tests.push_back(trimSpaceFront);
        tests.push_back(trimSpaceFrontAndBack);
        tests.push_back(trimSpaceMiddle);
        tests.push_back(trimMultiple1);
        tests.push_back(trimMultiple2);
        tests.push_back(trimSimple);

        tests.push_back(replaceFirstSimple);
        tests.push_back(replaceSingleLetterMultipleOccurence);
        tests.push_back(replaceMultipleLettersSingleOccurence);
        tests.push_back(replaceMultipleLettersSingleOccurence);
        tests.push_back(replaceSingleLetterSingleOccurences);
        tests.push_back(replaceWord);
        tests.push_back(replaceExtraOccurence);
        tests.push_back(replaceTooLarge);

        tests.push_back(replaceAllSimple);
        tests.push_back(replaceAllSingleLetterMultipleOccurence);
        tests.push_back(replaceAllMultipleLettersSingleOccurence);
        tests.push_back(replaceAllSingleLetterSingleOccurences);
        tests.push_back(replaceAllWord);
        tests.push_back(replaceAllEntireString);
        tests.push_back(replaceAllTooLarge);
        tests.push_back(replaceAllMultipleWords);
        tests.push_back(replaceAllAvoidInfiniteLoop);
        tests.push_back(replaceAllAvoidInfiniteLoopWithSmallerSize);
        tests.push_back(replaceAllAvoidInfiniteLoopWithSmallerSizeUneven);

        tests.push_back(containsSingleLetter);
        tests.push_back(containsSingleWord);
        tests.push_back(containsSingleLetterInWord);
        tests.push_back(containsEmptyString);
        tests.push_back(containsTooLargeString);
        tests.push_back(containsMultipleWords);

        tests.push_back(StartsWithSingleLetter);
        tests.push_back(StartsWithSingleWord);
        tests.push_back(StartsWithSingleLetterInWord);
        tests.push_back(StartsWithEmptyString);
        tests.push_back(StartsWithTooLargeString);
        tests.push_back(StartsWithMultipleWords);
        tests.push_back(StartsWithMultipleWordsSingleLetter);
        tests.push_back(StartsWithMultipleWordsFail);

        tests.push_back(endsWithSingleLetter);
        tests.push_back(endsWithSingleWord);
        tests.push_back(endsWithSingleLetterInWord);
        tests.push_back(endsWithEmptyString);
        tests.push_back(endsWithTooLargeString);
        tests.push_back(endsWithMultipleWords);
        tests.push_back(endsWithMultipleWordsSingleLetter);
        tests.push_back(endsWithMultipleWordsFail);
    }

    static TestResult basicIntToString() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicIntToString - test checks 10 is converted to a string correctly";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        int number = 10;
        std::string output = puma::PString::fromInt(number);
        if(!assertEquals("10",output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult basicInt0ToString() {

        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicInt0ToString - test checks 0 is converted to a string correctly";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        int number = 0;
        std::string output = puma::PString::fromInt(number);
        if(!assertEquals("0",output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult basicNegativeIntToString() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicNegativeIntToString - test checks -10 is converted to a string correctly";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        int number = -10;
        std::string output = puma::PString::fromInt(number);
        if(!assertEquals("-10",output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult basicDoubleToString() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicDoubleToString - test checks 10.5 is converted to a string correctly";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        double number = 10.5;
        std::string output = puma::PString::fromDouble(number);
        if(!assertEquals("10.500000",output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult basicDouble0ToString() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicDoubleToString - test checks 0 is converted to a string correctly";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        double number = 0;
        std::string output = puma::PString::fromDouble(number);
        if(!assertEquals("0.000000",output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult basicNegativeDoubleToString() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicDoubleToString - test checks -10.5 is converted to a string correctly";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        double number = -10.5;
        std::string output = puma::PString::fromDouble(number);
        if(!assertEquals("-10.500000",output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult basicStringToInt() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicStringToInt - parse 10 from string to int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "10";
        int output = puma::PString::toInt(number);
        if(!assertEquals(10,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult basicNegativeStringToInt() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicNegativeStringToInt - parse -10 from string to int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "-10";
        int output = puma::PString::toInt(number);
        if(!assertEquals(-10,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult maxStringToInt() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: maxStringToInt - parse max int from string to int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "2147483647";
        int output = puma::PString::toInt(number);
        if(!assertEquals(2147483647,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult minStringToInt() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: minStringToInt - parse min int from string to int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "-2147483648";
        int minInt = -2147483648;
        int output = puma::PString::toInt(number);
        if(!assertEquals(minInt,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToInt1() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToInt1 - make sure toInt can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "bobby";
        int output = puma::PString::toInt(number);
        if(!assertEquals(0,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToInt2() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToInt2 - make sure toInt can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "a10.0";
        int output = puma::PString::toInt(number);
        if(!assertEquals(0,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToInt3() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToInt3 - make sure toInt can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "0.10";
        int output = puma::PString::toInt(number);
        if(!assertEquals(0,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult multipleStringToInt() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToInt - make sure toInt can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::vector<std::string> inputs(11);
        std::vector<int> expected_outputs(11);

        inputs.push_back("-10");
        inputs.push_back("-8");
        inputs.push_back("-6");
        inputs.push_back("-4");
        inputs.push_back("-2");
        inputs.push_back("0");
        inputs.push_back("2");
        inputs.push_back("4");
        inputs.push_back("6");
        inputs.push_back("8");
        inputs.push_back("10");

        expected_outputs.push_back(-10);
        expected_outputs.push_back(-8);
        expected_outputs.push_back(-6);
        expected_outputs.push_back(-4);
        expected_outputs.push_back(-2);
        expected_outputs.push_back(0);
        expected_outputs.push_back(2);
        expected_outputs.push_back(4);
        expected_outputs.push_back(6);
        expected_outputs.push_back(8);
        expected_outputs.push_back(10);

        for(unsigned i=0;i<inputs.size(); i++)
        {
            if(!assertEquals(expected_outputs[i],puma::PString::toInt(inputs[i]),&result))
            {
                return result;
            }
        }

        return result;
    }

    static TestResult stringToMaxInt() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: multipleStringToMaxInt - make sure toInt can handle large int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        if(!assertEquals(2147483647,puma::PString::toInt("2147483647"),&result))
        {
            return result;
        }

        return result;
    }

    static TestResult stringToMinInt() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: multipleStringToMaxInt - make sure toInt can handle small int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        int smallestInt = -2147483648;
        if(!assertEquals(smallestInt,puma::PString::toInt("-2147483648"),&result))
        {
            return result;
        }


        return result;
    }

    static TestResult stringToOverMaxInt() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: multipleStringToMaxInt - make sure toInt can handle large int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);


        if(!assertEquals(0,puma::PString::toInt("2147483648"),&result))
        {
            return result;
        }


        return result;
    }

    static TestResult stringToSmallerMinInt() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: stringToSmallerMinInt - make sure toInt can handle small int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);


        if(!assertEquals(0,puma::PString::toInt("-2147483649"),&result))
        {
            return result;
        }


        return result;
    }

    static TestResult basicStringToIntWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicStringToIntWithReference - parse 10 from string to int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "10";
        int valueOut=-1;
        bool output = puma::PString::toInt(number,valueOut);
        if(!assertEquals(true,output,&result)||!assertEquals(10,valueOut,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult basicNegativeStringToIntWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicNegativeStringToIntWithReference - parse -10 from string to int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "-10";
        int valueOut = -1;
        bool output = puma::PString::toInt(number,valueOut);
        if(!assertEquals(-10,valueOut,&result) || !assertEquals(true,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult maxStringToIntWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: maxStringToIntWithReference - parse max int from string to int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "2147483647";
        int valueOut=-1;
        bool output = puma::PString::toInt(number, valueOut);
        if(!assertEquals(2147483647,valueOut,&result) || !assertEquals(true,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult minStringToIntWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: minStringToIntWithReference - parse min int from string to int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "-2147483648";
        int minInt = -2147483648;
        int valueOut=-1;
        bool output = puma::PString::toInt(number, valueOut);
        if(!assertEquals(minInt,valueOut,&result) || !assertEquals(true,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToInt1WithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToIntWithReference1 - make sure toInt can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "bobby";
        int valueOut;
        bool output = puma::PString::toInt(number, valueOut);
        if(!assertEquals(false,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToInt2WithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToIntWithReference2 - make sure toInt can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "a10.0";
        int valueOut;
        bool output = puma::PString::toInt(number,valueOut);
        if(!assertEquals(false,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToInt3WithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToIntWithReference3 - make sure toInt can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "a0.10";
        int valueOut =-1;
        bool output = puma::PString::toInt(number, valueOut);
        if(!assertEquals(-1,valueOut,&result) || !assertEquals(false,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult multipleStringToIntWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: multipleStringToIntWithReference - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::vector<std::string> inputs;
        std::vector<int> expected_outputs;

        inputs.push_back("-10");
        inputs.push_back("-8");
        inputs.push_back("-6");
        inputs.push_back("-4");
        inputs.push_back("-2");
        inputs.push_back("0");
        inputs.push_back("2");
        inputs.push_back("4");
        inputs.push_back("6");
        inputs.push_back("8");
        inputs.push_back("10");

        expected_outputs.push_back(-10);
        expected_outputs.push_back(-8);
        expected_outputs.push_back(-6);
        expected_outputs.push_back(-4);
        expected_outputs.push_back(-2);
        expected_outputs.push_back(0);
        expected_outputs.push_back(2);
        expected_outputs.push_back(4);
        expected_outputs.push_back(6);
        expected_outputs.push_back(8);
        expected_outputs.push_back(10);

        for(unsigned i=0;i<inputs.size(); i++)
        {
            int valueOut =-1;
            bool output = puma::PString::toInt(inputs[i],valueOut);
            if(!assertEquals(expected_outputs[i],valueOut,&result) || !assertEquals(true,output,&result))
            {
                return result;
            }
        }

        return result;
    }

    static TestResult stringToMaxIntWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: stringToMaxIntWithReference - make sure toInt can handle large int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        int valueOut=-1;
        bool output = puma::PString::toInt("2147483647", valueOut);

        if(!assertEquals(2147483647,valueOut,&result) || !assertEquals(true,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult stringToMinIntWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: stringToMinIntWithReference - make sure toInt can handle small int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        int smallestInt = -2147483648;

        int valueOut =1;
        bool output = puma::PString::toInt("-2147483648", valueOut);
        if(!assertEquals(smallestInt,valueOut,&result) || !assertEquals(true,output,&result))
        {
            return result;
        }


        return result;
    }

    static TestResult stringToOverMaxIntWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: stringToOverMaxIntWithReference - make sure toInt can handle large int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        int valueOut;
        bool output = puma::PString::toInt("2147483648",valueOut);

        if(!assertEquals(false,output,&result))
        {
            return result;
        }


        return result;
    }

    static TestResult stringToSmallerMinIntWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: stringToSmallerMinIntWithReference - make sure toInt can handle small int";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);


        int valueOut;
        bool output = puma::PString::toInt("-2147483649", valueOut);
        if(!assertEquals(false,output,&result))
        {
            return result;
        }


        return result;
    }
    /*Testing Shorts ********************************************************/
    static TestResult basicStringToShort() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicStringToShort - parse 10 from string to short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "10";
        short output = puma::PString::toShort(number);
        if(!assertEquals((short)10,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult basicNegativeStringToShort() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicNegativeStringToShort - parse -10 from string to short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "-10";
        short output = puma::PString::toShort(number);
        if(!assertEquals((short)-10,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult maxStringToShort() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: maxStringToShort - parse max Short from string to short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "32767";
        short output = puma::PString::toShort(number);
        if(!assertEquals((short)32767,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult minStringToShort() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: minStringToShort - parse min short from string to short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "-32768";
        short minShort = -32768;
        short output = puma::PString::toShort(number);
        if(!assertEquals(minShort,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToShort1() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToShort1 - make sure toShort can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "bobby";
        short output = puma::PString::toShort(number);
        if(!assertEquals((short)0,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToShort2() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToShort2 - make sure toshort can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "a10.0";
        short output = puma::PString::toShort(number);
        if(!assertEquals((short)0,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToShort3() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToShort3 - make sure toshort can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "0.10";
        short output = puma::PString::toShort(number);
        if(!assertEquals((short)0,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult multipleStringToShort() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToshort - make sure toshort can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::vector<std::string> inputs(11);
        std::vector<short> expected_outputs(11);

        inputs.push_back("-10");
        inputs.push_back("-8");
        inputs.push_back("-6");
        inputs.push_back("-4");
        inputs.push_back("-2");
        inputs.push_back("0");
        inputs.push_back("2");
        inputs.push_back("4");
        inputs.push_back("6");
        inputs.push_back("8");
        inputs.push_back("10");

        expected_outputs.push_back(-10);
        expected_outputs.push_back(-8);
        expected_outputs.push_back(-6);
        expected_outputs.push_back(-4);
        expected_outputs.push_back(-2);
        expected_outputs.push_back(0);
        expected_outputs.push_back(2);
        expected_outputs.push_back(4);
        expected_outputs.push_back(6);
        expected_outputs.push_back(8);
        expected_outputs.push_back(10);

        for(unsigned i=0;i<inputs.size(); i++)
        {
            if(!assertEquals(expected_outputs[i],puma::PString::toShort(inputs[i]),&result))
            {
                return result;
            }
        }

        return result;
    }

    static TestResult stringToMaxShort() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: stringToMaxShort - make sure toShort can handle large short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        if(!assertEquals((short)32767,puma::PString::toShort("32767"),&result))
        {
            return result;
        }

        return result;
    }

    static TestResult stringToMinShort() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: stringToMinShort - make sure toShort can handle small short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        short smallestShort = -32768;
        if(!assertEquals(smallestShort,puma::PString::toShort("-32768"),&result))
        {
            return result;
        }


        return result;
    }

    static TestResult stringToOverMaxShort() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: stringToOverMaxShort - make sure toShort can handle large short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);


        if(!assertEquals((short)0,puma::PString::toShort("32768"),&result))
        {
            return result;
        }


        return result;
    }

    static TestResult stringToSmallerMinShort() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: stringToSmallerMinShort - make sure toshort can handle small short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);


        if(!assertEquals((short)0,puma::PString::toShort("-32769"),&result))
        {
            return result;
        }


        return result;
    }

    static TestResult basicStringToShortWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicStringToShortWithReference - parse 10 from string to short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "10";
        short valueOut=-1;
        bool output = puma::PString::toShort(number,valueOut);
        if(!assertEquals(true,output,&result)||!assertEquals((short)10,valueOut,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult basicNegativeStringToShortWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicNegativeStringToShortWithReference - parse -10 from string to short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "-10";
        short valueOut = -1;
        bool output = puma::PString::toShort(number,valueOut);
        if(!assertEquals((short)-10,valueOut,&result) || !assertEquals(true,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult maxStringToShortWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: maxStringToShortWithReference - parse max short from string to short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "32767";
        short valueOut=-1;
        bool output = puma::PString::toShort(number, valueOut);
        if(!assertEquals((short)32767,valueOut,&result) || !assertEquals(true,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult minStringToShortWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: minStringToShortWithReference - parse min short from string to short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "-32768";
        short minShort = -32768;
        short valueOut=-1;
        bool output = puma::PString::toShort(number, valueOut);
        if(!assertEquals(minShort,valueOut,&result) || !assertEquals(true,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToShort1WithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToShortWithReference1 - make sure toShort can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "bobby";
        short valueOut;
        bool output = puma::PString::toShort(number, valueOut);
        if(!assertEquals(false,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToShort2WithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToShortWithReference2 - make sure toShort can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "a10.0";
        short valueOut;
        bool output = puma::PString::toShort(number,valueOut);
        if(!assertEquals(false,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToShort3WithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToShortWithReference3 - make sure toShort can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "a0.10";
        short valueOut =-1;
        bool output = puma::PString::toShort(number, valueOut);
        if(!assertEquals((short)-1,valueOut,&result) || !assertEquals(false,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult multipleStringToShortWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: multipleStringToShortWithReference - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::vector<std::string> inputs;
        std::vector<short> expected_outputs;

        inputs.push_back("-10");
        inputs.push_back("-8");
        inputs.push_back("-6");
        inputs.push_back("-4");
        inputs.push_back("-2");
        inputs.push_back("0");
        inputs.push_back("2");
        inputs.push_back("4");
        inputs.push_back("6");
        inputs.push_back("8");
        inputs.push_back("10");

        expected_outputs.push_back(-10);
        expected_outputs.push_back(-8);
        expected_outputs.push_back(-6);
        expected_outputs.push_back(-4);
        expected_outputs.push_back(-2);
        expected_outputs.push_back(0);
        expected_outputs.push_back(2);
        expected_outputs.push_back(4);
        expected_outputs.push_back(6);
        expected_outputs.push_back(8);
        expected_outputs.push_back(10);

        for(unsigned i=0;i<inputs.size(); i++)
        {
            short valueOut =-1;
            bool output = puma::PString::toShort(inputs[i],valueOut);
            if(!assertEquals(expected_outputs[i],valueOut,&result) || !assertEquals(true,output,&result))
            {
                return result;
            }
        }

        return result;
    }

    static TestResult stringToMaxShortWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: stringToMaxShortWithReference - make sure toShort can handle large short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        short valueOut=-1;
        bool output = puma::PString::toShort("32767", valueOut);

        if(!assertEquals((short)32767,valueOut,&result) || !assertEquals(true,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult stringToMinShortWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: stringToMinShortWithReference - make sure toShort can handle small short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        short smallestShort = -32768;

        short valueOut =1;
        bool output = puma::PString::toShort("-32768", valueOut);
        if(!assertEquals(smallestShort,valueOut,&result) || !assertEquals(true,output,&result))
        {
            return result;
        }


        return result;
    }

    static TestResult stringToOverMaxShortWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: stringToOverMaxShortWithReference - make sure toShort can handle large short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        short valueOut;
        bool output = puma::PString::toShort("32768",valueOut);

        if(!assertEquals(false,output,&result))
        {
            return result;
        }


        return result;
    }

    static TestResult stringToSmallerMinShortWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: stringToSmallerMinShortWithReference - make sure toshort can handle small short";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);


        short valueOut;
        bool output = puma::PString::toShort("-32769", valueOut);
        if(!assertEquals(false,output,&result))
        {
            return result;
        }


        return result;
    }

    /*Testing Longs ********************************************************/
    static TestResult basicStringToLong() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicStringToLong - parse 10 from string to Long";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "10";
        long output = puma::PString::toLong(number);
        if(!assertEquals((long)10,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult basicNegativeStringToLong() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicNegativeStringToLong - parse -10 from string to Long";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "-10";
        long output = puma::PString::toLong(number);
        if(!assertEquals((long)-10,output,&result))
        {
            return result;
        }

        return result;
    }


    static TestResult invalidStringToLong1() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToLong1 - make sure toLong can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "bobby";
        long output = puma::PString::toLong(number);
        if(!assertEquals((long)0,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToLong2() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToLong2 - make sure toLong can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "a10.0";
        long output = puma::PString::toLong(number);
        if(!assertEquals((long)0,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToLong3() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToLong3 - make sure toLong can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "0.10";
        long output = puma::PString::toLong(number);
        if(!assertEquals((long)0,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult multipleStringToLong() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToLong - make sure toLong can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::vector<std::string> inputs(11);
        std::vector<long> expected_outputs(11);

        inputs.push_back("-10");
        inputs.push_back("-8");
        inputs.push_back("-6");
        inputs.push_back("-4");
        inputs.push_back("-2");
        inputs.push_back("0");
        inputs.push_back("2");
        inputs.push_back("4");
        inputs.push_back("6");
        inputs.push_back("8");
        inputs.push_back("10");

        expected_outputs.push_back(-10);
        expected_outputs.push_back(-8);
        expected_outputs.push_back(-6);
        expected_outputs.push_back(-4);
        expected_outputs.push_back(-2);
        expected_outputs.push_back(0);
        expected_outputs.push_back(2);
        expected_outputs.push_back(4);
        expected_outputs.push_back(6);
        expected_outputs.push_back(8);
        expected_outputs.push_back(10);

        for(unsigned i=0;i<inputs.size(); i++)
        {
            if(!assertEquals(expected_outputs[i],puma::PString::toLong(inputs[i]),&result))
            {
                return result;
            }
        }

        return result;
    }

    static TestResult basicStringToLongWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicStringToLongWithReference - parse 10 from string to Long";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "10";
        long valueOut=-1;
        bool output = puma::PString::toLong(number,valueOut);
        if(!assertEquals(true,output,&result)||!assertEquals((long)10,valueOut,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult basicNegativeStringToLongWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: basicNegativeStringToLongWithReference - parse -10 from string to Long";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "-10";
        long valueOut = -1;
        bool output = puma::PString::toLong(number,valueOut);
        if(!assertEquals((long)-10,valueOut,&result) || !assertEquals(true,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToLong1WithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToLongWithReference1 - make sure toLong can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "bobby";
        long valueOut;
        bool output = puma::PString::toLong(number, valueOut);
        if(!assertEquals(false,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToLong2WithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToLongWithReference2 - make sure toLong can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "a10.0";
        long valueOut;
        bool output = puma::PString::toLong(number,valueOut);
        if(!assertEquals(false,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult invalidStringToLong3WithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: invalidStringToLongWithReference3 - make sure toLong can handle invalid input";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string number = "a0.10";
        long valueOut =-1;
        bool output = puma::PString::toLong(number, valueOut);
        if(!assertEquals((long)-1,valueOut,&result) || !assertEquals(false,output,&result))
        {
            return result;
        }

        return result;
    }

    static TestResult multipleStringToLongWithReference() {



        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: multipleStringToLongWithReference - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::vector<std::string> inputs;
        std::vector<long> expected_outputs;

        inputs.push_back("-10");
        inputs.push_back("-8");
        inputs.push_back("-6");
        inputs.push_back("-4");
        inputs.push_back("-2");
        inputs.push_back("0");
        inputs.push_back("2");
        inputs.push_back("4");
        inputs.push_back("6");
        inputs.push_back("8");
        inputs.push_back("10");

        expected_outputs.push_back(-10);
        expected_outputs.push_back(-8);
        expected_outputs.push_back(-6);
        expected_outputs.push_back(-4);
        expected_outputs.push_back(-2);
        expected_outputs.push_back(0);
        expected_outputs.push_back(2);
        expected_outputs.push_back(4);
        expected_outputs.push_back(6);
        expected_outputs.push_back(8);
        expected_outputs.push_back(10);

        for(unsigned i=0;i<inputs.size(); i++)
        {
            long valueOut =-1;
            bool output = puma::PString::toLong(inputs[i],valueOut);
            if(!assertEquals(expected_outputs[i],valueOut,&result) || !assertEquals(true,output,&result))
            {
                return result;
            }
        }

        return result;
    }

    /*split String ********************************************************/

    static TestResult splitStringEmpty()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: spliStringEmpty - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "";

        std::vector<std::string> output = puma::PString::splitString(input,"");

        if(!assertEquals(1,(int)output.size(), &result) || !assertEquals("",output[0],&result))
        {
            return result;
        }

        return result;

    }

    static TestResult splitStringEmptyWithDelimiter()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: spliStringEmptyWithDelimiter - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "";

        std::vector<std::string> output = puma::PString::splitString(input,"abc");

        if(!assertEquals(1,(int)output.size(), &result) || !assertEquals("",output[0],&result))
        {
            return result;
        }

        return result;

    }

    static TestResult splitStringSmallWithDelimiter1()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: spliStringSmallWithDelimiter1 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "ababab";
        std::vector<std::string> expected_output;
        expected_output.push_back("");
        expected_output.push_back("b");
        expected_output.push_back("b");
        expected_output.push_back("b");

        std::vector<std::string> output = puma::PString::splitString(input,"a");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    static TestResult splitStringSmallWithDelimiter2()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: spliStringSmallWithDelimiter2 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "bababab";
        std::vector<std::string> expected_output;
        expected_output.push_back("b");
        expected_output.push_back("b");
        expected_output.push_back("b");
        expected_output.push_back("b");

        std::vector<std::string> output = puma::PString::splitString(input,"a");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    static TestResult splitStringSmallWithDelimiter3()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: spliStringSmallWithDelimiter3 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "babbababb";
        std::vector<std::string> expected_output;
        expected_output.push_back("b");
        expected_output.push_back("bb");
        expected_output.push_back("b");
        expected_output.push_back("bb");

        std::vector<std::string> output = puma::PString::splitString(input,"a");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    static TestResult splitStringSmallWithDelimiter4()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringSmallWithDelimiter4 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "babbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbabab";
        std::vector<std::string> expected_output;
        expected_output.push_back("b");
        expected_output.push_back("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
        expected_output.push_back("b");
        expected_output.push_back("b");

        std::vector<std::string> output = puma::PString::splitString(input,"a");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    static TestResult splitStringSmallWithDelimiter5()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringSmallWithDelimiter5 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "abcadefahi";
        std::vector<std::string> expected_output;
        expected_output.push_back("");
        expected_output.push_back("bc");
        expected_output.push_back("def");
        expected_output.push_back("hi");

        std::vector<std::string> output = puma::PString::splitString(input,"a");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    static TestResult splitStringSmallWithDelimiter6()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringSmallWithDelimiter6 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "a\na\r a\ta";
        std::vector<std::string> expected_output;
        expected_output.push_back("");
        expected_output.push_back("\n");
        expected_output.push_back("\r ");
        expected_output.push_back("\t");
        expected_output.push_back("");

        std::vector<std::string> output = puma::PString::splitString(input,"a");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    static TestResult splitStringSmallWithDelimiter7()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringSmallWithDelimiter7 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "aaa";
        std::vector<std::string> expected_output;
        expected_output.push_back("");
        expected_output.push_back("");
        expected_output.push_back("");
        expected_output.push_back("");

        std::vector<std::string> output = puma::PString::splitString(input,"a");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    static TestResult splitStringSmallWithDelimiter8()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringSmallWithDelimiter8 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "heyo";
        std::vector<std::string> expected_output;
        expected_output.push_back("heyo");

        std::vector<std::string> output = puma::PString::splitString(input,"");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    /*split String Unique********************************************************/


    static TestResult splitStringUniqueEmpty()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringUniqueEmpty - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "";

        std::vector<std::string> output = puma::PString::splitStringUnique(input,"");

        if(!assertEquals(0,(int)output.size(), &result))
        {
            return result;
        }

        return result;

    }

    static TestResult splitStringUniqueEmptyWithDelimiter()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringUniqueEmptyWithDelimiter - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "";

        std::vector<std::string> output = puma::PString::splitStringUnique(input,"abc");

        if(!assertEquals(0,(int)output.size(), &result))
        {
            return result;
        }

        return result;

    }

    static TestResult splitStringUniqueSmallWithDelimiter1()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringUniqueSmallWithDelimiter1 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "ababab";
        std::vector<std::string> expected_output;
        expected_output.push_back("b");
        expected_output.push_back("b");
        expected_output.push_back("b");

        std::vector<std::string> output = puma::PString::splitStringUnique(input,"a");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    static TestResult splitStringUniqueSmallWithDelimiter2()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringUniqueSmallWithDelimiter2 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "bababab";
        std::vector<std::string> expected_output;
        expected_output.push_back("b");
        expected_output.push_back("b");
        expected_output.push_back("b");
        expected_output.push_back("b");

        std::vector<std::string> output = puma::PString::splitStringUnique(input,"a");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    static TestResult splitStringUniqueSmallWithDelimiter3()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringUniqueSmallWithDelimiter3 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "babbababb";
        std::vector<std::string> expected_output;
        expected_output.push_back("b");
        expected_output.push_back("bb");
        expected_output.push_back("b");
        expected_output.push_back("bb");

        std::vector<std::string> output = puma::PString::splitStringUnique(input,"a");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    static TestResult splitStringUniqueSmallWithDelimiter4()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringUniqueSmallWithDelimiter4 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "babbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbabab";
        std::vector<std::string> expected_output;
        expected_output.push_back("b");
        expected_output.push_back("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
        expected_output.push_back("b");
        expected_output.push_back("b");

        std::vector<std::string> output = puma::PString::splitStringUnique(input,"a");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    static TestResult splitStringUniqueSmallWithDelimiter5()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringUniqueSmallWithDelimiter5 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "abcadefahi";
        std::vector<std::string> expected_output;
        expected_output.push_back("bc");
        expected_output.push_back("def");
        expected_output.push_back("hi");

        std::vector<std::string> output = puma::PString::splitStringUnique(input,"a");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    static TestResult splitStringUniqueSmallWithDelimiter6()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringUniqueSmallWithDelimiter6 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "a\na\r a\ta";
        std::vector<std::string> expected_output;
        expected_output.push_back("\n");
        expected_output.push_back("\r ");
        expected_output.push_back("\t");

        std::vector<std::string> output = puma::PString::splitStringUnique(input,"a");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    static TestResult splitStringUniqueSmallWithDelimiter7()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringUniqueSmallWithDelimiter7 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "aaa";
        std::vector<std::string> expected_output;

        std::vector<std::string> output = puma::PString::splitStringUnique(input,"a");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }


        return result;

    }

    static TestResult splitStringUniqueSmallWithDelimiter8()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: splitStringUniqueSmallWithDelimiter8 - basic stress test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "heyo";
        std::vector<std::string> expected_output;
        expected_output.push_back("heyo");

        std::vector<std::string> output = puma::PString::splitStringUnique(input,"");


        if(!assertEquals((int)(expected_output.size()), (int)(output.size()), &result))
        {
            return result;
        }

        for(unsigned i=0; i<output.size();i++)
        {
            if(!assertEquals(expected_output[i],output[i], &result))
            {
                return result;
            }
        }

        return result;

    }

    /*Trim ***********************************************************************/
    static TestResult trimNewLineBack()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimNewLineBack - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test\n";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimNewLineFront()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimNewLineFront - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "\ntest";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimNewLineFrontAndBack()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimNewLineFrontAndBack - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "\ntest\n";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimNewLineMiddle()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimNewLineMiddle - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test\ntest";
        std::string expected = "test\ntest";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimTabBack()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimTabBack - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test\t";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimTabFront()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimTabFront - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "\ttest";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimTabFrontAndBack()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimTabFrontAndBack - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "\ttest\t";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimTabMiddle()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimTabMiddle - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test\ttest";
        std::string expected = "test\ttest";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimReturnBack()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimReturnBack - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test\r";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimReturnFront()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimReturnFront - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "\rtest";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimReturnFrontAndBack()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimReturnFrontAndBack - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "\rtest\r";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimReturnMiddle()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimReturnMiddle - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test\rtest";
        std::string expected = "test\rtest";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimSpaceBack()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimSpaceBack - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test ";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimSpaceFront()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimSpaceFront - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = " test";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimSpaceFrontAndBack()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimSpaceFrontAndBack - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = " test ";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimSpaceMiddle()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimSpaceMiddle - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test test";
        std::string expected = "test test";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimMultiple1()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimMultiple1 - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "  \r\ttest test\n";
        std::string expected = "test test";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimMultiple2()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimMultiple2 - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "  \r\t  test\r\ntest \r \n \n ";
        std::string expected = "test\r\ntest";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult trimSimple()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: trimSimple - basic split test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "Here is a test input string";
        std::string expected = "Here is a test input string";

        if(!assertEquals(expected, puma::PString::trim(input), &result))
        {
            return result;
        }


        return result;
    }

    /*ReplaceFirst ******************************************************************/

    static TestResult replaceFirstSimple()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceFirstSimple - basic replace test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::replaceFirst(input,"",""), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult replaceSingleLetterMultipleOccurence()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceSingleLetterMultipleOccurence - basic replace test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        std::string expected = "west";

        if(!assertEquals(expected, puma::PString::replaceFirst(input,"t","w"), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult replaceMultipleLettersSingleOccurence()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceMultipleLettersSingleOccurence - basic replace test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        std::string expected = "tease";

        if(!assertEquals(expected, puma::PString::replaceFirst(input,"st","ase"), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult replaceSingleLetterSingleOccurences()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceSingleLetterSingleOccurences - basic replace test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        std::string expected = "tsst";

        if(!assertEquals(expected, puma::PString::replaceFirst(input,"e","s"), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult replaceWord()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceWord - basic replace test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "testing the method";
        std::string expected = "testing the function";

        if(!assertEquals(expected, puma::PString::replaceFirst(input,"method","function"), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult replaceExtraOccurence()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceExtraOccurence - basic replace test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "tata";
        std::string expected = "ta";

        if(!assertEquals(expected, puma::PString::replaceFirst(input,"ta",""), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult replaceTooLarge()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceTooLarge - basic replace test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::replaceFirst(input,"testing","ta"), &result))
        {
            return result;
        }

        return result;
    }

    /*ReplaceAll ******************************************************************/

    static TestResult replaceAllSimple()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceAllSimple - basic replace all test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::replaceAll(input,"",""), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult replaceAllSingleLetterMultipleOccurence()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceAllSingleLetterMultipleOccurence - basic replace all test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        std::string expected = "wesw";

        if(!assertEquals(expected, puma::PString::replaceAll(input,"t","w"), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult replaceAllMultipleLettersSingleOccurence()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceAllMultipleLettersSingleOccurence - basic replace all test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        std::string expected = "tease";

        if(!assertEquals(expected, puma::PString::replaceAll(input,"st","ase"), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult replaceAllSingleLetterSingleOccurences()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceAllSingleLetterSingleOccurences - basic replace all test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        std::string expected = "tsst";

        if(!assertEquals(expected, puma::PString::replaceAll(input,"e","s"), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult replaceAllWord()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceAllWord - basic replace all test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "testing the method";
        std::string expected = "testing the function";

        if(!assertEquals(expected, puma::PString::replaceAll(input,"method","function"), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult replaceAllEntireString()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceAllEntireString - basic replace all test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "tata";
        std::string expected = "";

        if(!assertEquals(expected, puma::PString::replaceAll(input,"ta",""), &result))
        {
            return result;
        }


        return result;
    }

    static TestResult replaceAllTooLarge()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceAllTooLarge - basic replace all test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        std::string expected = "test";

        if(!assertEquals(expected, puma::PString::replaceAll(input,"testing","ta"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult replaceAllMultipleWords()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: replaceAllMulpleWords - basic replace all test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test test test not a test but is a test. tests are long but important";
        std::string expected = "fun fun fun not a fun but is a fun. funs are long but important";

        if(!assertEquals(expected, puma::PString::replaceAll(input,"test","fun"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult replaceAllAvoidInfiniteLoop()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: toAvoidInfiniteLoopInReplaceAll";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "11";
        std::string expected = "1111";

        if(!assertEquals(expected, puma::PString::replaceAll(input,"1","11"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult replaceAllAvoidInfiniteLoopWithSmallerSize()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: toAvoidInfiniteLoopInReplaceAll";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "111111";
        std::string expected = "111";

        if(!assertEquals(expected, puma::PString::replaceAll(input,"11","1"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult replaceAllAvoidInfiniteLoopWithSmallerSizeUneven()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: toAvoidInfiniteLoopInReplaceAll";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "1111111";
        std::string expected = "1111";

        if(!assertEquals(expected, puma::PString::replaceAll(input,"11","1"), &result))
        {
            return result;
        }

        return result;
    }

    /*Contains ******************************************************************/

    static TestResult containsSingleLetter()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: containsSingleLetter - basic contains test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "t";
        bool expected = true;

        if(!assertEquals(expected, puma::PString::contains(input,"t"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult containsSingleWord()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: containsSingleWord - basic contains test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        bool expected = true;

        if(!assertEquals(expected, puma::PString::contains(input,"test"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult containsSingleLetterInWord()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: containsSingleLetterInWord - basic contains test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        bool expected = true;

        if(!assertEquals(expected, puma::PString::contains(input,"e"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult containsEmptyString()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: containsEmptyString - basic contains test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        bool expected = false;

        if(!assertEquals(expected, puma::PString::contains(input,""), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult containsTooLargeString()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: containsTooLargeString - basic contains test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        bool expected = false;

        if(!assertEquals(expected, puma::PString::contains(input,"testing"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult containsMultipleWords()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: containsMultipleWords - basic contains test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "I am currently attempting to test whether there are multpile words for the contains test";
        bool expected = true;

        if(!assertEquals(expected, puma::PString::contains(input,"test"), &result))
        {
            return result;
        }

        return result;
    }

    /*starts_with ******************************************************************/
    static TestResult StartsWithSingleLetter()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: StartsWithSingleLetter - basic StartsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "t";
        bool expected = true;

        if(!assertEquals(expected, puma::PString::starts_with(input,"t"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult StartsWithSingleWord()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: StartsWithSingleWord - basic StartsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        bool expected = true;

        if(!assertEquals(expected, puma::PString::starts_with(input,"test"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult StartsWithSingleLetterInWord()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: StartsWithSingleLetterInWord - basic StartsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        bool expected = false;

        if(!assertEquals(expected, puma::PString::starts_with(input,"e"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult StartsWithEmptyString()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: StartsWithEmptyString - basic StartsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        bool expected = false;

        if(!assertEquals(expected, puma::PString::starts_with(input,""), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult StartsWithTooLargeString()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: StartsWithTooLargeString - basic StartsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        bool expected = false;

        if(!assertEquals(expected, puma::PString::starts_with(input,"testing"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult StartsWithMultipleWords()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: StartsWithMultipleWords - basic StartsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "I am currently attempting to test whether there are multpile words for the StartsWith test";
        bool expected = true ;

        if(!assertEquals(expected, puma::PString::starts_with(input,"I am"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult StartsWithMultipleWordsSingleLetter()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: StartsWithMultipleWordsSingleLetter - basic StartsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "I am currently attempting to test whether there are multpile words for the StartsWith test";
        bool expected = true;

        if(!assertEquals(expected, puma::PString::starts_with(input,"I"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult StartsWithMultipleWordsFail()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: StartsWithMultipleWordsFail - basic StartsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "I am currently attempting to test whether there are multpile words for the StartsWith test";
        bool expected = false ;

        if(!assertEquals(expected, puma::PString::starts_with(input,"test"), &result))
        {
            return result;
        }

        return result;
    }

    /*ends_with ******************************************************************/
    static TestResult endsWithSingleLetter()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: endsWithSingleLetter - basic endsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "t";
        bool expected = true;

        if(!assertEquals(expected, puma::PString::ends_with(input,"t"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult endsWithSingleWord()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: endsWithSingleWord - basic endsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        bool expected = true;

        if(!assertEquals(expected, puma::PString::ends_with(input,"test"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult endsWithSingleLetterInWord()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: endsWithSingleLetterInWord - basic endsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);


        std::string input = "test";
        bool expected = false;

        if(!assertEquals(expected, puma::PString::ends_with(input,"e"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult endsWithEmptyString()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: endsWithEmptyString - basic endsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);


        std::string input = "test";
        bool expected = false;

        if(!assertEquals(expected, puma::PString::ends_with(input,""), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult endsWithTooLargeString()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: endsWithTooLargeString - basic endsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "test";
        bool expected = false;

        if(!assertEquals(expected, puma::PString::ends_with(input,"testing"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult endsWithMultipleWords()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: endsWithMultipleWords - basic endsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);



        std::string input = "I am currently attempting to test whether there are multpile words for the endsWith test";
        bool expected = true ;


        std::cout<<"endsWithMultipleWordsSingleLetter"<<std::endl;

        if(!assertEquals(expected, puma::PString::ends_with(input,"endsWith test"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult endsWithMultipleWordsSingleLetter()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: endsWithMultipleWordsSingleLetter - basic endsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "I am currently attempting to test whether there are multpile words for the endsWith test";
        bool expected = true;

        if(!assertEquals(expected, puma::PString::ends_with(input,"test"), &result))
        {
            return result;
        }

        return result;
    }

    static TestResult endsWithMultipleWordsFail()
    {
        std::string suiteName = "GenerateWeave_Test";
        std::string testName = "GenerateWeave_Test: endsWithMultipleWordsFail - basic endsWith test";
        std::string testDescription = "(Extended description if desired)";
        TestResult result(suiteName, testName, 1, testDescription);

        std::string input = "I am currently attempting to test whether there are multpile words for the endsWith test";
        bool expected = false ;


        if(!assertEquals(expected, puma::PString::ends_with(input,"currently"), &result))
        {
            return result;
        }

        return result;
    }

};
