#ifndef MAP_PARSER_HPP
#define MAP_PARSER_HPP

#include "Parser.hpp"

#include <utility>

template <typename T, typename From, typename F, typename P>
class MapParser: public Parser<T, MapParser<T, From, F, P>> {

    template<typename T2, typename F2, typename P2>
    friend class ConditionalParser;

    template <typename T2, typename From2, typename F2, typename P2>
    friend class MapParser;

    template <typename T2, typename Tuple>
    friend class AltParser;

    template <typename T2, typename Tuple2>
    friend class SequenceParser;

    template<typename T2, typename Derived>
    friend class Parser;

private:
    // Note that for member functions, the typename P has already been deduced at the
    // class level, so P&& is not a universal reference.
    MapParser(const P& parser, F&& mapFn)
        : parser_(parser), mapFn_(std::move(mapFn)) {}

    MapParser(P&& parser, F&& mapFn)
        : parser_(std::move(parser)), mapFn_(std::move(mapFn)) {}


    virtual std::optional<T> apply(const std::string& input, size_t *pos) const override
    {
        std::optional<From> optResult = parser_.apply(input, pos);
        if (optResult.has_value()) {
            return std::make_optional<T>(mapFn_(std::move(optResult.value())));
        }
        return {};
    }

private:
    P parser_;
    F mapFn_;
};

#endif
