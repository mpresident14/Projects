#ifndef PARSERS_HPP
#define PARSERS_HPP

#include "ConditionalParser.hpp"
#include "MapParser.hpp"
#include "CharParser.hpp"
#include "AltParser.hpp"

#include <functional>


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

    // TODO: Create a new parser class for this.
    auto fail()
    {
        return anyChar().onlyIf([](char){ return false; });
    }


    /**************************************************************************
     *                           NONCHAINED COMBINATORS
     **************************************************************************/
    template <typename... ParserTypes>
    AltParser<parsers::p_first_t<ParserTypes...>, std::tuple<ParserTypes...>> alt(ParserTypes&&... parsers)
    {
        return AltParser<parsers::p_first_t<ParserTypes...>, std::tuple<ParserTypes...>>(std::forward<ParserTypes>(parsers)...);
    }

    template <typename P>
    std::reference_wrapper<P> lazy(P& parser)
    {
        return std::ref<P>(parser);
    }
}

#endif
