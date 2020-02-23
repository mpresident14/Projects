#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <string_view>
#include <optional>
#include <stdexcept>
#include <type_traits>

template <typename T, typename P>
class ConditionalParser;

template <typename T, typename R, typename P>
class MapParser;

template <typename T>
class Parser {
public:
    T parse(const std::string& input);

    /* *
    * See below for comments on this.
    * */
    // auto onlyIf(bool (*condFn)(const T&)) = 0;
    // template<typename To>
    // auto mapTo(To (*mapFn)(T&&)) = 0;


protected:
    std::optional<T> apply(std::string_view input);
    virtual std::optional<T> apply(std::string_view input, size_t *pos) = 0;
};


template<typename T>
T Parser<T>::parse(const std::string& input)
{
    std::optional<T> optResult = apply(input);
    if (optResult.has_value()) {
        return optResult.value();
    }
    throw std::invalid_argument("No parse for input " + input);
}

template<typename T>
std::optional<T> Parser<T>::apply(std::string_view input)
{
    size_t pos = 0;
    return apply(input, &pos);
}

/* *
 * decltype is compile-time, so it cannot deduce the derived type
 * I think I will have these virtual and define them in every class :(
 * */

// template<typename T>
// auto Parser<T>::onlyIf(bool (*condFn)(const T&))
// {
//     return ConditionalParser<T, std::remove_pointer_t<decltype(this)>>(*this, condFn);
// }

// template<typename T>
// template<typename To>
// auto Parser<T>::mapTo(To (*mapFn)(T&&))
// {
//     return MapParser<To, T, std::remove_pointer_t<decltype(this)>>(*this, mapFn);
// }

#endif
