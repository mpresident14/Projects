#include "Parsers.hpp"

#include <iostream>
#include <string>


using namespace std;

int main()
{
    auto BP = parsers::anyChar.onlyIf([](const char& c) { return c == 'b'; });

    string (*lambda1)(char&&) = [](char&& c) { return string(3, c); };
    size_t (*lambda2)(string&&) = [](string&& s) { return s.size(); };

    auto BTo3B = BP.mapTo(lambda1).mapTo(lambda2);

    cout << BTo3B.parse("b") << endl;

    return 0;
}
