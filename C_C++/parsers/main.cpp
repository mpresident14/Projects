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
        [](const string& input) -> optional<pair<Widget, string>> {
            if (input.empty()) {
                return {};
            }

            return make_optional(make_pair(Widget::createWidget(input[0]), input.substr(1)));
        };

    int n = 6;
    auto parseIntFn = 
        [&n](const string& input) -> optional<pair<int, string>> {
            if (input.empty()) {
                return {};
            }

            return make_optional(make_pair(n, input.substr(1)));
        };
   
    // auto pGen{
    //     [&parseIntFn](const Widget&){ 
    //         return Parser<int>{parseIntFn}; 
    //     }
    // };

    // Parser<Widget> p1{parseFn};
    // Parser<int> p2 = p1.andThen(move(pGen));

    // try {
    //     int w = p2.parse(argv[1]);
    //     cout << w << endl;
    // } catch (invalid_argument& e) {
    //     cerr << e.what() << endl;
    // }

    //**************
    // Parser<Widget> p1{parseFn};
    // Parser<int> p2{parseIntFn};
    // Parser<pair<Widget, int>> p3 = p1.combine(p2);
    
    // try {
    //     pair<Widget, int> widgets = p3.parse(argv[1]);
    //     cout << widgets.first << " " << widgets.second << endl;
    // } catch (invalid_argument& e) {
    //     cerr << e.what() << endl;
    // }
    //**************

    Parser<Widget> p1{parseFn};
    Parser<int> p2{parseIntFn};
    Parser<Widget> p3{parseFn};
    auto pCombo = p1.combine(p2).combine(p3);
    try {
        auto items = pCombo.parse(argv[1]);
        cout << items.first.first << " " << items.first.second << " " << items.second << endl;
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }


    // try {
    //     auto chars = parsers::getSingleChar.combine(parsers::getSingleChar).combine(parsers::getSingleChar).parse(argv[1]);
    //     cout << chars.first.first << " " << chars.first.second << " "  << chars.second << endl;
    // } catch (invalid_argument& e) {
    //     cerr << e.what() << endl;
    // }
}
