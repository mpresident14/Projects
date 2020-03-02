#ifndef MAP_PARSER_HPP
#define MAP_PARSER_HPP

#include "Parser.hpp"

#include <utility>

template <typename T, typename F, typename P>
class MapParser: public Parser<T, MapParser<T, F, P>> {

    template<typename T2, typename F2, typename P2>
    friend class ConditionalParser;

    template <typename T2, typename F2, typename P2>
    friend class MapParser;

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

private:
    // Note that for member functions, the typename P has already been deduced at the
    // class level, so P&& is not a universal reference.
    MapParser(const P& parser, F&& mapFn)
        : parser_(parser), mapFn_(std::move(mapFn)) {}

    MapParser(P&& parser, F&& mapFn)
        : parser_(std::move(parser)), mapFn_(std::move(mapFn)) {}


    virtual std::optional<T> apply(std::istream& input) const override
    {
        auto optResult = parser_.apply(input);
        if (optResult.has_value()) {
            return std::optional<T>(mapFn_(std::move(optResult.value())));
        }
        return {};
    }

private:
    P parser_;
    F mapFn_;
};

#endif
