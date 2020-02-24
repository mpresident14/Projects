#ifndef MAP_PARSER_HPP
#define MAP_PARSER_HPP

#include "Parser.hpp"

#include <utility>

template <typename T, typename From, typename P>
class MapParser: public Parser<T, MapParser<T, From, P>> {
    template<typename T2, typename P2>
    friend class ConditionalParser;
    template <typename T2, typename From2, typename P2>
    friend class MapParser;
    template<typename T2, typename Derived>
    friend class Parser;

private:
    MapParser(const P& parser, T (*mapFn)(From&&))
        : parser_(parser), mapFn_(mapFn) {}

    MapParser(P&& parser, T (*mapFn)(From&&))
        : parser_(std::move(parser)), mapFn_(mapFn) {}

    virtual std::optional<T> apply(std::string_view input, size_t *pos) override
    {
        std::optional<From> optResult = parser_.apply(input, pos);
        if (optResult.has_value()) {
            return std::make_optional<T>(mapFn_(std::move(optResult.value())));
        }
        return {};
    }

private:
    P parser_;
    T (*mapFn_)(From&&);
};

#endif
