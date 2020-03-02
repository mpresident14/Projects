#ifndef PARSERS_HPP
#define PARSERS_HPP

#include "ConditionalParser.hpp"
#include "MapParser.hpp"
#include "CharParser.hpp"
#include "AltParser.hpp"
#include "LazyParser.hpp"
#include "SequenceParser.hpp"
#include "ManyParser.hpp"

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
    SequenceParser<parsers::p_tuple_results_t<ParserTypes...>, std::decay_t<ParserTypes>...>
    seq(ParserTypes&&... parsers)
    {
        return
            SequenceParser<parsers::p_tuple_results_t<ParserTypes...>, std::decay_t<ParserTypes>...>
            (std::forward<ParserTypes>(parsers)...);
    }


    // TODO: P is deduced to a Parser& so parser is a Parser& && -> Parser&
    template <typename P>
    ManyParser<std::vector<parsers::p_result_t<P>>, std::decay_t<P>> many(P&& parser)
    {
        return
            ManyParser<std::vector<parsers::p_result_t<P>>, std::decay_t<P>>(std::forward<P>(parser));
    }
}

#endif
