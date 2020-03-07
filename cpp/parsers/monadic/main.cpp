#include "parser.hpp"
#include "widget.hpp"

#include <cctype>
#include <chrono>
#include <ctime>
#include <iostream>
#include <list>
#include <optional>
#include <stdexcept>
#include <string>

using namespace std;
using namespace parsers;
using namespace std::chrono;

int main() {
  try {
    anyDigit.combine(thisChar('a')).alt(anyUnsigned.combine(thisChar('b'))).parse("133c");
  } catch (invalid_argument& e) {
    cerr << e.what() << endl;
  }
}
