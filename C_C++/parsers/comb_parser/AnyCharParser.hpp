#ifndef ANY_CHAR_PARSER_HPP
#define ANY_CHAR_PARSER_HPP 1

#include "ParserImpl.hpp"


class AnyCharParser: public ParserImpl<char> {
public:
    AnyCharParser::OptResult maybeParse(std::string_view input) const override
    {
        if (!input.empty()) {
            return makeResult(input[0], input.substr(1));
        }
        return {};
    }
};

#endif

