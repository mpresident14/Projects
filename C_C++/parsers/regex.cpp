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

class Regex;
using RgxPtr = unique_ptr<Regex>;

// TODO: Make more efficient
// TODO: Add comments and regex grammar
// TODO: Clean up this file.
class Regex {
public:
    virtual ~Regex(){};
    // Virtual constructor idiom. See https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Virtual_Constructor
    virtual RgxPtr clone () const = 0;
    virtual string toString() = 0;
    // TODO: Replace with string_view
    virtual vector<pair<string, string>> getMatches(const string&) = 0;
    
    bool matches(const string& str)
    {
        for (auto& matchAndRest : getMatches(str)) {
            if (get<1>(matchAndRest).empty()) {
                cout << "Match: " << get<0>(matchAndRest) << endl;
                return true;
            }
        }
        cout << "No Match" << endl;
        return false;
    }
};

class Dot : public Regex {
public:
    Dot() {}
    RgxPtr clone () const override { return make_unique<Dot>(Dot(*this)); }
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
    RgxPtr clone () const override { return make_unique<Character>(Character(*this)); }
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
    Concat(RgxPtr rgx1, RgxPtr rgx2) : rgx1_{move(rgx1)}, rgx2_{move(rgx2)} {}
    // ~Concat() { delete rgx1_; delete rgx2_; }
    RgxPtr clone () const override { return make_unique<Concat>(Concat(rgx1_->clone(), rgx2_->clone())); }
    string toString() override { return "CONCAT (" + rgx1_->toString() + ", " + rgx2_->toString() + ")"; }
    vector<pair<string, string>> getMatches(const string& str) override
    {
        vector<pair<string, string>> allMatches;
        auto firstMatches = rgx1_->getMatches(str);
        for (auto& firstMatchAndRest : firstMatches) {
            string& match = get<0>(firstMatchAndRest);
            string& rest = get<1>(firstMatchAndRest);
            for (auto& secondMatchAndRest : rgx2_->getMatches(rest)) {
                string& match2 = get<0>(secondMatchAndRest);
                string& rest2 = get<1>(secondMatchAndRest);
                allMatches.push_back(make_pair(match + match2, rest2));
            }
        }
        return allMatches;
    }
    RgxPtr rgx1_;
    RgxPtr rgx2_;
};

class Alt : public Regex {
public:
    Alt(RgxPtr rgx1, RgxPtr rgx2) : rgx1_{move(rgx1)}, rgx2_{move(rgx2)} {}
    // ~Alt() { delete rgx1_; delete rgx2_; }
    RgxPtr clone () const override { return make_unique<Alt>(Alt(rgx1_->clone(), rgx2_->clone())); }
    string toString() override { return "ALT (" + rgx1_->toString() + ", " + rgx2_->toString() + ")"; }
    vector<pair<string, string>> getMatches(const string& str) override
    {
        auto vec1 = rgx1_->getMatches(str);
        auto vec2 = rgx2_->getMatches(str);
        vec1.insert(vec1.end(), vec2.begin(), vec2.end());
        return vec1;
    }
    RgxPtr rgx1_;
    RgxPtr rgx2_;
};

class Epsilon : public Regex {
public:
    Epsilon() {}
    string toString() override { return "EPSILON"; }
    RgxPtr clone () const override { return make_unique<Epsilon>(Epsilon(*this)); }
    vector<pair<string, string>> getMatches(const string& str) override
    {
        return {make_pair("", str)};
    }
};

class Star : public Regex {
public:
    Star(RgxPtr rgx) : rgx_{move(rgx)} {}
    // ~Star() { delete rgx_; }
    RgxPtr clone () const override { return make_unique<Star>(Star(rgx_->clone())); }
    string toString() override { return "STAR (" + rgx_->toString() + ")"; }
    vector<pair<string, string>> getMatches(const string& str) override
    {
        // "Star re" is equivalent to "Alt (Concat (re, Star re), Epsilon)"
        Alt syn(
            make_unique<Concat>(
                Concat(
                    rgx_->clone(), 
                    make_unique<Star>(Star(rgx_->clone())))), 
            make_unique<Epsilon>(Epsilon()));
        return syn.getMatches(str);
    }
    RgxPtr rgx_;
};

int main(int argc, char **argv)
{
    if (argc != 3) {
        cerr << "Enter a regex to parse and a string to match." << endl;
        return 1;
    }

    Parser<RgxPtr> regex = fail<RgxPtr>;

    Parser<RgxPtr> dot = thisChar('.').andThenMap([](char) -> RgxPtr { return make_unique<Dot>(Dot()); });

    unordered_set<char> specialChars = {'(', ')', '|', '*'};
    Parser<RgxPtr> character = 
        anyChar
        .verify([&specialChars](char c) { return !specialChars.count(c); })
        .andThenMap([](char c) -> RgxPtr { return make_unique<Character>(Character(c)); });

    Parser<RgxPtr> group = thisChar('(').ignoreAndThenRef(regex).thenIgnore(thisChar(')'));

    Parser<RgxPtr> initRgx = dot.alt(group).alt(character);

    Parser<RgxPtr> star =
        initRgx.thenIgnore(thisChar('*')).andThenMap(
            [](auto&& rgx) -> RgxPtr { return make_unique<Star>(Star(move(rgx))); });

    Parser<RgxPtr> initRgxWithStar = star.alt(initRgx);

    Parser<RgxPtr> concat =
        initRgxWithStar.combineRef(regex).andThenMap(
            [](auto&& rgxPair) -> RgxPtr { return make_unique<Concat>(Concat(move(rgxPair.first), move(rgxPair.second))); });

    Parser<RgxPtr> alternative =
        initRgxWithStar.thenIgnore(thisChar('|')).combineRef(regex).andThenMap(
            [](auto&& rgxPair) -> RgxPtr { return make_unique<Alt>(Alt(move(rgxPair.first), move(rgxPair.second))); });

    regex = concat.alt(alternative).alt(initRgxWithStar);

    try {
        auto result = regex.parse(argv[1]);
        cout << result->toString() << endl;
        result->matches(argv[2]);
    } catch (invalid_argument& e) {
        cerr << e.what() << endl;
    }
}
