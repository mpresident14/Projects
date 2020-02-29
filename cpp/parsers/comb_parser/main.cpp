#include "Parsers.hpp"

#include <iostream>
#include <string>


using namespace std;

int main()
{
    char match = 'a';
    auto a = parsers::anyChar().onlyIf([&match](const char& c) { return c == match; });
    auto b = parsers::anyChar().onlyIf([](const char& c) { return c == 'b'; });
    auto c = parsers::anyChar().onlyIf([](const char& c) { return c == 'c'; });
    auto abc = parsers::alt(a, b, c);

    int n = 3;
    auto lambda1 = [&n](char&& c) { return string(n, c); };
    size_t (*lambda2)(string&&) = [](string&& s) { return s.size(); };

    auto abc3 = abc.mapTo(lambda1).mapTo(lambda2);

    cout << abc3.parse("aaa") << endl;
    cout << abc3.parse("bbb") << endl;
    cout << abc3.parse("ccc") << endl;

    auto pStr = parsers::lazy<string>();

    auto a2 = parsers::anyChar().onlyIf([&match](const char& c) { return c == match; });

    pStr.set(a2.mapTo([](char&& c) { return string(3, c); }));


    cout << pStr.parse("aaa") << endl;


    return 0;
}
