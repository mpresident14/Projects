#include "Parsers.hpp"

#include <bits/stdc++.h>

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


void testIgnoreParser()
{
    auto ignoreChar = skip(anyChar());
    tester.assertTrue((is_same_v<ignore_t, decltype(ignoreChar.parse("a"))>));
    tester.assertThrows([&ignoreChar]() {
        ignoreChar.parse("ab");
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


void testAltParser()
{
    auto abc = alt(thisChar('a'), thisChar('b'), thisChar('c'));

    tester.assertEquals('a', abc.parse("a"));
    tester.assertEquals('b', abc.parse("b"));
    tester.assertEquals('c', abc.parse("c"));
    tester.assertThrows([&abc]() {
        abc.parse("d");
    });
    tester.assertEquals("ababcbcabb", many(abc).parse("ababcbcabb"));


    auto strP = alt(thisString("ant"), thisString("ants"), thisString("antsy"));
    tester.assertThrows([&strP]() {
        strP.parse("ants"); // should find "ant" and stop
    });
    auto betterStrP = alt(thisString("antsy"), thisString("ants"), thisString("ant"));
    tester.assertEquals("ants", betterStrP.parse("ants"));
}


void testSeqParser()
{
    auto hello = thisString("hello");
    auto hodgepodge = seq(hello, thisChar('a'), anyULong);
    tester.assertEquals(std::tuple(string("hello"), 'a', 1234), hodgepodge.parse("helloa  1234"));
    tester.assertThrows([&hodgepodge]() {
        hodgepodge.parse("helloa bye");
    });

    auto hodgepodge2 = seq(hello, thisChar('b'), anyULong);
    auto withAlt = alt(hodgepodge, hodgepodge2);
    tester.assertEquals(std::tuple(string("hello"), 'b', 1234), withAlt.parse("hellob  1234"));

    auto withSkip = seq(skip(hello), anyULong);
    tester.assertEquals(std::tuple(1234), withSkip.parse("hello 1234"));
    tester.assertThrows([&withSkip]() {
        withSkip.parse("1234");
    });
}


void testLazyParser()
{
    auto rAssocAdd = lazy<unsigned long>();
    auto plusNum = ignoreAndThen(thisChar('+'), rAssocAdd);
    rAssocAdd.set(
        transform(
            seq(
                anyULong,
                transform(
                    many(plusNum),
                    [](auto&& v) { return accumulate(v.begin(), v.end(), 0); })
            ),
            [](auto&& nums) { return get<0>(nums) + get<1>(nums); }));

    tester.assertEquals(10, rAssocAdd.parse("1+ 2 + 3 +4"));
    cout << rAssocAdd.parse("2+4+ 4+1412") << endl;

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

