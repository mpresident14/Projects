#ifndef MANY_PARSER_HPP
#define MANY_PARSER_HPP

#include "Parser.hpp"

#include <vector>

template <typename T, typename P>
class ManyParser;

namespace parsers
{
    template <typename P>
    ManyParser<std::vector<parsers::p_result_t<P>>, std::decay_t<P>> many(P&& parser);
}


template <typename T, typename P>
class ManyParser: public Parser<T, ManyParser<T, P>> {

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

    template <typename P2>
    friend ManyParser<std::vector<parsers::p_result_t<P2>>, std::decay_t<P2>> parsers::many(P2&& parser);

private:
    ManyParser(const P& parser) : parser_(parser) {}
    ManyParser(P&& parser) : parser_(std::move(parser)) {}

    virtual std::optional<T> apply(std::istream& input) const override
    {
        using PType = parsers::p_result_t<P>;

        std::vector<PType> v;
        std::optional<PType> optResult = parser_.apply(input);
        while (optResult.has_value()) {
            v.push_back(std::move(optResult.value()));
            optResult = parser_.apply(input);
        }

        return std::optional(v);
    }

    P parser_;
};

#endif
