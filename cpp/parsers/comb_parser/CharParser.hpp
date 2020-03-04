#ifndef CHAR_PARSER_HPP
#define CHAR_PARSER_HPP

#include "Parser.hpp"

class CharParser;

namespace parsers
{
    CharParser anyChar();
}

class CharParser: public Parser<char> {

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

    friend CharParser parsers::anyChar();

private:
    CharParser() {}

    virtual std::optional<char> apply(std::istream& input) override
    {
        int c = input.get();
        if (c != EOF) {
            return std::optional(c);
        }

        // The istream object apparently stops working when you reach the EOF,
        // so you need to clear the bit.
        input.clear();
        input.unget();
        this->errPos_ = input.tellg();
        return {};
    }

    virtual std::string getErrMsgs(std::istream& input) override
    {
        return this->myErrMsg(input);
    }
};

#endif
