#include "unit_test.hpp"

using namespace std;

/***************************
 * Tests for functionality *
 ***************************/

struct Widget {
  int val_;
  // bool operator==(const Widget& other) { return val_ == other.val_; }
};
bool operator==(const Widget& w1, const Widget& w2)
{ return w1.val_ == w2.val_; }

auto tester = mcprez::UnitTest::createTester();

void testAssertTrue()
{
  tester.initTest();

  tester.assertTrue(false);
  tester.assertTrue(true);

  tester.printResults();
}

void testAssertFalse()
{
  tester.initTest();

  tester.assertFalse(false);
  tester.assertFalse(true);

  tester.printResults();
}


void testAssertEquals()
{
  tester.initTest();

  tester.assertEquals(Widget{1}, Widget{2});
  tester.assertEquals(1,1);
  tester.assertEquals("hello", "goodbye");

  tester.printResults();
}

void testAssertNotEqual()
{
  tester.initTest();

  tester.assertNotEqual(1, 2);
  tester.assertNotEqual(4, 4);

  tester.printResults();
}


void testAssertThrows()
{
  tester.initTest();

  auto throwing = [](){ throw invalid_argument("Error!"); };
  tester.assertThrows(throwing, "Error!");

  auto nonThrowing = [](){ return 0; };
  tester.assertThrows(nonThrowing, "Error!");

  tester.printResults();
}

void testLoop()
{
  tester.initTest();

  for (size_t i = 0; i < 10; ++i) {
    tester.assertTrue(i == 0 + i);
  }

  tester.printResults();

}

void testAllPass()
{
  tester.initTest();

  tester.assertTrue(true);
  tester.assertFalse(false);

  tester.printResults();
}

int main()
{
  // Call each of your tests
  testAssertTrue();
  testAssertFalse();
  testAssertEquals();
  testAssertNotEqual();
  testAssertThrows();
  testLoop();
  testAllPass();

  // Print results of your test file
  tester.summarize();
  return 0;
}
