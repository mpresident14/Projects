#ifndef CONDITIONAL_PARSER_HPP
#define CONDITIONAL_PARSER_HPP

#include "Parser.hpp"

#include <utility>

template <typename T, typename P>
class ConditionalParser: public Parser<T> {
public:
    template<typename T2, typename R2, typename P2>
    friend class MapParser;
    template<typename T2, typename P2>
    friend class ConditionalParser;
    friend class CharParser;
    template<typename T2>
    friend class Parser;

    auto onlyIf(bool (*condFn)(const T&))
    {
        return ConditionalParser<T, std::remove_pointer_t<decltype(this)>>(*this, condFn);
    }

    template<typename To>
    auto mapTo(To (*mapFn)(T&&))
    {
        return MapParser<To, T, std::remove_pointer_t<decltype(this)>>(*this, mapFn);
    }

private:
    ConditionalParser(const P& parser, bool (*condFn)(const T&))
        : parser_(parser), condFn_(condFn) {}

    ConditionalParser(P&& parser, bool (*condFn)(const T&))
        : parser_(std::move<P>(parser)), condFn_(condFn) {}

    virtual std::optional<T> apply(std::string_view input, size_t *pos) override
    {
        std::optional<T> optResult = parser_.apply(input, pos);
        if (optResult.has_value() && condFn_(optResult.value())) {
            return optResult;
        }
        return {};
    }



// private:
    P parser_;
    bool (*condFn_)(const T&);
};

#endif
