#include "unit_test.hpp"

using namespace std;

/***************************
 * Tests for functionality *
 ***************************/

UnitTest tester = createTester();

void test1()
{
  tester.initTest();

  tester.assertEquals(1, 1);
  for (size_t i = 0; i < 10; ++i) {
    tester.assertTrue(i == 0 + i);
  }

  tester.printResults();

}

void test2()
{
  tester.initTest();

  tester.assertTrue(false);
  tester.assertTrue(true);
  tester.assertNotEqual(9, 4 + 5);

  tester.printResults();
}

void test3()
{
  tester.initTest();

  auto lambda = [](){ throw invalid_argument("Error!"); };
  tester.assertThrows(lambda, "Error!");
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
