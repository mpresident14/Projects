#include "parser.hpp"
#include "widget.hpp"

#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <unordered_set>

using namespace std;
using namespace parsers;

class Regex {
public:
    virtual ~Regex(){};
    virtual string toString() = 0;
    // TODO: Replace with string_view
    virtual vector<pair<string, string>> getMatches(const string&) = 0;
    bool matches(const string& str)
    {
        return !getMatches(str).empty();
    }
};

class Dot : public Regex {
public:
    Dot() {}
    string toString() override { return "DOT"; }
    vector<pair<string, string>> getMatches(const string& str) override
    {
        if (str.empty()) {
            return {};
        }
        return {make_pair(str.substr(0, 1), str.substr(1))};
    }
};

class Character : public Regex {
public:
    Character(char c) : c_{c} {}
    string toString() override { return "CHAR " + string(1, c_); }
    vector<pair<string, string>> getMatches(const string& str) override
    {
        if (str.empty() || str[0] != c_) {
            return {};
        }
        return {make_pair(str.substr(0, 1), str.substr(1))};
    }
    char c_;
};

class Concat : public Regex {
public:
    Concat(Regex *rgx1, Regex *rgx2) : rgx1_{rgx1}, rgx2_{rgx2} {}
    ~Concat() { delete rgx1_; delete rgx2_; }
    string toString() override { return "CONCAT (" + rgx1_->toString() + ", " + rgx2_->toString() + ")"; }
    vector<pair<string, string>> getMatches(const string& str) override
    {
        if (str.empty()) {
            return {};
        }
        return {make_pair(str.substr(0, 1), str.substr(1))};
    }
    Regex *rgx1_;
    Regex *rgx2_;
};

class Star : public Regex {
public:
    Star(Regex *rgx) : rgx_{rgx} {}
    ~Star() { delete rgx_; }
    string toString() override { return "STAR (" + rgx_->toString() + ")"; }
    vector<pair<string, string>> getMatches(const string& str) override
    {
        Alt syn(new Concat(rgx_, this), new Epsilon());
        return syn.getMatches(str);
    }
    Regex *rgx_;
};

class Alt : public Regex {
public:
    Alt(Regex *rgx1, Regex *rgx2) : rgx1_{rgx1}, rgx2_{rgx2} {}
    ~Alt() { delete rgx1_; delete rgx2_; }
    string toString() override { return "ALT (" + rgx1_->toString() + ", " + rgx2_->toString() + ")"; }
    vector<pair<string, string>> getMatches(const string& str) override
    {
        return;
    }
    Regex *rgx1_;
    Regex *rgx2_;
};

class Epsilon : public Regex {
public:
    Epsilon() {}
    string toString() override { return "EPSILON"; }
    vector<pair<string, string>> getMatches(const string& str) override
    {
        return {make_pair("", str)};
    }
};

int main(int argc, char **argv)
{
    if (argc != 2) {
        cerr << "Enter a string to parse." << endl;
        return 1;
    }

    Parser<Regex*> regex = fail<Regex*>;

    Parser<Regex*> dot = thisChar('.').andThenMap([](char) -> Regex* { return new Dot(); });

    unordered_set<char> specialChars = {'(', ')', '|', '*'};
    Parser<Regex*> character = 
        anyChar
        .verify([&specialChars](char c) { return !specialChars.count(c); })
        .andThenMap([](char c) -> Regex* { return new Character(c); });

    Parser<Regex*> group = thisChar('(').ignoreAndThenRef(regex).thenIgnore(thisChar(')'));

    Parser<Regex*> initRgx = dot.alt(group).alt(character);

    Parser<Regex*> star =
        initRgx.thenIgnore(thisChar('*')).andThenMap(
            [](auto&& rgx) -> Regex* { return new Star(rgx); });

    Parser<Regex*> initRgxWithStar = star.alt(initRgx);

    Parser<Regex*> concat =
        initRgxWithStar.combineRef(regex).andThenMap(
            [](auto&& rgxPair) -> Regex* { return new Concat(rgxPair.first, rgxPair.second); });

    Parser<Regex*> alternative =
        initRgxWithStar.thenIgnore(thisChar('|')).combineRef(regex).andThenMap(
            [](auto&& rgxPair) -> Regex* { return new Alt(rgxPair.first, rgxPair.second); });

    regex = concat.alt(alternative).alt(initRgxWithStar);

    try {
        auto result = regex.parse(argv[1]);
        cout << result->toString() << endl;
        delete result;
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
}
