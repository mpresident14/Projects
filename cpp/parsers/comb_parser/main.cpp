#include "Parsers.hpp"

#include <iostream>
#include <string>
#include <sstream>

#include <prez/print_stuff.hpp>

#include <boost/type_index.hpp>


using namespace std;
using namespace parsers;
using namespace prez;

int main()
{
    cout << anyULong.parseAll("1") << endl;
    cout << anyULong.parseAll(" 1") << endl;
    cout << anyULong.parseAll(" 1000") << endl;

    auto manyNumbers = many(anyULong);
    printContainer(manyNumbers.parseAll("1 100 1000"));

    auto seqNumbers = seq(anyULong, anyULong, anyULong);
    auto t = seqNumbers.parseAll("123 456 78");
    printTuple(t);

    cout << sizeof(anyULong) << endl;
    cout << sizeof(seqNumbers) << endl;
    cout << sizeof(anyChar()) << endl;

    return 0;
}
