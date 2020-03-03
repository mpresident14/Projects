#include "Parsers.hpp"

#include <bits/stdc++.h>

#include <prez/unit_test.hpp>


using namespace std;
using namespace parsers;

prez::UnitTest tester = prez::UnitTest::createTester();


void testCharParser()
{
    auto any = anyChar();

    tester.assertEquals('x', any.parseAll("x"));
    tester.assertThrows([&any]() {
        any.parseAll("xx");
    });
    tester.assertThrows([&any]() {
        any.parseAll("");
    });
}


void testThisChar()
{
    auto a = thisChar('x');

    tester.assertEquals('x', a.parseAll("x"));
    tester.assertThrows([&a]() {
        a.parseAll("y");
    });
    tester.assertThrows([&a]() {
        a.parseAll("xx");
    });
}


void testStringParser()
{
    string expected = "parser";
    auto hello = thisString(expected);

    tester.assertEquals(expected, hello.parseAll(expected));
    tester.assertThrows([&hello]() {
        hello.parseAll("parsers");
    });
    tester.assertThrows([&hello]() {
        hello.parseAll("parse");
    });
    tester.assertThrows([&hello]() {
        hello.parseAll("hello");
    });
}


void testConditionalParser()
{
    auto lessThan100 = doOnlyIf(anyULong, [](unsigned long n) { return n < 100; });

    tester.assertEquals(30, lessThan100.parseAll("30"));
    tester.assertThrows([&lessThan100]() {
        lessThan100.parseAll("234");
    });
    tester.assertThrows([&lessThan100]() {
        lessThan100.parseAll("abc");
    });
}


void testMapParser()
{
    auto strLen = transform(thisString("hello"), [](string&& s) { return s.length(); });

    tester.assertEquals(5, strLen.parseAll("hello"));
    tester.assertThrows([&strLen]() {
        strLen.parseAll("goodbye");
    });
}


void testIgnoreParser()
{
    auto ignoreChar = skip(anyChar());
    tester.assertTrue((is_same_v<ignore_t, decltype(ignoreChar.parseAll("a"))>));
    tester.assertThrows([&ignoreChar]() {
        ignoreChar.parseAll("ab");
    });
}


void testManyParser()
{
    auto ignoreManyDigits = many(skip(anyDigitChar));
    auto manyAs = many(thisChar('a'));
    auto manyNumbers = many(anyULong);

    tester.assertTrue((is_same_v<ignore_t, decltype(ignoreManyDigits.parseAll("12345"))>));
    ignoreManyDigits.parseAll("12345");
    tester.assertThrows([&ignoreManyDigits]() {
        ignoreManyDigits.parseAll("123ab");
    });


    tester.assertEquals("aaaa", manyAs.parseAll("aaaa"));
    tester.assertEquals("", manyAs.parseAll(""));
    tester.assertThrows([&manyAs]() {
        manyAs.parseAll("aaab");
    });

    vector<unsigned long> v = {1, 100, 1000};
    tester.assertEquals(v, manyNumbers.parseAll("1 100 1000"));
    tester.assertEquals(vector<unsigned long>(), manyNumbers.parseAll(""));
    tester.assertThrows([&manyNumbers]() {
        manyNumbers.parseAll("100a");
    });
}


void testAltParser()
{
    auto abc = alt(thisChar('a'), thisChar('b'), thisChar('c'));

    tester.assertEquals('a', abc.parseAll("a"));
    tester.assertEquals('b', abc.parseAll("b"));
    tester.assertEquals('c', abc.parseAll("c"));
    tester.assertThrows([&abc]() {
        abc.parseAll("d");
    });
    tester.assertEquals("ababcbcabb", many(abc).parseAll("ababcbcabb"));


    auto strP = alt(thisString("ant"), thisString("ants"), thisString("antsy"));
    tester.assertThrows([&strP]() {
        strP.parseAll("ants"); // should find "ant" and stop
    });
    auto betterStrP = alt(thisString("antsy"), thisString("ants"), thisString("ant"));
    tester.assertEquals("ants", betterStrP.parseAll("ants"));
}


void testSeqParser()
{
    auto hello = thisString("hello");
    auto hodgepodge = seq(hello, thisChar('a'), anyULong);
    tester.assertEquals(std::tuple(string("hello"), 'a', 1234), hodgepodge.parseAll("helloa  1234"));
    tester.assertThrows([&hodgepodge]() {
        hodgepodge.parseAll("helloa bye");
    });

    auto hodgepodge2 = seq(hello, thisChar('b'), anyULong);
    auto withAlt = alt(hodgepodge, hodgepodge2);
    tester.assertEquals(std::tuple(string("hello"), 'b', 1234), withAlt.parseAll("hellob  1234"));

    auto withSkip = seq(skip(hello), anyULong);
    tester.assertEquals(std::tuple(1234), withSkip.parseAll("hello 1234"));
    tester.assertThrows([&withSkip]() {
        withSkip.parseAll("1234");
    });
}


void testLazyParser()
{
    auto rAssocAdd = lazy<unsigned long>();
    auto plusNum = ignoreAndThen(skipWs(thisChar('+')), rAssocAdd.getRef());
    rAssocAdd.set(
        transform(
            seq(
                anyULong,
                transform(
                    many(plusNum),
                    [](auto&& v) { return accumulate(v.begin(), v.end(), 0); })
            ),
            [](auto&& nums) { return get<0>(nums) + get<1>(nums); }));

    tester.assertEquals(10, rAssocAdd.parseAll("1+ 2 + 3 +4"));
}


int main()
{
    testCharParser();
    testThisChar();
    testStringParser();
    testConditionalParser();
    testMapParser();
    testIgnoreParser();
    testManyParser();
    testAltParser();
    testSeqParser();
    testLazyParser();

    return 0;
}

