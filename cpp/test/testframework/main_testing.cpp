#include "../test/mastertest.h"
#include "color.h"
#include "toString.h"
#include "testresult.h"

#include <cstring>
#include <set>


std::vector<int> parseInputs(int argc, char** argv);
void printTests(std::vector<TestResult> failedTests, int numTests);

int main(int argc, char** argv) {

    MasterTest tester;

    std::set<int> indexSet;

    if( argc>1 ) {
        if( !strcmp(argv[1],"-h") ) {
            tester.printTestSuites();
            return 0;
        } else if( !strcmp(argv[1],"-a") ) {
            //running all tests
        } else {
            for(int i=1; i<argc; i++) {

                std::string arg = argv[i];

                if (arg.find("-") != std::string::npos ) {//found
                    std::vector<std::string> split = toString::splitString(arg,"-");

                    if(split.size()!=2) {
                        std::cout << "Invalid Inputs - Use '-h' for help or -a for all" << std::endl;
                        return 0;
                    }
                    int begin = atoi(split[0].c_str());
                    int end = atoi(split[1].c_str());

                    if( begin==0 && strcmp(split[0].c_str(),"0") ) { //split[0] was not valid integer
                        std::cout << "Invalid Inputs - Use '-h' for help or -a for all" << std::endl;
                        return 0;
                    }

                    if( end==0 && strcmp(split[1].c_str(),"0") ) { //split[1] was not valid integer
                        std::cout << "Invalid Inputs - Use '-h' for help or -a for all" << std::endl;
                        return 0;
                    }

                    if( begin>end || begin<0 ) {
                        std::cout << "Invalid Inputs - Use '-h' for help or -a for all" << std::endl;
                        return 0;
                    }

                    for(int index=begin; index<=end; index++) {
                        indexSet.insert( index );
                    }

                } else {
                    int index = atoi(argv[i]);
                    if( index==0 ) {
                        if( !strcmp(argv[i],"0") ) {
                            indexSet.insert( index );
                        } else {
                            std::cout << "Invalid Inputs - Use '-h' for help or -a for all" << std::endl;
                            return 0;
                        }
                    } else {
                        indexSet.insert( index );
                    }
                }
            }
        }
    }

    std::vector<int> testIndices;
    std::copy(indexSet.begin(), indexSet.end(), std::back_inserter(testIndices));

    if( !tester.validSubTests(testIndices) ) {
        std::cout << "Invalid SubTest Indices" << std::endl;
        return 0;
    }

    std::vector<TestResult> failedTests;
    int numTests;
    if( testIndices.size() > 0 ) {
        failedTests = tester.runTests(testIndices);
        numTests = tester.numTests(testIndices);
    } else {
        failedTests = tester.runAllTests();
        numTests = tester.numTests();
    }

    printTests(failedTests, numTests);

    return 0;
}


void printTests(std::vector<TestResult> failedTests, int numTests){

    int failCount = failedTests.size();
    int passCount = numTests-failCount;

    Color::Modifier red(Color::FG_RED);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier def(Color::FG_DEFAULT);
    std::string bold = "\e[1m";
    std::string regular = "\e[0m";

    if(failedTests.size() > 0) {
        std::cout << bold << red << "\n[---- Passed " << passCount << " / " << numTests << " Tests ----]\n" << regular << std::endl;

        std::cout << "\n[----- Failed Tests: -----]" << std::endl;
        for(unsigned int i=0; i<failedTests.size(); i++) {
            std::cout << def << "    Failed Test " << red << i+1 << def << " { "<< std::endl;
            std::cout << "        Test Suite: " << red << failedTests[i].suiteName << def << " Test Number: " << red << failedTests[i].testNumber << def << std::endl;
            std::cout << def << "        Test Title: " << red << std::endl;
            std::cout << "            " << failedTests[i].testTitle  << std::endl;
            std::cout << def << "        Test Description: " << red << std::endl;
            std::cout << "            " << failedTests[i].testDescription  << std::endl;
            std::cout << def << "        Expected: " << red <<  failedTests[i].expected << def <<  "  Actual: " << red <<  failedTests[i].actual << def << std::endl;
            std::cout << "     }" << std::endl;
        }
    }
    else {
        std::cout << bold << green << "\n[---- Passed " << passCount << " / " << numTests << " Tests ----]\n" << def << regular  << std::endl;
    }


}

