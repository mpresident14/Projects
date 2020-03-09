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
  Parser<char> p1;
  auto p2 = thisChar('a').thenIgnore(p1).alt(thisChar('b'));
  // p1.set(move(p2));


  try {
    p2.parse("abb");
  } catch (invalid_argument& e) {
    cerr << e.what() << endl;
  }
}
