#ifndef ALT_PARSER_HPP
#define ALT_PARSER_HPP

#include "Parser.hpp"

#include <utility>
#include <tuple>

template <typename T2, typename... PTypes>
class AltParser;

namespace parsers
{
    template <typename... ParserTypes>
    AltParser<parsers::p_first_t<ParserTypes...>, std::decay_t<ParserTypes>...>
    alt(ParserTypes&&... parsers);
}


template <typename T, typename... ParserTypes>
class AltParser: public Parser<T, AltParser<T, ParserTypes...>> {

    template<typename T2, typename F, typename P2>
    friend class ConditionalParser;

    template <typename T2, typename F2, typename P2>
    friend class MapParser;

    template <typename T2, typename... PTypes>
    friend class AltParser;

    template <typename T2, typename... PTypes>
    friend class SequenceParser;

    template<typename T2, typename Derived>
    friend class Parser;

    template <typename... PTypes>
    friend AltParser<parsers::p_first_t<PTypes...>, std::decay_t<PTypes>...>
    parsers::alt(PTypes&&... parsers);


private:
    AltParser(const ParserTypes&... parsers)
        : parsers_(std::tuple<ParserTypes...>(parsers...)) {}

    AltParser(ParserTypes&&... parsers)
        : parsers_(std::tuple<ParserTypes...>(std::forward<ParserTypes>(parsers)...)) {}


    virtual std::optional<T> apply(std::istream& input) const override
    {
        return applyHelper<0>(input);
    }


    // Recursive tuple iteration via templates
    template <int I, std::enable_if_t<I == sizeof...(ParserTypes), int> = 0>
    std::optional<T> applyHelper(std::istream&) const
    {
        return {};
    }

    template <int I, std::enable_if_t<I != sizeof...(ParserTypes), int> = 0>
    std::optional<T> applyHelper(std::istream& input) const
    {
        std::optional<T> optResult = std::get<I>(parsers_).apply(input);
        if (optResult.has_value()) {
            return optResult;
        }
        return applyHelper<I+1>(input);
    }


    std::tuple<ParserTypes...> parsers_;
};


#endif
