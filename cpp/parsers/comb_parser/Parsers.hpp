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

// TODO: Put this in templates.cpp
// template <typename T>
// struct p_result;

// template <template <typename> class P, typename T>
// struct p_result<P<T>> {};


namespace parsers
{
    /**************************************************************************
     *                           PRECOMPUTED PARSERS
     **************************************************************************/
    CharParser anyChar(bool consumeWhiteSpace = true)
    {
        if (consumeWhiteSpace) {
            static CharParser p(true);
            return p;
        }
        static CharParser p(false);
        return p;
    }


    auto thisChar(char c, bool consumeWhiteSpace = true)
    {
        return anyChar(consumeWhiteSpace).onlyIf([c](char d) { return c == d; });
    }


    StringParser thisString(const std::string& str, bool consumeWhiteSpace = true)
    { return StringParser(str, consumeWhiteSpace); }

    StringParser thisString(std::string&& str, bool consumeWhiteSpace = true)
    { return StringParser(str, consumeWhiteSpace); }

    StringParser thisString(const char *str, bool consumeWhiteSpace = true)
    { return StringParser(str, consumeWhiteSpace); }


    template <typename T>
    LazyParser<T> lazy() { return LazyParser<T>(); }


    /**************************************************************************
     *                           NONCHAINED COMBINATORS
     **************************************************************************/
    template <typename... ParserTypes>
    AltParser<parsers::p_first_t<ParserTypes...>, std::decay_t<ParserTypes>...>
    alt(ParserTypes&&... parsers)
    {
        return
            AltParser<parsers::p_first_t<ParserTypes...>, std::decay_t<ParserTypes>...>
            (std::forward<ParserTypes>(parsers)...);
    }


    template <typename... ParserTypes>
    SequenceParser<parsers::p_results_filtered_t<ParserTypes...>, std::decay_t<ParserTypes>...>
    seq(ParserTypes&&... parsers)
    {
        return
            SequenceParser<parsers::p_results_filtered_t<ParserTypes...>, std::decay_t<ParserTypes>...>
            (std::forward<ParserTypes>(parsers)...);
    }


    template <typename P>
    ManyParser<
        std::conditional_t<
            std::is_same_v<parsers::p_result_t<P>, char>,
            std::string,
            std::conditional_t<
                std::is_same_v<parsers::p_result_t<P>, parsers::ignore_t>,
                parsers::ignore_t,
                std::vector<parsers::p_result_t<P>>
            >
        >,
        std::decay_t<P>
    >
    many(P&& parser)
    {
        return
            ManyParser<
                std::conditional_t<
                    std::is_same_v<parsers::p_result_t<P>, char>,
                    std::string,
                    std::conditional_t<
                        std::is_same_v<parsers::p_result_t<P>, parsers::ignore_t>,
                        parsers::ignore_t,
                        std::vector<parsers::p_result_t<P>>
                    >
                >,
                std::decay_t<P>
            >
            (std::forward<P>(parser));
    }


    template <typename P>
    IgnoreParser<std::decay_t<P>> ignore(P&& parser)
    {
        return IgnoreParser<std::decay_t<P>>(std::forward<P>(parser));
    }
}

#endif
