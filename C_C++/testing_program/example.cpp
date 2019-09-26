#include "testing-program.hpp"

using namespace std;

/***************************
 * Tests for functionality *
 ***************************/

void test1() {
  TestingProgram tester{"Test 1"};  // Set up the testing object

  affirm(1 == 1);
  for (size_t i = 0; i < 10; ++i) {
    affirm(i == 0 + i);
  }

  TestingProgram::printResults();  // Print results about the
                                   // individual test
}

void test2() {
  TestingProgram tester{"Test 2"};

  affirm(false);  // This fails
  affirm(true);
  affirm(9 == 8);  // This fails

  TestingProgram::printResults();
}

void test3() {
  TestingProgram tester{"Test 3"};

  int x[2] = {2, 5};
  affirm(x[0] == 2);
  affirm(x[1] == 5);

  TestingProgram::printResults();
}

int main() {
  // Call each of your tests
  test1();
  test2();
  test3();

  // Print results of your test file
  TestingProgram::summarize();
  return 0;
}