#ifndef CONDITIONAL_PARSER_HPP
#define CONDITIONAL_PARSER_HPP

#include "Parser.hpp"

template <typename T, typename F, typename P>
class ConditionalParser: public Parser<T, ConditionalParser<T, F, P>> {

    template <typename T2, typename F2, typename P2>
    friend class MapParser;

    template<typename T2, typename F2, typename P2>
    friend class ConditionalParser;

    template <typename T2, typename... PTypes>
    friend class AltParser;

    template <typename T2, typename... PTypes>
    friend class SequenceParser;

    template <typename T2, typename P2>
    friend class ManyParser;

    template<typename T2, typename Derived>
    friend class Parser;

private:
    ConditionalParser(const P& parser, F&& condFn)
        : parser_(parser), condFn_(std::move(condFn)) {}

    ConditionalParser(P&& parser, F&& condFn)
        : parser_(std::move(parser)), condFn_(std::move(condFn)) {}


    virtual std::optional<T> apply(std::istream& input) const override
    {
        size_t oldPos = input.tellg();
        std::optional<T> optResult = parser_.apply(input);
        if (optResult.has_value() && condFn_(optResult.value())) {
            return optResult;
        }

        // TODO: optResult will leak upon condFn failure if T is a pointer

        // Restore the position if the condFn fails.
        input.seekg(oldPos);
        return {};
    }

    P parser_;
    F condFn_;
};

#endif