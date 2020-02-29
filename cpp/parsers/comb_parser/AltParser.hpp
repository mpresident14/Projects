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
    AltParser<parsers::p_first_t<ParserTypes...>, ParserTypes...>
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
    friend AltParser<parsers::p_first_t<PTypes...>, PTypes...>
    parsers::alt(PTypes&&... parsers);


private:
    AltParser(ParserTypes&&... parsers)
        : parsers_(std::tuple<ParserTypes...>(std::forward<ParserTypes>(parsers)...)) {}


    virtual std::optional<T> apply(const std::string& input, size_t *pos) const override
    {
        return applyHelper<0>(input, pos);
    }


    // Recursive tuple iteration via templates
    template <int I, std::enable_if_t<I == sizeof...(ParserTypes), int> = 0>
    std::optional<T> applyHelper(const std::string&, size_t *) const
    {
        return {};
    }

    template <int I, std::enable_if_t<I != sizeof...(ParserTypes), int> = 0>
    std::optional<T> applyHelper(const std::string& input, size_t *pos) const
    {
        std::optional<T> optResult = std::get<I>(parsers_).apply(input, pos);
        if (optResult.has_value()) {
            return optResult;
        }
        return applyHelper<I+1>(input, pos);
    }


    std::tuple<ParserTypes...> parsers_;
};


#endif
