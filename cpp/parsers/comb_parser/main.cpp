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

    auto abcSeq = parsers::seq(a,move(b),c);
    auto acSeq = parsers::seq(a,a,c);
    auto tupResult = parsers::alt(acSeq, abcSeq).parse("abc");
    char ca;
    char cb;
    char cc;
    tie(ca, cb, cc) = tupResult;
    cout << ca << ", " << cb << ", " << cc << endl;

    auto manyA = parsers::many(a);
    auto v = manyA.parse("aaaaaaaaaaaaaaaaaaaa");
    cout << v.size() << endl;


    cout << "m = " << type_id_with_cvr<parsers::p_tuple_results_t<decltype(a), decltype(b)>>().pretty_name() << endl;

    return 0;
}
