#include "testing-program.hpp"
using namespace std;

// Initialize static variables
size_t TestingProgram::fails_ = 0;
size_t TestingProgram::totalChecks_ = 0;
bool TestingProgram::streamInitialized = false;
ifstream TestingProgram::in{__FILE__};
size_t TestingProgram::currentLine = 1;
unordered_map<int, std::string> TestingProgram::prevLines;
size_t TestingProgram::testsFailed_ = 0;
size_t TestingProgram::totalTests_ = 0;
bool TestingProgram::alreadyMarkedFailure_ = false;

// Constructor
TestingProgram::TestingProgram(string name)
    :testName_{name}
{
    // Reset Individual test parameters when a new test is created
    fails_ = 0;
    totalChecks_ = 0;
    alreadyMarkedFailure_ = false;
    // Increment total number of tests
    ++totalTests_;
    // Print the test name
    cerr << testName_ << endl;
}

