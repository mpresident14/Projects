#ifndef CHAR_PARSER_HPP
#define CHAR_PARSER_HPP 1

#include "Parser.hpp"

class CharParser: public Parser<char> {
public:
    // TODO: make private and use helper function
    CharParser(char c) : c_(c) {}

protected:
    OptResult maybeParse(std::string_view input)
    {
        if (input[0] == c_) {
            return makeResult(c_, input.substr(1));
        }
        return {};
    }

    // TODO: figure out how to add line and col #s
    std::string getErrorMsg()
    {
        using namespace std::string_literals;
        return ("Expected: "s).append(1, c_).append(1, '.');
    }

private:
    char c_;
};

#endif
