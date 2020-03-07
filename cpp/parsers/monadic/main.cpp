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

int main(int, char** argv) {

  // try {
  //   thisChar('a').parse("b");
  // } catch (invalid_argument& e) {
  //   cerr << e.what() << endl;
  // }

  // try {
  //   thisChar('b').some().alt(skipws(thisChar('a').many())).parse(" aaaaaaaaaa");
  // } catch (invalid_argument& e) {
  //   cerr << e.what() << endl;
  // }

  // try {
  //   thisChar('a').combine(thisChar('b').many()).parse("abbbbbb");
  // } catch (invalid_argument& e) {
  //   cerr << e.what() << endl;
  // }

  const auto intParser = skipws(anyInt).thenIgnore(skipws(thisChar(',')));


  try {
    skipws(thisChar('{'))
    .ignoreAndThen(intParser.many())
    .combine(anyChar)
    .parse(argv[1]);
  } catch (invalid_argument& e) {
    cerr << e.what() << endl;
  }


  // try {
  //   thisChar('a').thenIgnore(thisChar('b').many()).parse("abbbbbbbbbbbbb");
  // } catch (invalid_argument& e) {
  //   cerr << e.what() << endl;
  // }

  // try {
  //   thisChar('a').alt(thisChar('b')).parse("c");
  // } catch (invalid_argument& e) {
  //   cerr << e.what() << endl;
  // }
}
