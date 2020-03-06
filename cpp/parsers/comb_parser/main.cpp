#include "Parsers.hpp"

#include <iostream>
#include <sstream>
#include <string>

#include <bits/stdc++.h>

#include <prez/print_stuff.hpp>

#include <boost/type_index.hpp>

using namespace std;
using namespace parsers;
using namespace prez;

int main() {
  auto manyNumbers = ignoreAndThen(anyULong, anyChar());
  return 0;
}
