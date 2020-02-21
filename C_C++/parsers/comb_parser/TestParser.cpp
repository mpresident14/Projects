#include "Parsers.hpp"

#include <unit_test.hpp>
#include <cstring>

using namespace std;
using namespace parsers;

/***************************
 * Tests for functionality *
 ***************************/

UnitTest tester = createTester();

void testCharParser()
{
    tester.initTest();

    tester.affirm(anyChar.parse("$") == '$');

    try {
        anyChar.parse("$4");
    } catch (invalid_argument& e) {
        tester.affirm(!strcmp(e.what(), "No parse: 4 remained."));
    }

    tester.printResults();
}

void testConditionalParser()
{
    tester.initTest();

    ConditionalParser<char> aParser = cond<char>(anyChar, [](char& c) { return c == 'a'; });

    // tester.affirm(aParser.parse("a") == 'a');

    tester.printResults();
}

int main()
{
    testCharParser();
    testConditionalParser();


    tester.summarize();
    return 0;
}
