#include "Parsers.hpp"

#include <prez/unit_test.hpp>


using namespace std;
using namespace parsers;

prez::UnitTest tester = prez::UnitTest::createTester();


void testCharParser()
{
    auto any = anyChar();

    tester.assertEquals('x', any.parse("x"));
    tester.assertThrows([&any]() {
        any.parse("xx");
    });
    tester.assertThrows([&any]() {
        any.parse("");
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


void testStringParser()
{
    string expected = "parser";
    auto hello = thisString(expected);

    tester.assertEquals(expected, hello.parse(expected));
    tester.assertThrows([&hello]() {
        hello.parse("parsers");
    });
    tester.assertThrows([&hello]() {
        hello.parse("parse");
    });
    tester.assertThrows([&hello]() {
        hello.parse("hello");
    });
}


void testConditionalParser()
{
    auto lessThan100 = doOnlyIf(anyULong, [](unsigned long n) { return n < 100; });

    tester.assertEquals(30, lessThan100.parse("30"));
    tester.assertThrows([&lessThan100]() {
        lessThan100.parse("234");
    });
    tester.assertThrows([&lessThan100]() {
        lessThan100.parse("abc");
    });
}


void testMapParser()
{
    auto strLen = transform(thisString("hello"), [](string&& s) { return s.length(); });

    tester.assertEquals(5, strLen.parse("hello"));
    tester.assertThrows([&strLen]() {
        strLen.parse("goodbye");
    });
}


void testManyParser()
{
    auto ignoreManyDigits = many(skip(anyDigitChar));
    auto manyAs = many(thisChar('a'));
    auto manyNumbers = many(anyULong);

    tester.assertTrue((is_same_v<ignore_t, decltype(ignoreManyDigits.parse("12345"))>));
    ignoreManyDigits.parse("12345");
    tester.assertThrows([&ignoreManyDigits]() {
        ignoreManyDigits.parse("123ab");
    });


    tester.assertEquals("aaaa", manyAs.parse("aaaa"));
    tester.assertEquals("", manyAs.parse(""));
    tester.assertThrows([&manyAs]() {
        manyAs.parse("aaab");
    });

    vector<unsigned long> v = {1, 100, 1000};
    tester.assertEquals(v, manyNumbers.parse("1 100 1000"));
    tester.assertEquals(vector<unsigned long>(), manyNumbers.parse(""));
    tester.assertThrows([&manyNumbers]() {
        manyNumbers.parse("100a");
    });
}


int main()
{
    testCharParser();
    testThisChar();
    testStringParser();
    testConditionalParser();
    testMapParser();
    testManyParser();

    return 0;
}

