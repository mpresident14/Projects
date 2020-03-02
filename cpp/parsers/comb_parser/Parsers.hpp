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
    CharParser anyChar()
    {
        static CharParser p;
        return p;
    }

    StringParser thisString(const std::string& str) { return StringParser(str); }
    StringParser thisString(std::string&& str) { return StringParser(str); }
    StringParser thisString(const char *str) { return StringParser(str); }

    template <typename T>
    LazyParser<T> lazy()
    {
        return LazyParser<T>();
    }


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
    ManyParser<std::vector<parsers::p_result_t<P>>, std::decay_t<P>> many(P&& parser)
    {
        return
            ManyParser<std::vector<parsers::p_result_t<P>>, std::decay_t<P>>(std::forward<P>(parser));
    }


    template <typename P>
    IgnoreParser<std::decay_t<P>> ignore(P&& parser)
    {
        return IgnoreParser<std::decay_t<P>>(std::forward<P>(parser));
    }
}

#endif
