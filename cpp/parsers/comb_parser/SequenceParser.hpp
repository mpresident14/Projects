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
    SequenceParser<p_results_filtered_t<PTypes...>, std::decay_t<PTypes>...>
    seq(PTypes&&... parsers);
}


template <typename T, typename... ParserTypes>
class SequenceParser: public Parser<T> {

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

    template <typename... PTypes>
    friend SequenceParser<parsers::p_results_filtered_t<PTypes...>, std::decay_t<PTypes>...>
    parsers::seq(PTypes&&... parsers);


private:
    SequenceParser(const ParserTypes&... parsers)
        : parsers_(std::tuple<ParserTypes...>(parsers...)) {}

    SequenceParser(ParserTypes&&... parsers)
        : parsers_(std::tuple<ParserTypes...>(std::move(parsers)...)) {}


    virtual std::optional<T> apply(std::istream& input) const override
    {
        size_t oldPos = input.tellg();
        std::optional<T> optResult = applyHelper<0>(input, std::tuple<>());
        if (optResult.has_value()) {
            return optResult;
        }
        // TODO: optResult will leak upon condFn failure if T is a pointer

        // Reset position if parse failed
        input.clear();
        input.seekg(oldPos);
        return optResult;
    }


    /* Recursive tuple iteration via templates */
    // Base case
    template <int I, std::enable_if_t<I == sizeof...(ParserTypes), int> = 0>
    std::optional<T> applyHelper(std::istream&, T&& tup) const
    {
        return std::optional(tup);
    }


    // If not an ignore_t (should go in result tuple)
    template <
        int I,
        typename Tup,
        std::enable_if_t<
            // conjunction short-circuits, i.e. doesn't instantiate ::value
            // if it doesn't have to, so we hide the tuple_element_t request
            // in the ::value member of is_ignore.
            std::conjunction_v<
                std::bool_constant<I != sizeof...(ParserTypes)>,
                std::negation<
                    parsers::is_ignore<I, std::tuple<ParserTypes...>>
                >
            >,
            int
        > = 0
    >
    std::optional<T> applyHelper(std::istream& input, Tup&& currentTuple) const
    {
        auto optResult = std::get<I>(parsers_).apply(input);
        if (optResult.has_value()) {
            return applyHelper<I+1>(
                input,
                std::tuple_cat(
                    currentTuple,
                    std::tuple(std::move(optResult.value()))));
        }
        return {};
    }


    // If an ignore_t (should not go in result tuple)
    template <
        int I,
        typename Tup,
        std::enable_if_t<
            std::conjunction_v<
                std::bool_constant<I != sizeof...(ParserTypes)>,
                parsers::is_ignore<I, std::tuple<ParserTypes...>>
            >,
            int
        > = 0
    >
    std::optional<T> applyHelper(std::istream& input, Tup&& currentTuple) const
    {
        auto optResult = std::get<I>(parsers_).apply(input);
        if (optResult.has_value()) {
            return applyHelper<I+1>(
                input,
                std::forward<Tup>(currentTuple));
        }
        return {};
    }


    std::tuple<ParserTypes...> parsers_;
};


#endif
