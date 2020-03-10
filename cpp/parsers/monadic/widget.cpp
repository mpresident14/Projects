#include "widget.hpp"
#include "parser.hpp"

#include <cctype>
#include <chrono>
#include <ctime>
#include <iostream>
#include <list>
#include <optional>
#include <stdexcept>
#include <string>

#include <prez/print_stuff.hpp>

using namespace std;
using namespace parsers;
using namespace std::chrono;

double div2(int n) { return n / 2.0; }

template <typename T>
void printList(vector<T> myList) {
  for (T& item : myList) {
    cout << item << " ";
  }
  cout << endl;
}

template <typename T>
void printElements(list<T> myList) {
  for (T& item : myList) {
    cout << item << endl;
  }
}

// "{1 ,2,3}->{4,5,6}->{4,334,-2}-> { -3,-4544,5,6,4,6,4, -73} ->
// {4,233,235,23,-23} -> {4543,242, -525  ,  -463, -234 }->  { 53, 2, 45, -2}"
int main(int argc, char** argv) {
  if (argc != 2) {
    cerr << "Enter a string to parse." << endl;
    return 1;
  }

  const auto arrowParser = skipws(thisChar('-')).thenIgnore(thisChar('>'));
  const auto intParser = skipws(anyInt).thenIgnore(skipws(thisChar(',')));

  const auto widgetParser =
      skipws(thisChar('{'))
          .ignoreAndThen(intParser.many())
          .combine(skipws(anyInt))
          .thenIgnore(skipws(thisChar('}')))
          .thenIgnore(whitespace)
          .transform([](pair<vector<int>, int>&& myPair) {
            Widget w;
            vector<int>& nums = get<0>(myPair);
            for_each(nums.begin(), nums.end(), [&w](int& n) { w.store(n); });
            w.store(get<1>(myPair));
            return w;
          });

  auto linkedWidgetParser = widgetParser.thenIgnore(arrowParser)
                                .many()
                                .combine(widgetParser)
                                .transform([](pair<vector<Widget>, Widget>&& myPair) {
                                  vector<Widget>& widgets = get<0>(myPair);
                                  list<Widget> myList{widgets.begin(), widgets.end()};
                                  myList.push_back(get<1>(myPair));
                                  return myList;
                                });

  try {
    auto widgets = linkedWidgetParser.parse(argv[1]);
    prez::printContainer(widgets);
    printElements(widgets);
  } catch (invalid_argument& e) {
    cerr << e.what() << endl;
  }

  vector<int> v = {1,3,4,5,6,6};
  prez::printContainer(v);
}
