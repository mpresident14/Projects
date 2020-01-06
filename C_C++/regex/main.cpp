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

    unique_ptr<Regex> rgx = Regex::kRegexParser.parse(argv[1]);
    // unique_ptr<Regex> rgx2 = Regex::kCharParser.parse(argv[1]);
    // auto p = parsers::thisChar('a');
    // cout << "hi" << endl;
    // cout << p.parse(argv[1]) << endl;
    rgx->print();
    // rgx2->print();

    // unique_ptr<Regex> rgx = make_unique<Dot>(Dot());
}
