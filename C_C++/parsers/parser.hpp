#ifndef PARSER_HPP
#define PARSER_HPP

#include "movable_fn.hpp"

#include <string>
#include <optional>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <utility>

template<typename T>
class Parser {
public:
    using return_t   = std::optional<std::pair<T, std::string>>;
    using input_t    = const std::string;
    using parse_fn_t = MovableFn<return_t, input_t&>;
    
    Parser(const parse_fn_t&);
    Parser(parse_fn_t&&);
    Parser(const T&);
    Parser(T&&);
    ~Parser() = default;
    Parser(const Parser&) = default;
    Parser(Parser&&) = default;
    Parser& operator=(const Parser&) = default;
    Parser& operator=(Parser&&) = default;

    /* Combinators */
    template<typename R>
    Parser<R> andThen(MovableFn<Parser<R>, T&&>&) const;

    template<typename R>
    Parser<std::pair<T,R>> combine(const Parser<R>& nextParser) const;

    T parse(input_t&) const;

// private:
    parse_fn_t parseFn_;
};

#include "parser_impl.hpp"

#endif