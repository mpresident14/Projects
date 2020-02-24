#ifndef CONDITIONAL_PARSER_HPP
#define CONDITIONAL_PARSER_HPP

#include "Parser.hpp"

#include <utility>
#include <iostream>

template <typename T, typename F, typename P>
class ConditionalParser: public Parser<T, ConditionalParser<T, F, P>> {
public:
    template<typename T2, typename R2, typename P2>
    friend class MapParser;
    template<typename T2, typename F2, typename P2>
    friend class ConditionalParser;
    template<typename T2, typename Derived>
    friend class Parser;

private:
    ConditionalParser(const P& parser, F&& condFn)
        : parser_(parser), condFn_(std::move(condFn)) {}

    ConditionalParser(P&& parser, F&& condFn)
        : parser_(std::move(parser)), condFn_(std::move(condFn)) {}

    virtual std::optional<T> apply(const std::string& input, size_t *pos) const override
    {
        std::optional<T> optResult = parser_.apply(input, pos);
        if (optResult.has_value() && condFn_(optResult.value())) {
            return optResult;
        }
        return {};
    }

    P parser_;
    F condFn_;
};

#endif
