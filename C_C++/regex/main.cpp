#include "regex.hpp"
#include "parser.hpp"

#include <iostream>
#include <memory>

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 2) {
        cerr << "Enter a pattern." << endl;
    }

    try {
        unique_ptr<Regex> rgx2 = Regex::kConcatParser.parse(argv[1]);
        rgx2->print();
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }

    try {
        unique_ptr<Regex> rgx = Regex::kRegexParser.parse(argv[1]);
        rgx->print();
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }

    // ALT() ISN'T SUPPOSED TO BE USED FOR THIS
    Parser<string> p =
        parsers::thisChar('a').andThenMap([](char c){ return string(1, c); })
        .alt(parsers::thisChar('a').many());

    try {
        string s = p.parse(argv[1]);
        cout << s << endl;
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
}
