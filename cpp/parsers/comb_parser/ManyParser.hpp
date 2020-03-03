#ifndef MANY_PARSER_HPP
#define MANY_PARSER_HPP

#include "Parser.hpp"

#include <vector>

template <typename T, typename P>
class ManyParser;

namespace parsers
{
    template <typename P>
    ManyParser<
        std::conditional_t<
            std::is_same_v<p_result_t<P>, char>,
            std::string,
            std::conditional_t<
                std::is_same_v<p_result_t<P>, ignore_t>,
                ignore_t,
                std::vector<p_result_t<P>>>>,
        decay_ifn_lazy_t<P>>
    many(P&& parser);
}


template <typename T, typename P>
class ManyParser: public Parser<T> {

    template <typename T2, typename F2, typename P2>
    friend class MapParser;

    template<typename T2, typename F2, typename P2>
    friend class ConditionalParser;

    template <typename T2, typename... PTypes>
    friend class AltParser;

    template <typename T2, typename... PTypes>
    friend class SequenceParser;

    template <typename T2, typename P2>
    friend class ManyParser;

    template <typename P2>
    friend class IgnoreParser;

    template <typename P2>
    ManyParser<
        std::conditional_t<
            std::is_same_v<parsers::p_result_t<P2>, char>,
            std::string,
            std::conditional_t<
                std::is_same_v<parsers::p_result_t<P2>, parsers::ignore_t>,
                parsers::ignore_t,
                std::vector<parsers::p_result_t<P2>>>>,
        parsers::decay_ifn_lazy_t<P2>>
    friend parsers::many(P2&& parser);


private:
    ManyParser(const P& parser) : parser_(parser) {}
    ManyParser(P&& parser) : parser_(std::move(parser)) {}

    virtual std::optional<T> apply(std::istream& input) const override
    {
        return applyHelper(input);
    }


    /* Default: return a vector */
    template <
        typename P1 = P,
        std::enable_if_t<
            !std::is_same_v<parsers::p_result_t<P1>, char>
                && !std::is_same_v<parsers::p_result_t<P1>, parsers::ignore_t>,
            int> = 0>
    std::optional<T> applyHelper(std::istream& input) const
    {
        using PType = parsers::p_result_t<P>;

        std::vector<PType> v;
        std::optional<PType> optResult = parser_.apply(input);
        while (optResult.has_value()) {
            v.push_back(std::move(optResult.value()));
            optResult = parser_.apply(input);
        }

        return std::optional(v);
    }


    /* Specialization: return a string for many chars */
    template <
        typename P1 = P,
        std::enable_if_t<std::is_same_v<parsers::p_result_t<P1>, char>, int> = 0>
    std::optional<T> applyHelper(std::istream& input) const
    {
        std::string s;
        std::optional<char> optResult = parser_.apply(input);
        while (optResult.has_value()) {
            s.append(1, std::move(optResult.value()));
            optResult = parser_.apply(input);
        }

        return std::optional(s);
    }


    /* Specialization: return ignore_t for many ignore_ts */
    template <
        typename P1 = P,
        std::enable_if_t<std::is_same_v<parsers::p_result_t<P1>, parsers::ignore_t>, int> = 0>
    std::optional<T> applyHelper(std::istream& input) const
    {
        std::optional<parsers::ignore_t> optResult = parser_.apply(input);
        while (optResult.has_value()) {
            optResult = parser_.apply(input);
        }

        return std::optional(parsers::ignore_t());
    }

    P parser_;
};


#endif
