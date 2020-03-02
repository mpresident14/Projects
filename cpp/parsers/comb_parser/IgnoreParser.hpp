#ifndef IGNORE_PARSER_HPP
#define IGNORE_PARSER_HPP

#include "Parser.hpp"

template <typename P>
class IgnoreParser;

namespace parsers
{
    template <typename P>
    IgnoreParser<std::decay_t<P>> ignore(P&& parser);
}

template <typename P>
class IgnoreParser: public Parser<parsers::ignore_t, IgnoreParser<P>> {

    template <typename T2, typename F2, typename P2>
    friend class MapParser;

    template<typename T2, typename F2, typename P3>
    friend class ConditionalParser;

    template <typename T2, typename... PTypes>
    friend class AltParser;

    template <typename T2, typename... PTypes>
    friend class SequenceParser;

    template <typename T2, typename P2>
    friend class ManyParser;

    template <typename P2>
    friend class IgnoreParser;

    template<typename T2, typename Derived>
    friend class Parser;

    template <typename P2>
    friend IgnoreParser<std::decay_t<P2>> parsers::ignore(P2&& parser);

private:
    IgnoreParser(const P& parser)
        : parser_(parser) {}

    IgnoreParser(P&& parser)
        : parser_(std::move(parser)) {}


    virtual std::optional<parsers::ignore_t> apply(std::istream& input) const override
    {
        auto optResult = parser_.apply(input);
        if (optResult.has_value()) {
            return std::optional(parsers::ignore_t());
        }

        return {};
    }

    P parser_;
};

#endif
