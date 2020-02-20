#include "unit_test.hpp"

using namespace std;

/***************************
 * Tests for functionality *
 ***************************/

UnitTest tester = createTester();

void test1()
{
  tester.initTest();

  tester.affirm(1 == 1);
  for (size_t i = 0; i < 10; ++i) {
    tester.affirm(i == 0 + i);
  }

  tester.printResults();  // Print results about the
                                   // individual test
}

void test2()
{
  tester.initTest();

  tester.affirm(false);  // This fails
  tester.affirm(true);
  tester.affirm(9 == 8);  // This fails

  tester.printResults();
}

void test3()
{
  tester.initTest();

  int x[2] = {2, 5};
  tester.affirm(x[0] == 2);
  tester.affirm(x[1] == 5);

  tester.printResults();
}

int main()
{
  // Call each of your tests
  test1();
  test2();
  test3();

  // Print results of your test file
  tester.summarize();
  return 0;
}
