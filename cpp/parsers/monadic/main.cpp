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

#include <boost/type_index.hpp>

#include <prez/print_stuff.hpp>

using namespace std;
using namespace parsers;
using namespace std::chrono;
using namespace boost::typeindex;

void f(Parser<char> p) { cout << type_id_with_cvr<decltype(p)>().pretty_name() << endl; }

int main() {
  vector<int> v = {1, 2, 3};

  Parser<char> p1 = fail<char>();
  auto p2 = thisChar('a').thenIgnore(p1).alt(thisChar('b'));
  p1.set(move(p2));
  f(p1);
  auto p3 = anyChar.combine(anyChar, thisString("hello").thenIgnore(thisString("goodbye")), anyDouble);

  try {
    prez::printTuple(p3.parse("yzhellogoodbye2.3"));
  } catch (invalid_argument& e) {
    cerr << e.what() << endl;
  }
}
