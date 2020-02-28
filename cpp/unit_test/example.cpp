#include "unit_test.hpp"

using namespace std;

struct Widget { int val_;};
bool operator==(const Widget& w1, const Widget& w2) { return w1.val_ == w2.val_; }

auto tester = prez::UnitTest::createTester();

void testAssertTrue()
{
  tester.assertTrue(false);
  tester.assertTrue(true);
}

void testAssertFalse()
{
  tester.assertFalse(false);
  tester.assertFalse(true);
}


void testAssertEquals()
{
  tester.assertEquals(Widget{1}, Widget{2});
  tester.assertEquals(1,1);
  tester.assertEquals("hello", "goodbye");
}

void testAssertNotEqual()
{
  tester.assertNotEqual(1, 2);
  tester.assertNotEqual(4, 4);
}


void testAssertThrows()
{
  auto throwing = [](){ throw invalid_argument("Error!"); };
  tester.assertThrows(throwing, "Error!");

  auto nonThrowing = [](){ return 0; };
  tester.assertThrows(nonThrowing, "Error!");
}

void testLoop()
{
  for (size_t i = 0; i < 10; ++i) {
    tester.assertTrue(i == 0 + i);
  }
}

void testAllPass()
{
  tester.assertTrue(true);
  tester.assertFalse(false);
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

  return 0;
}
