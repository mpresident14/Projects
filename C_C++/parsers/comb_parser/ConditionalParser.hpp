#ifndef CONDITIONAL_PARSER_HPP
#define CONDITIONAL_PARSER_HPP

#include "Parser.hpp"

#include <utility>
#include <iostream>

template <typename T, typename P>
class ConditionalParser: public Parser<T, ConditionalParser<T, P>> {
public:
    template<typename T2, typename R2, typename P2>
    friend class MapParser;
    template<typename T2, typename P2>
    friend class ConditionalParser;
    template<typename T2, typename Derived>
    friend class Parser;

private:
    ConditionalParser(const P& parser, bool (*condFn)(const T&))
        : parser_(parser), condFn_(condFn) {std::cout << "const &" << std::endl;}

    ConditionalParser(P&& parser, bool (*condFn)(const T&))
        : parser_(std::move(parser)), condFn_(condFn) { std::cout << "&&" << std::endl;}

    virtual std::optional<T> apply(std::string_view input, size_t *pos) override
    {
        std::optional<T> optResult = parser_.apply(input, pos);
        if (optResult.has_value() && condFn_(optResult.value())) {
            return optResult;
        }
        return {};
    }

    P parser_;
    bool (*condFn_)(const T&);
};

#endif
