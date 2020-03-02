#ifndef CHAR_PARSER_HPP
#define CHAR_PARSER_HPP

#include "Parser.hpp"

class CharParser;

namespace parsers
{
    CharParser anyChar();
}

class CharParser: public Parser<char, CharParser> {

    template<typename T2, typename F, typename P2>
    friend class ConditionalParser;

    template <typename T2, typename F2, typename P2>
    friend class MapParser;

    template <typename T2, typename... PTypes>
    friend class AltParser;

    template <typename T2, typename... PTypes>
    friend class SequenceParser;

    friend CharParser parsers::anyChar();

private:
    CharParser() {}

    virtual std::optional<char> apply(std::istream& input) const override
    {
        int c;
        if ((c = input.get()) != EOF) {
            return std::optional(c);
        }
        return {};
    }
};

#endif
