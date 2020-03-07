#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

class Regex;
using RgxPtr = std::unique_ptr<Regex>;

class Regex {
public:
  virtual ~Regex(){};
  // Virtual constructor idiom. See
  // https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Virtual_Constructor
  virtual RgxPtr clone() const = 0;
  virtual std::string toString() = 0;
  bool matches(const std::string& str) {
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
  // Returns a std::vector of pairs. Each pair consists of the match of the
  // regex and the characters remaining in the input std::string.
  virtual std::vector<std::string_view> getMatches(std::string_view) = 0;
};

class Dot : public Regex {
public:
  Dot() {}
  RgxPtr clone() const override { return std::make_unique<Dot>(*this); }
  std::string toString() override { return "DOT"; }

private:
  std::vector<std::string_view> getMatches(std::string_view str) override {
    if (str.empty()) {
      return {};
    }
    return {str.substr(1)};
  }
};

class Character : public Regex {
public:
  Character(char c) : c_{c} {}
  RgxPtr clone() const override { return std::make_unique<Character>(*this); }
  std::string toString() override { return "CHAR " + std::string(1, c_); }

private:
  std::vector<std::string_view> getMatches(std::string_view str) override {
    if (str.empty() || str[0] != c_) {
      return {};
    }
    return {str.substr(1)};
  }
  char c_;
};

class Concat : public Regex {
public:
  Concat(RgxPtr rgx1, RgxPtr rgx2) : rgx1_{std::move(rgx1)}, rgx2_{std::move(rgx2)} {}
  RgxPtr clone() const override {
    return std::make_unique<Concat>(rgx1_->clone(), rgx2_->clone());
  }
  std::string toString() override {
    return "CONCAT (" + rgx1_->toString() + ", " + rgx2_->toString() + ")";
  }

private:
  std::vector<std::string_view> getMatches(std::string_view str) override {
    std::vector<std::string_view> allMatches;
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
  Alt(RgxPtr rgx1, RgxPtr rgx2) : rgx1_{std::move(rgx1)}, rgx2_{std::move(rgx2)} {}
  RgxPtr clone() const override {
    return std::make_unique<Alt>(rgx1_->clone(), rgx2_->clone());
  }
  std::string toString() override {
    return "ALT (" + rgx1_->toString() + ", " + rgx2_->toString() + ")";
  }

private:
  std::vector<std::string_view> getMatches(std::string_view str) override {
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
  std::string toString() override { return "EPSILON"; }
  RgxPtr clone() const override { return std::make_unique<Epsilon>(*this); }

private:
  std::vector<std::string_view> getMatches(std::string_view str) override {
    return {str};
  }
};

class Star : public Regex {
public:
  Star(RgxPtr rgx) : rgx_{std::move(rgx)} {}
  RgxPtr clone() const override { return std::make_unique<Star>(rgx_->clone()); }
  std::string toString() override { return "STAR (" + rgx_->toString() + ")"; }

private:
  std::vector<std::string_view> getMatches(std::string_view str) override {
    // "Star re" is equivalent to "Alt (Concat (re, Star re), Epsilon)"
    Alt equivRgx(
        std::make_unique<Concat>(rgx_->clone(), std::make_unique<Star>(rgx_->clone())),
        std::make_unique<Epsilon>());

    Regex* equivRgxPtr = &equivRgx;  // So we can access private member function
    return equivRgxPtr->getMatches(str);
  }
  RgxPtr rgx_;
};
