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
    cout << anyULong.parse("1") << endl;
    cout << anyULong.parse(" 1") << endl;
    cout << anyULong.parse(" 1000") << endl;

    auto manyNumbers = many(anyULong);
    printContainer(manyNumbers.parse("1 100 1000"));

    return 0;
}
