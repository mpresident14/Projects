#ifndef STRING_PARSER_HPP
#define STRING_PARSER_HPP

#include "Parser.hpp"

#include <string>

class StringParser;

namespace parsers
{
    StringParser thisString(const std::string& str, bool consumeWhiteSpace);
    StringParser thisString(std::string&& str, bool consumeWhiteSpace);
    StringParser thisString(const char *str, bool consumeWhiteSpace);
}

class StringParser: public Parser<std::string, StringParser> {

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

    friend StringParser parsers::thisString(const std::string& str, bool consumeWhiteSpace);
    friend StringParser parsers::thisString(std::string&& str, bool consumeWhiteSpace);
    friend StringParser parsers::thisString(const char *str, bool consumeWhiteSpace);

private:
    StringParser(const std::string& str, bool consumeWhiteSpace)
        : str_(str), consumeWhiteSpace_{consumeWhiteSpace} {}

    StringParser(std::string&& str, bool consumeWhiteSpace)
        : str_(std::move(str)), consumeWhiteSpace_{consumeWhiteSpace} {}

    virtual std::optional<std::string> apply(std::istream& input) const override
    {
        int oldPos = input.tellg();

        std::cout << "POS: " << oldPos << std::endl;

        char inC;
        if (consumeWhiteSpace_) {
            while (isspace(inC = input.peek())) {
                input.get();
            }
        }

        for (const char& c : str_) {
            inC = input.get();
            if (inC == EOF || inC != c) {
                input.clear();
                input.seekg(oldPos);
                return {};
            }
        }

        return std::optional(str_);
    }

    std::string str_;
    bool consumeWhiteSpace_;
};

#endif
