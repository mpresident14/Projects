#include "Parsers.hpp"

#include <iostream>
#include <string>
#include <sstream>

#include <boost/type_index.hpp>


using namespace std;
using namespace boost::typeindex;

int main()
{
    char match = 'a';
    const auto a = parsers::anyChar().onlyIf([&match](const char& c) { return c == match; });
    auto b = parsers::anyChar().onlyIf([](const char& c) { return c == 'b'; });
    auto c = parsers::anyChar(false).onlyIf([](const char& c) { return c == 'c'; });
    auto abc = parsers::alt(a, b, c);

    int n = 3;
    auto lambda1 = [&n](char&& c) { return string(n, c); };
    size_t (*lambda2)(string&&) = [](string&& s) { return s.size(); };

    auto abc3 = abc.mapTo(lambda1).mapTo(lambda2);

    cout << abc3.parse(" a") << endl;
    cout << abc3.parse("b") << endl;
    cout << abc3.parse("c") << endl;

    auto pStr = parsers::lazy<string>();
    pStr.set(a.mapTo([](char&& c) { return string(3, c); }));
    cout << pStr.parse("a") << endl;

    auto acbSeq = parsers::seq(a,c,b);
    auto abcSeq = parsers::seq(a,b,c);
    auto tupResult = parsers::alt(acbSeq, abcSeq).parse("abc");
    cout << get<0>(tupResult) << ", " << get<1>(tupResult) << ", " << get<2>(tupResult) << endl;

    auto manyA = parsers::many(a);
    auto v = manyA.parse("aaaaaaaaaaaaaaaaaaaa");
    cout << v.size() << endl;

    auto ignoreA = parsers::ignore(a);
    ignoreA.parse("a");

    auto ignoreAThenCB = parsers::seq(ignoreA,c,b);
    auto ignoreTup = ignoreAThenCB.parse("acb");
    cout << get<0>(ignoreTup) << ", " << get<1>(ignoreTup) << endl;

    auto strP = parsers::thisString("hello");
    cout << strP.parse("    hello") << endl;

    auto testP = parsers::alt(
        parsers::thisString("evers"),
        parsers::thisString("ever"));
    cout << testP.parse("ever") << endl;


    cout << "m = " << type_id_with_cvr<decltype(ignoreTup)>().pretty_name() << endl;

    return 0;
}
