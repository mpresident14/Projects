#ifndef PARSER_BASE_HPP
#define PARSER_BASE_HPP

#include <string>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <istream>

#include <iostream>

template <typename T>
class LazyParser;


/* Parser abstract basest class */
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
