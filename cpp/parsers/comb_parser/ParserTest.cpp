#include "Parsers.hpp"

#include <prez/unit_test.hpp>


using namespace std;
using namespace parsers;

prez::UnitTest tester = prez::UnitTest::createTester();


void testAnyChar()
{
    auto anySkipWs = anyChar();
    auto anyNoSkipWs = anyChar(false);

    tester.assertEquals('x', anySkipWs.parse("x"));
    tester.assertEquals('x', anySkipWs.parse(" \n\tx"));
    tester.assertThrows([&anySkipWs]() {
        anySkipWs.parse("xx");
    });

    tester.assertEquals('x', anyNoSkipWs.parse("x"));
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

    tester.assertEquals('x', a.parse("x"));
    tester.assertThrows([&a]() {
        a.parse("y");
    });
    tester.assertThrows([&a]() {
        a.parse("xx");
    });
}


void testThisString()
{
    string expected = "parser";
    auto helloSkipWs = thisString(expected);
    auto helloNoSkipWs = thisString(expected, false);

    tester.assertEquals(expected, helloSkipWs.parse(expected));
    tester.assertEquals(expected, helloSkipWs.parse("\n \t" + expected));
    tester.assertThrows([&helloSkipWs]() {
        helloSkipWs.parse("parsers");
    });
    tester.assertThrows([&helloSkipWs]() {
        helloSkipWs.parse("parse");
    });
    tester.assertThrows([&helloSkipWs]() {
        helloSkipWs.parse("hello");
    });

    tester.assertEquals(expected, helloNoSkipWs.parse(expected));
    // Pretty weird that multiple captures have to go in parentheses.
    tester.assertThrows(([&helloNoSkipWs, &expected]() {
        helloNoSkipWs.parse("\n \t" + expected);
    }));
    tester.assertThrows([&helloNoSkipWs]() {
        helloNoSkipWs.parse("parsers");
    });
    tester.assertThrows([&helloNoSkipWs]() {
        helloNoSkipWs.parse("parse");
    });
    tester.assertThrows([&helloNoSkipWs]() {
        helloNoSkipWs.parse("hello");
    });
}


int main()
{
    testAnyChar();
    testThisChar();
    testThisString();

    return 0;
}

