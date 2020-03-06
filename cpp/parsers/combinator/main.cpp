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
  auto manyNumbers = many(anyULong);

  try {
    manyNumbers.parseAll("1 100 10q0");
  } catch (invalid_argument& e) {
    cout << e.what() << endl;
  }

  auto seqNumbers = seq(anyULong, anyULong, anyULong);

  try {
    seqNumbers.parseAll("123 456 abc");
  } catch (invalid_argument& e) {
    cout << e.what() << endl;
  }

  auto rAssocAdd = lazy<unsigned long>();
  auto plusNum = ignoreAndThen(skipWs(thisChar('+')), rAssocAdd.getRef());
  rAssocAdd.set(transform(
      seq(anyULong,
          transform(
              many(plusNum),
              [](auto&& v) { return accumulate(v.begin(), v.end(), 0); })),
      [](auto&& nums) { return get<0>(nums) + get<1>(nums); }));

  try {
    rAssocAdd.parseAll("1+ 2 + e +4");
  } catch (invalid_argument& e) {
    cout << e.what() << endl;
  }

  auto strP = alt(thisString("had"), thisString("good"), thisString("was"));
  try {
    strP.parseAll("ants");
  } catch (invalid_argument& e) {
    cout << e.what() << endl;
  }

  return 0;
}
