// #include "parser.hpp"
// #include "widget.hpp"
#include "simple_shptr.hpp"

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
// using namespace parsers;
using namespace std::chrono;
using namespace boost::typeindex;


int main() {
  SimpleShPtr<int> s = new int(3);
}
