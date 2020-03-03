#ifndef PARSERS_HPP
#define PARSERS_HPP

#include "ConditionalParser.hpp"
#include "MapParser.hpp"
#include "CharParser.hpp"
#include "AltParser.hpp"
#include "LazyParser.hpp"
#include "SequenceParser.hpp"
#include "ManyParser.hpp"
#include "IgnoreParser.hpp"
#include "StringParser.hpp"

#include <stdlib.h>

// TODO: Put this in templates.cpp
// template <typename T>
// struct p_result;

// template <template <typename> class P, typename T>
// struct p_result<P<T>> {};


namespace parsers
{
    /**************************************************************************
     *                              COMBINATORS
     **************************************************************************/
    template <typename... ParserTypes>
    AltParser<p_first_t<ParserTypes...>, decay_ifn_lazy_t<ParserTypes>...>
    alt(ParserTypes&&... parsers)
    {
        return
            AltParser<p_first_t<ParserTypes...>, decay_ifn_lazy_t<ParserTypes>...>
            (std::forward<ParserTypes>(parsers)...);
    }


    template <typename... ParserTypes>
    SequenceParser<p_results_filtered_t<ParserTypes...>, decay_ifn_lazy_t<ParserTypes>...>
    seq(ParserTypes&&... parsers)
    {
        return
            SequenceParser<
                p_results_filtered_t<ParserTypes...>,
                decay_ifn_lazy_t<ParserTypes>...>
            (std::forward<ParserTypes>(parsers)...);
    }


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
    many(P&& parser)
    {
        return
            ManyParser<
                std::conditional_t<
                    std::is_same_v<p_result_t<P>, char>,
                    std::string,
                    std::conditional_t<
                        std::is_same_v<p_result_t<P>, ignore_t>,
                        ignore_t,
                        std::vector<p_result_t<P>>>>,
                decay_ifn_lazy_t<P>>
            (std::forward<P>(parser));
    }


    template <typename P>
    auto some(P&& parser)
    {
        return transform(
            seq(parser, many(parser)),
            [](auto&& tup) {
                auto& vec = std::get<1>(tup);
                vec.insert(vec.begin(), std::move(std::get<0>(tup)));
                return std::move(vec);
            });
    }



    template <typename P>
    IgnoreParser<decay_ifn_lazy_t<P>> skip(P&& parser)
    {
        return IgnoreParser<decay_ifn_lazy_t<P>>(std::forward<P>(parser));
    }


    template<typename P, typename F>
    ConditionalParser<p_result_t<P>, std::decay_t<F>, decay_ifn_lazy_t<P>>
    doOnlyIf(P&& parser, F&& condFn)
    {
        return ConditionalParser<p_result_t<P>, std::decay_t<F>, decay_ifn_lazy_t<P>>(
            std::forward<P>(parser), std::forward<F>(condFn));
    }


    template<typename P, typename F>
    MapParser<
        std::decay_t<std::invoke_result_t<F, p_result_t<P>&&>>,
        std::decay_t<F>,
        decay_ifn_lazy_t<P>>
    transform(P&& parser, F&& mapFn)
    {
        return MapParser<
            std::decay_t<std::invoke_result_t<F, p_result_t<P>&&>>,
            std::decay_t<F>,
            decay_ifn_lazy_t<P>
        >
        (std::forward<P>(parser), std::forward<F>(mapFn));
    }


    /**************************************************************************
     *                            COMMON PARSERS
     **************************************************************************/
    CharParser anyChar()
    {
        static CharParser p;
        return p;
    }


    auto thisChar(char c)
    {
        return doOnlyIf(anyChar(), [c](char d) { return c == d; });
    }


    StringParser thisString(const std::string& str) { return StringParser(str); }
    StringParser thisString(std::string&& str) { return StringParser(str); }
    StringParser thisString(const char *str) { return StringParser(str); }


    auto whiteSpace = many(doOnlyIf(anyChar(), [](char c) { return isspace(c); }));


    // TODO: This is really inefficient and super common. Define a new parser instead.
    template <typename P1, typename P2>
    auto ignoreAndThen(P1&& toIgnore, P2&& parser)
    {
        return transform(
            seq(
                std::forward<P1>(toIgnore),
                std::forward<P2>(parser)),
            [](auto&& tup) { return std::move(std::get<1>(tup)); });

    }


    template <typename P>
    auto skipWs(P&& parser)
    {
        return transform(
            seq(
                whiteSpace,
                std::forward<P>(parser)),
            [](auto&& tup) { return std::move(std::get<1>(tup)); });

    }


    auto anyDigitChar = doOnlyIf(anyChar(), [](char c) { return '0' <= c && c <= '9'; });

    auto anyULong =
        transform(
            skipWs(some(anyDigitChar)),
            [](std::string&& str) { return stoul(str); });


    template <typename T>
    LazyParser<T> lazy() { return LazyParser<T>(); }
}

#endif
