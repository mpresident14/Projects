#include "CharParser.hpp"

#include <unit_test.hpp>
#include <cstring>

using namespace std;

/***************************
 * Tests for functionality *
 ***************************/

UnitTest tester = createTester();

void testCharParser()
{
    tester.initTest();

    CharParser p('$');

    tester.affirm(p.parse("$") == '$');

    try {
        p.parse("$4");
    } catch (invalid_argument& e) {
        tester.affirm(!strcmp(e.what(), "No parse: 4 remained."));
    }

    try {
        p.parse("a");
    } catch (invalid_argument& e) {
        tester.affirm(!strcmp(e.what(), "Expected: $."));
    }

    tester.printResults();
}


int main()
{
    testCharParser();


    tester.summarize();
    return 0;
}
