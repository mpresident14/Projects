#ifndef PARSERS_HPP
#define PARSERS_HPP

#include "Parser.hpp"
#include "ConditionalParser.hpp"
#include "MapParser.hpp"
#include "CharParser.hpp"
#include "AltParser.hpp"


namespace parsers
{
    /**************************************************************************
     *                           PRECOMPUTED PARSERS
     **************************************************************************/
    constexpr CharParser anyChar;


    /**************************************************************************
     *                           NONCHAINED COMBINATORS
     **************************************************************************/
    template <typename T, typename... ParserTypes>
    auto alt(ParserTypes&&... parsers)
    {
        return AltParser<T, std::tuple<ParserTypes...>>(std::forward<ParserTypes>(parsers)...);
    }
}

#endif
