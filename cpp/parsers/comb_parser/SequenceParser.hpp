#ifndef SEQUENCE_PARSER_HPP
#define SEQUENCE_PARSER_HPP

#include "Parser.hpp"

#include <utility>
#include <tuple>

// namespace parsers
// {
//     template <typename... ParserTypes>
//     AltParser<p_first_t<ParserTypes...>, std::tuple<ParserTypes...>> alt(ParserTypes&&... parsers);
// }


template <typename T, typename Tuple>
class SequenceParser: public Parser<Tuple, SequenceParser<T, Tuple>> {

    template<typename T2, typename F, typename P2>
    friend class ConditionalParser;

    template <typename T2, typename From, typename F, typename P2>
    friend class MapParser;

    template <typename T2, typename Tuple2>
    friend class AltParser;

    template <typename T2, typename Tuple2>
    friend class SequenceParser;

    template<typename T2, typename Derived>
    friend class Parser;


private:
    template <typename... ParserTypes>
    SequenceParser(ParserTypes&&... parsers)
        : parsers_(std::tuple<ParserTypes...>(std::forward<ParserTypes>(parsers)...)) {}


    virtual std::optional<T> apply(const std::string& input, size_t *pos) const override
    {
        return applyHelper<0>(input, pos);
    }


    // Recursive tuple iteration via templates
    template <typename TupResult, int I, std::enable_if_t<I == std::tuple_size_v<Tuple>, int> = 0>
    std::optional<T> applyHelper(const std::string&, size_t *, TupResult) const
    {
        return {};
    }

    template <typename TupResult, int I, std::enable_if_t<I != std::tuple_size_v<Tuple>, int> = 0>
    std::optional<T> applyHelper(const std::string& input, size_t *pos, TupResult currentTuple) const
    {
        std::optional<T> optResult = std::get<I>(parsers_).apply(input, pos);
        if (optResult.has_value()) {
            auto newTup = std::tuple_cat(currentTuple, make_tuple(std::move(optResult.value())));
            return applyHelper<decltype(newTup), I+1>(input, pos, newTup);
        }
        return {};
    }


    Tuple parsers_;
};


#endif
