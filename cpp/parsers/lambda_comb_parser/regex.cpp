#include "parser.hpp"
#include "widget.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <unordered_set>

#include <prez/timeit.hpp>

using namespace std;
using namespace parsers;
using namespace prez;

class Regex;
using RgxPtr = unique_ptr<Regex>;

class Regex {
public:
    virtual ~Regex(){};
    // Virtual constructor idiom. See https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Virtual_Constructor
    virtual RgxPtr clone () const = 0;
    virtual string toString() = 0;
    bool matches(const string& str)
    {
        for (auto& rest : getMatches(str)) {
            if (rest.empty()) {
                return true;
            }
        }
        return false;
    }

    friend class Alt;
    friend class Concat;
    friend class Star;
private:
    // Returns a vector of pairs. Each pair consists of the match of the regex and the characters
    // remaining in the input string.
    virtual vector<string_view> getMatches(string_view) = 0;
};

class Dot : public Regex {
public:
    Dot() {}
    RgxPtr clone () const override { return make_unique<Dot>(*this); }
    string toString() override { return "DOT"; }
private:
    vector<string_view> getMatches(string_view str) override
    {
        if (str.empty()) {
            return {};
        }
        return {str.substr(1)};
    }
};

class Character : public Regex {
public:
    Character(char c) : c_{c} {}
    RgxPtr clone () const override { return make_unique<Character>(*this); }
    string toString() override { return "CHAR " + string(1, c_); }
private:
    vector<string_view> getMatches(string_view str) override
    {
        if (str.empty() || str[0] != c_) {
            return {};
        }
        return {str.substr(1)};
    }
    char c_;
};

class Concat : public Regex {
public:
    Concat(RgxPtr rgx1, RgxPtr rgx2) : rgx1_{move(rgx1)}, rgx2_{move(rgx2)} {}
    RgxPtr clone () const override { return make_unique<Concat>(rgx1_->clone(), rgx2_->clone()); }
    string toString() override { return "CONCAT (" + rgx1_->toString() + ", " + rgx2_->toString() + ")"; }
private:
    vector<string_view> getMatches(string_view str) override
    {
        vector<string_view> allMatches;
        auto firstMatches = rgx1_->getMatches(str);
        for (auto& rest : firstMatches) {
            for (auto& rest2 : rgx2_->getMatches(rest)) {
                allMatches.push_back(rest2);
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
    RgxPtr clone () const override { return make_unique<Alt>(rgx1_->clone(), rgx2_->clone()); }
    string toString() override { return "ALT (" + rgx1_->toString() + ", " + rgx2_->toString() + ")"; }
private:
    vector<string_view> getMatches(string_view str) override
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
    RgxPtr clone () const override { return make_unique<Epsilon>(*this); }
private:
    vector<string_view> getMatches(string_view str) override
    {
        return {str};
    }
};

class Star : public Regex {
public:
    Star(RgxPtr rgx) : rgx_{move(rgx)} {}
    RgxPtr clone () const override { return make_unique<Star>(rgx_->clone()); }
    string toString() override { return "STAR (" + rgx_->toString() + ")"; }
private:
    vector<string_view> getMatches(string_view str) override
    {
        // "Star re" is equivalent to "Alt (Concat (re, Star re), Epsilon)"
        Alt equivRgx(
            make_unique<Concat>(
                rgx_->clone(),
                make_unique<Star>(rgx_->clone())),
            make_unique<Epsilon>());

        Regex *equivRgxPtr = &equivRgx; // So we can access private member function
        return equivRgxPtr->getMatches(str);
    }
    RgxPtr rgx_;
};

void doParse(const char *rgx)
{
    // if (argc != 3) {
    //     cerr << "Enter a regex to parse and a string to match." << endl;
    //     return 1;
    // }

    /* Regex            := Concat | Alt | InitRgxWithStar
     * Concat           := InitRgxWithStar Regex
     * Alt              := InitRgxWithStar '|' Regex
     * Star             := InitRgx '*'
     * InitRgxWithStar  := Star | InitRgx
     * InitRgx          := Dot | (Regex) | Character
     * Dot              := '.'
     * Character        := char
     */

    Parser<RgxPtr> regex = fail<RgxPtr>;

    Parser<RgxPtr> dot = thisChar('.').andThenMap([](char) -> RgxPtr { return make_unique<Dot>(); });

    unordered_set<char> specialChars = {'(', ')', '|', '*'};
    Parser<RgxPtr> character =
        anyChar
        .verify([&specialChars](char c) { return !specialChars.count(c); })
        .andThenMap([](char c) -> RgxPtr { return make_unique<Character>(c); });

    Parser<RgxPtr> group = thisChar('(').ignoreAndThenRef(regex).thenIgnore(thisChar(')'));

    Parser<RgxPtr> initRgx = dot.alt(group).alt(character);

    Parser<RgxPtr> star =
        initRgx.thenIgnore(thisChar('*')).andThenMap(
            [](auto&& rgx) -> RgxPtr { return make_unique<Star>(move(rgx)); });

    Parser<RgxPtr> initRgxWithStar = star.alt(initRgx);

    Parser<RgxPtr> concat =
        initRgxWithStar.combineRef(regex).andThenMap(
            [](auto&& rgxPair) -> RgxPtr { return make_unique<Concat>(move(rgxPair.first), move(rgxPair.second)); });

    Parser<RgxPtr> alternative =
        initRgxWithStar.thenIgnore(thisChar('|')).combineRef(regex).andThenMap(
            [](auto&& rgxPair) -> RgxPtr { return make_unique<Alt>(move(rgxPair.first), move(rgxPair.second)); });

    regex = concat.alt(alternative).alt(initRgxWithStar);

    try {
    regex.parse(rgx);
    // auto result = regex.parse(rgx);
    // cout << result->toString() << endl;
    // cout << (result->matches(input) ? "Match" : "No Match") << endl;
    } catch (invalid_argument& e) {
        // cerr << e.what() << endl;
    }
}


int main(/* int argc, char** argv */) {
    // if (argc != 3) {
    //   cerr << "Enter a regex to parse and a string to match." << endl;
    //   return 1;
    // }

    size_t trials = 10000;
    size_t t;

    // t = timeit<std::chrono::milliseconds>(trials, []() { doParse("a*b|cd|(ef)*(abc|d)"); });
    // cout << "T1: " << t << " milliseconds." << endl;

    // t = timeit<std::chrono::milliseconds>(trials, []() { doParse("a*b|cd|(ef)*(abc|d)"); });
    // cout << "T2: " << t << " milliseconds." << endl;

    // t = timeit<std::chrono::milliseconds>(trials, []() { doParse("a*b|cd|(ef)*(abc|d)"); });
    // cout << "T3: " << t << " milliseconds." << endl;

    t = timeit<std::chrono::microseconds>(trials, []() { whitespace.parse("\t   \n  "); });
    cout << "T3: " << t << " microseconds." << endl;

}

