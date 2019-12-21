#include "parser.hpp"
#include "parsers.hpp"
#include "widget.hpp"

using namespace std;

#include <iostream>
#include <optional>
#include <string>
#include <stdexcept>
#include <ctype.h>


int main(int argc, char **argv)
{
    Parser<Widget> p{
        [](const string& input) -> std::optional<std::pair<Widget, std::string>> {
            if (input.empty()) {
                return {};
            }

            return make_optional(make_pair(Widget::createWidget(input[0]), input.substr(1)));
        }
    };

    Parser<pair<Widget, Widget>> p2 = p.combine(p);

    Parser<pair<char, char>> q = getSingleChar.combine(getSingleChar);
    
    try {
        pair<Widget, Widget> widgets = p2.parse(argv[1]);
        cout << widgets.first << " " << widgets.second << endl;
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }

    try {
        pair<char, char> chars = q.parse(argv[1]);
        cout << chars.first << " "  << chars.second << endl;
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
}