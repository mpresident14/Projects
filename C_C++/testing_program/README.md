# TestingProgram
A program to test your code.

Instructions for using the TestingProgram class:

1. #include "testing-program.hpp" in your test file.

2. Write each of your tests in separate functions. Begin each test by creating a TestingProgram object with the name of the test. Use     affirm to check statements that should be true. End the test with a call to the static function TestingProgram::printResults() to get statistics about the individual test.

3. In your main function, call each of your tests. End the main function with a call to the static function TestingProgram::printResults() to get statistics about your entire test program.

See example.cpp for a sample use of the TestingProgram class.
