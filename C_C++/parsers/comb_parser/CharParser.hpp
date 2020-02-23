#ifndef CHAR_PARSER_HPP
#define CHAR_PARSER_HPP

#include "Parser.hpp"

class CharParser: public Parser<char> {
public:
    CharParser(char c) : c_(c) {}
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

    auto onlyIf(bool (*condFn)(const char&))
    {
        return ConditionalParser<char, std::remove_pointer_t<decltype(this)>>(*this, condFn);
    }

    template<typename To>
    auto mapTo(To (*mapFn)(char&&))
    {
        return MapParser<To, char, std::remove_pointer_t<decltype(this)>>(*this, mapFn);
    }

private:
    char c_;
};

#endif
