#ifndef ALT_PARSER_HPP
#define ALT_PARSER_HPP

#include "Parser.hpp"

#include <utility>
#include <tuple>


template <typename T, typename Tuple>
class AltParser: public Parser<T, AltParser<T, Tuple>> {

    template<typename T2, typename F, typename P2>
    friend class ConditionalParser;

    template <typename T2, typename From, typename F, typename P2>
    friend class MapParser;

    template <typename T2, typename Tuple2>
    friend class AltParser;

    template<typename T2, typename Derived>
    friend class Parser;

// private:
public:
    template <typename... ParserTypes>
    AltParser(ParserTypes&&... parsers)
        : parsers_(std::tuple<ParserTypes...>(std::forward<ParserTypes>(parsers)...)) {}


private:
    virtual std::optional<T> apply(const std::string& input, size_t *pos) const override
    {
        return applyHelper<0>(input, pos);
    }


    // Recursive tuple iteration via templates
    template <int I, std::enable_if_t<I == std::tuple_size_v<Tuple>, int> = 0>
    std::optional<T> applyHelper(const std::string&, size_t *) const
    {
        return {};
    }

    template <int I, std::enable_if_t<I != std::tuple_size_v<Tuple>, int> = 0>
    std::optional<T> applyHelper(const std::string& input, size_t *pos) const
    {
        std::optional<T> optResult = std::get<I>(parsers_).apply(input, pos);
        if (optResult.has_value()) {
            return optResult;
        }
        return applyHelper<I+1>(input, pos);
    }


    Tuple parsers_;
};

#endif