#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <optional>
#include <functional>

template<typename T>
class Parser {
public:
    // std::function allows us to use lambdas with closures as objects.
    Parser(const std::function<std::optional<std::pair<T, std::string>>(const std::string&)>& parseFn);
    Parser(std::function<std::optional<std::pair<T, std::string>>(const std::string&)>&& parseFn);
    Parser(const T& obj);
    ~Parser() = default;
    Parser(const Parser& other) = default;
    Parser(Parser&& other) = default;
    Parser& operator=(const Parser& other) = default;
    Parser& operator=(Parser&& other) = default;

    /* Combinators */
    template<typename R>
    Parser<R> andThen(std::function<Parser<R>(T&& obj)> pGenFn) const;
    template<typename R>
    Parser<std::pair<T,R>> combine(const Parser<R>& next) const;

    T parse(const std::string& input) const;

// private:
    std::function<std::optional<std::pair<T, std::string>>(const std::string&)> parseFn_;
};

#include "parser-impl.hpp"

#endif