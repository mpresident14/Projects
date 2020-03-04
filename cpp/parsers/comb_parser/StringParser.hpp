#ifndef STRING_PARSER_HPP
#define STRING_PARSER_HPP

#include "Parser.hpp"

#include <string>

class StringParser;

namespace parsers
{
    StringParser thisString(const std::string& str);
    StringParser thisString(std::string&& str);
    StringParser thisString(const char *str);
}

class StringParser: public Parser<std::string> {

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

    friend StringParser parsers::thisString(const std::string& str);
    friend StringParser parsers::thisString(std::string&& str);
    friend StringParser parsers::thisString(const char *str);

private:
    StringParser(const std::string& str) : str_(str) {}
    StringParser(std::string&& str) : str_(std::move(str)) {}

    virtual std::optional<std::string> apply(std::istream& input) override
    {
        int oldPos = input.tellg();
        for (const char& c : str_) {
            char inC = input.get();
            if (inC == EOF || inC != c) {
                input.clear();
                input.seekg(oldPos);
                return {};
            }
        }

        return std::optional(str_);
    }


    virtual std::string getErrMsgs(std::istream& input) override
    {
        if (this->customErrMsg_.empty()) {
            customErrMsg_ = "\"" + str_ + "\"";
        }
        return this->myErrMsg(input);
    }


    std::string str_;
};

#endif
