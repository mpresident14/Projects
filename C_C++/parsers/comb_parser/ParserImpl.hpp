#ifndef PARSER_IMPL_HPP
#define PARSER_IMPL_HPP 1

#include <string_view>
#include <optional>
#include <utility>
#include <stdexcept>

#include <iostream>

template<typename T>
class Parser;

template<typename T>
class ParserImpl {
public:
    friend class Parser<T>;
    using OptResult = std::optional<std::pair<T, std::string_view>>;

    virtual ~ParserImpl() {}

// protected:
    virtual OptResult maybeParse(std::string_view) const = 0;

    OptResult makeResult(T result, std::string_view rest) const
    {
        return std::make_optional(std::make_pair(result, rest));
    }
};

#endif
