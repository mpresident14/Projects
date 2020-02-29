#ifndef SEQUENCE_PARSER_HPP
#define SEQUENCE_PARSER_HPP

#include "Parser.hpp"

#include <utility>
#include <tuple>

template <typename T, typename... PTypes>
class SequenceParser;

namespace parsers
{
    template <typename... PTypes>
    SequenceParser<parsers::p_tuple_results_t<PTypes...>, PTypes...>
    seq(PTypes&&... parsers);
}


template <typename T, typename... ParserTypes>
class SequenceParser: public Parser<T, SequenceParser<T, ParserTypes...>> {

    template<typename T2, typename F2, typename P2>
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
    friend SequenceParser<parsers::p_tuple_results_t<PTypes...>, PTypes...>
    parsers::seq(PTypes&&... parsers);


private:
    SequenceParser(ParserTypes&&... parsers)
        : parsers_(std::tuple<ParserTypes...>(std::forward<ParserTypes>(parsers)...)) {}


    virtual std::optional<T> apply(const std::string& input, size_t *pos) const override
    {
        size_t oldPos = *pos;
        std::optional<T> optResult = applyHelper<0>(input, pos, std::tuple<>());
        if (optResult.has_value()) {
            return optResult;
        }

        // Reset position if parse failed
        *pos = oldPos;
        return optResult;
    }


    // Recursive tuple iteration via templates
    template <int I, std::enable_if_t<I == sizeof...(ParserTypes), int> = 0>
    std::optional<T> applyHelper(const std::string&, size_t *, T&& tup) const
    {
        return std::make_optional(tup);
    }

    template <int I, typename Tup, std::enable_if_t<I != sizeof...(ParserTypes), int> = 0>
    std::optional<T> applyHelper(const std::string& input, size_t *pos, Tup&& currentTuple) const
    {
        auto optResult = std::get<I>(parsers_).apply(input, pos);
        if (optResult.has_value()) {
            return applyHelper<I+1>(
                input,
                pos,
                std::tuple_cat(
                    currentTuple,
                    std::make_tuple(std::move(optResult.value()))));
        }
        return {};
    }


    std::tuple<ParserTypes...> parsers_;
};


#endif
