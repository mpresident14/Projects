#ifndef CHAR_PARSER_HPP
#define CHAR_PARSER_HPP

#include "Parser.hpp"

class CharParser;

namespace parsers
{
    CharParser anyChar(bool consumeWhiteSpace);
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

    template <typename T2, typename P2>
    friend class ManyParser;

    template <typename P2>
    friend class IgnoreParser;

    friend CharParser parsers::anyChar(bool consumeWhiteSpace);

private:
    CharParser(bool consumeWhiteSpace) : consumeWhiteSpace_(consumeWhiteSpace) {}

    virtual std::optional<char> apply(std::istream& input) const override
    {
        int c;
        if (consumeWhiteSpace_) {
            while (isspace(c = input.get())){}
        } else {
            c = input.get();
        }

        if (c != EOF) {
            return std::optional(c);
        }

        // The istream object apparently stops working when you reach the EOF,
        // so you need to clear the bit.
        input.clear();
        input.unget();
        return {};
    }

    bool consumeWhiteSpace_;
};

#endif
