// #include "Parsers.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <variant>
#include <bits/stdc++.h>

#include <prez/print_stuff.hpp>

#include <boost/type_index.hpp>

using namespace std;
// using namespace parsers;
using namespace prez;

int main() {
  // auto seqNumbers = seq(anyULong, anyULong, anyULong);

  // try {
  //   seqNumbers.parseAll("123 456 789");
  // } catch (invalid_argument& e) {
  //   cout << e.what() << endl;
  // }

  variant<int, int, int> v(4);
  return 0;
}
