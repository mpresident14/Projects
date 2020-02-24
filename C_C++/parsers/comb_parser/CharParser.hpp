#ifndef CHAR_PARSER_HPP
#define CHAR_PARSER_HPP

#include "Parser.hpp"

class CharParser: public Parser<char, CharParser> {
    template<typename T2, typename P2>
    friend class ConditionalParser;
    template <typename T2, typename From2, typename P2>
    friend class MapParser;
    template<typename T2, typename Derived>
    friend class Parser;

private:
    virtual std::optional<char> apply(std::string_view input, size_t *pos) override
    {
        // if (input[*pos] == c_) {
        //     ++(*pos);
        //     return std::make_optional(c_);
        // }
        // return {};
        if (*pos < input.size()) {
            auto optResult = std::make_optional(input[*pos]);
            ++(*pos);
            return optResult;
        }
        return {};
    }
};

#endif
