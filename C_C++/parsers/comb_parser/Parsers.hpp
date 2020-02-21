#ifndef PARSERS_HPP
#define PARSERS_HPP 1

#include "Parser.hpp"
#include "AnyCharParser.hpp"
#include "ConditionalParser.hpp"

namespace parsers
{
    // TODO: Use inline in private static for this
    AnyCharParser anyCharImpl;
    Parser<char> anyChar(&anyCharImpl);

    template<typename T>
    inline ConditionalParser<T> cond(const Parser<T>& parser, bool (*condFn)(T&))
    {
        return ConditionalParser<T>(parser, condFn);
    }
}


#endif
