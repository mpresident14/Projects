#ifndef PARSER_BASE_HPP
#define PARSER_BASE_HPP

#include <istream>
#include <optional>

#include <iostream>

template <typename T>
class LazyParser;

/* *
 * Parser abstract basest class
 * (mainly so LazyParser does not have to know the Derived type of
 * the parser it's holding)
 * */
template <typename T>
class ParserBase {
    template <typename T2>
    friend class LazyParser;

public:
    virtual ~ParserBase(){}

protected:
    virtual std::optional<T> apply(std::istream& input) const = 0;
};

#endif
