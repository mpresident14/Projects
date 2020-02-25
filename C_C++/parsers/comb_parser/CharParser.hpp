#ifndef CHAR_PARSER_HPP
#define CHAR_PARSER_HPP

#include "Parser.hpp"

class CharParser: public Parser<char, CharParser> {

    template<typename T2, typename F, typename P2>
    friend class ConditionalParser;

    template <typename T2, typename From, typename F, typename P2>
    friend class MapParser;

    template <typename T2, typename Tuple>
    friend class AltParser;

    template<typename T2, typename Derived>
    friend class Parser;

private:
    virtual std::optional<char> apply(const std::string& input, size_t *pos) const override
    {
        if (*pos < input.size()) {
            auto optResult = std::make_optional(input[*pos]);
            ++(*pos);
            return optResult;
        }
        return {};
    }
};

#endif
