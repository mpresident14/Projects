#ifndef REGEX_HPP
#define REGEX_HPP

#include "parser.hpp"

#include <string>
#include <memory>


class Regex {
public:
    static std::unique_ptr<Regex> parseRegex(const std::string&);

    // Needs body because destructor of base class is also called.
    // https://www.geeksforgeeks.org/pure-virtual-destructor-c/
    virtual ~Regex(){};
    // virtual bool matches(const std::string&) const = 0;

    virtual void print() const = 0;

    static Parser<std::unique_ptr<Regex>> kRegexParser;
    

// private:
    static Parser<std::unique_ptr<Regex>> kDotParser;
    static Parser<std::unique_ptr<Regex>> kCharParser;
    static Parser<std::unique_ptr<Regex>> kConcatParser;
    static Parser<std::unique_ptr<Regex>> kStarParser;
};

class Concat : public Regex {
public:
    Concat(std::unique_ptr<Regex> rgx1, std::unique_ptr<Regex>rgx2);
    // bool matches(const std::string&) const override;

private:
    void print() const override;
    std::unique_ptr<Regex> rgx1_;
    std::unique_ptr<Regex> rgx2_;
};

// class Star : public Regex {
// public:
//     Star(const std::string&);
//     bool matches(const std::string&) const override;

// private:
//     void print() const override;

//     std::unique_ptr<Regex> rgx_;
// };

class Character : public Regex {
public:
    Character(char);
    // ~Character();
    // bool matches(const std::string&) const override;

private:
    void print() const override;

    char c_;
};

class Dot : public Regex {
public:
    // bool matches(const std::string&) const override;

private:
    void print() const override;
};

#endif
