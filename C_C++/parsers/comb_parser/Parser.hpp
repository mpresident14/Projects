#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <typename T, typename F, typename P>
class ConditionalParser;

template <typename T, typename R, typename P>
class MapParser;

class CharParser;

template <typename T, typename Derived>
class Parser {
public:
    T parse(const std::string& input) const;

    template<
        typename F,
        // Early compiler error for user rather than displaying error in the implementation parts
        typename = std::enable_if_t<
            std::is_same_v<
                std::invoke_result_t<F, const T&>,
                bool
            >
        >
    >
    auto onlyIf(F condFn) const &;

    template<
        typename F,
        // Early compiler error for user rather than displaying error in the implementation parts
        typename = std::enable_if_t<
            std::is_same_v<
                std::invoke_result_t<F, const T&>,
                bool
            >
        >
    >
    auto onlyIf(F condFn) const &&;

    template<typename To> auto mapTo(To (*mapFn)(T&&)) const &;
    template<typename To> auto mapTo(To (*mapFn)(T&&)) const &&;
protected:
    virtual std::optional<T> apply(const std::string& input, size_t *pos) const = 0;
};


/**************************************************************************
 *                       INTERFACE PARSING FUNCTION
 **************************************************************************/

template <typename T, typename Derived>
T Parser<T, Derived>::parse(const std::string& input) const
{
    size_t pos = 0;
    std::optional<T> optResult = apply(input, &pos);
    if (optResult.has_value()) {
        return optResult.value();
    }
    throw std::invalid_argument("No parse for input " + input);
}

/**************************************************************************
 *                              COMBINATORS
 **************************************************************************
 * decltype is compile-time, so it cannot deduce the Parser type in the
 * template of the derived class constructor. Therefore, we use the
 * curiously recurring template pattern (CRTP) to allow the base class
 * Parser to know what type of Parser it is at compile-time. See
 * https://www.geeksforgeeks.org/curiously-recurring-template-pattern-crtp-2/)
 * for more on CRTP.
 *
 * Note that we static_cast to a Derived* and then dereference, rather
 * than just casting to Derived, which would result in object slicing
 * (if we provided a Parser -> Derived constructor). Note that we could
 * also static_cast to a Derived& instead.
 * */


template <typename T, typename Derived>
template<typename F, typename Void>
auto Parser<T, Derived>::onlyIf(F condFn) const &
{
    return ConditionalParser<T, F, Derived>(
        *static_cast<const Derived*>(this), std::move(condFn));
}

template <typename T, typename Derived>
template<typename F, typename Void>
auto Parser<T, Derived>::onlyIf(F condFn) const &&
{
    return ConditionalParser<T, F, Derived>(
        std::move(*static_cast<const Derived*>(this)), std::move(condFn));
}

template <typename T, typename Derived>
template<typename To>
auto Parser<T, Derived>::mapTo(To (*mapFn)(T&&)) const &
{
    return MapParser<To, T, Derived>(*static_cast<const Derived*>(this), mapFn);
}

template <typename T, typename Derived>
template<typename To>
auto Parser<T, Derived>::mapTo(To (*mapFn)(T&&)) const &&
{
    return MapParser<To, T, Derived>(std::move(*static_cast<const Derived*>(this)), mapFn);
}

#endif
