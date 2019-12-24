#include "parser.hpp"
#include "widget.hpp"

#include <iostream>
#include <optional>
#include <string>
#include <stdexcept>
#include <list>
#include <cctype>

using namespace std;
using namespace parsers;

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


int main(int argc, char **argv)
{
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
        .many();
        // .combine(widgetParser)
        // .andThenMap(
        //     [](pair<vector<Widget>, Widget>&& myPair) {
        //         vector<Widget>& widgets = get<0>(myPair);
        //         list<Widget> myList{widgets.begin(), widgets.end()};
        //         myList.push_back(get<1>(myPair));
        //         return myList;
        //     }
        // );

    
    // try {
    //     auto widgets = linkedWidgetParser.parse(argv[1]);
    //     printElements(widgets);
    // } catch (invalid_argument& e) {
    //     cerr << e.what() << endl;
    // }

    try {
        auto widgets = linkedWidgetParser.parse(argv[1]);
        // cout << widgets << endl;
        printList(widgets);
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }


    // try {
    //     auto p = 
    //         thisChar('b')
    //         .alt(thisChar('a'))
    //         .many()
    //         .thenIgnore(anyInt.many());
    //     auto result = p.parse(argv[1]);
    //     cout << result << endl;
    //     // printList(result);
    // } catch (invalid_argument& e) {
    //     cerr << e.what() << endl;
    // }
}
