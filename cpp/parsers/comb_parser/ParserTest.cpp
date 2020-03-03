#include "Parsers.hpp"

#include <prez/unit_test.hpp>


using namespace std;
using namespace parsers;

prez::UnitTest tester = prez::UnitTest::createTester();


void testAnyChar()
{
    auto anySkipWs = anyChar();
    auto anyNoSkipWs = anyChar(false);

    tester.assertEquals(anySkipWs.parse("x"), 'x');
    tester.assertEquals(anySkipWs.parse(" \n\tx"), 'x');
    tester.assertThrows([&anySkipWs]() {
        anySkipWs.parse("xx");
    });

    tester.assertEquals(anyNoSkipWs.parse("x"), 'x');
    tester.assertThrows([&anyNoSkipWs]() {
        anyNoSkipWs.parse(" \n\tx");
    });
    tester.assertThrows([&anySkipWs]() {
        anySkipWs.parse("xx");
    });
}

void testThisChar()
{
    auto a = thisChar('x');

    tester.assertEquals(a.parse("x"), 'x');
    tester.assertThrows([&a]() {
        a.parse("y");
    });
    tester.assertThrows([&a]() {
        a.parse("xx");
    });
}


int main()
{
    testAnyChar();
    testThisChar();

    return 0;
}

