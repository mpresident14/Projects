#ifndef PARSER_BASE_HPP
#define PARSER_BASE_HPP

#include <string>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

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
    virtual T parse(const std::string& input) const = 0;

protected:
    virtual std::optional<T> apply(const std::string& input, size_t *pos) const = 0;
};

#endif
