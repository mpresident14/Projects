#include "parser.hpp"
#include "parsers.hpp"
#include "widget.hpp"

#include <iostream>
#include <optional>
#include <string>
#include <stdexcept>
#include <ctype.h>

using namespace std;

int main(int argc, char **argv)
{
    auto parseFn = 
        MovableFn<std::optional<std::pair<Widget, std::string>>, const std::string&>{
        [](const string& input) -> optional<pair<Widget, string>> {
            if (input.empty()) {
                return {};
            }

            return make_optional(make_pair(Widget::createWidget(input[0]), input.substr(1)));
        }
    };
   
    MovableFn<Parser<Widget>, Widget&&> m{
        [&](Widget&& w){ 
            return Parser<Widget>{parseFn}; 
        }
    };

    Parser<Widget> p1{parseFn};
    Parser<Widget> p2 = p1.andThen(move(m));

    try {
        Widget w = p2.parse(argv[1]);
        cout << w << endl;
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }

    // Parser<pair<Widget, Widget>> p2 = p.combine(p);
    
    // try {
    //     pair<Widget, Widget> widgets = p2.parse(argv[1]);
    //     cout << widgets.first << " " << widgets.second << endl;
    // } catch (invalid_argument& e) {
    //     cerr << e.what() << endl;
    // }

    // try {
    //     pair<char, char> chars = q.parse(argv[1]);
    //     cout << chars.first << " "  << chars.second << endl;
    // } catch (invalid_argument& e) {
    //     cerr << e.what() << endl;
    // }
}