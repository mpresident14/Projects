#include "parser.hpp"
#include "widget.hpp"

#include <iostream>
#include <optional>
#include <string>
#include <stdexcept>
#include <list>
#include <cctype>
#include <ctime>
#include <chrono>

using namespace std;
using namespace parsers;
using namespace std::chrono;

double div2(int n)
{
    return n / 2.0;
}

template<typename T>
void printList(vector<T> myList)
{
    for (T& item : myList) {
        cout << item << " ";
    }
    cout << endl;
}

template<typename T>
void printElements(list<T> myList)
{
    for (T& item : myList) {
        cout << item << endl;
    }
}

// "{1 ,2,3}->{4,5,6}->{4,334,-2}-> { -3,-4544,5,6,4,6,4, -73} ->    {4,233,235,23,-23} -> {4543,242, -525  ,  -463, -234 }->  { 53, 2, 45, -2}"
int main(int argc, char **argv)
{
    if (argc != 2) {
        cerr << "Enter a string to parse." << endl;
        return 1;
    }

    const auto arrowParser = 
        skipws(thisChar('-'))
        .thenIgnore(thisChar('>'));

    const auto intParser = 
        skipws(anyInt)
        .thenIgnore(skipws(thisChar(',')));
    
    const auto widgetParser = 
        skipws(thisChar('{'))
        .ignoreAndThen(intParser.many())
        .combine(skipws(anyInt))
        .thenIgnore(skipws(thisChar('}')))
        .thenIgnore(whitespace)
        .andThenMap(
            [](pair<vector<int>, int>&& myPair) {
                Widget w;
                vector<int>& nums = get<0>(myPair);
                for_each(nums.begin(), nums.end(), [&w](int &n){ w.store(n); });
                w.store(get<1>(myPair));
                return w;
            }
        );

    auto linkedWidgetParser = 
        widgetParser
        .thenIgnore(arrowParser)
        .many()
        .combine(widgetParser)
        .andThenMap(
            [](pair<vector<Widget>, Widget>&& myPair) {
                vector<Widget>& widgets = get<0>(myPair);
                list<Widget> myList{widgets.begin(), widgets.end()};
                myList.push_back(get<1>(myPair));
                return myList;
            }
        );

    auto start = clock();
    auto start_chr = high_resolution_clock::now();

    for (int i = 0; i < 1; ++i) {
        try {
            auto widgets = linkedWidgetParser.parse(argv[1]);
            printElements(widgets);
        } catch (invalid_argument& e) {
            cerr << e.what() << endl;
        }
    }

    auto stop = clock();
    auto stop_chr = high_resolution_clock::now();

    cout << "Chrono: Took " << duration<double, milli>(stop_chr - start_chr).count() / 1000 << " seconds" << endl;
    cout << "Clock: Took " << (stop - start) * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
}
