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


// void f(Parser<char> p) {
//   cout << "hi" << endl;
// }

int main() {
  Parser<char> p1 = fail<char>();
  auto p2 = thisChar('a').thenIgnore(p1).alt(thisChar('b'));
  p1.set(move(p2));
  // f(p1);


  try {
    p1.parse("abb");
  } catch (invalid_argument& e) {
    cerr << e.what() << endl;
  }
}
