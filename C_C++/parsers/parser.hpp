#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <optional>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <memory>
#include <vector>

// TODO: perhaps pass input_t by value instead of reference all the time to prevent 
// unnecessary copies when putting them in pairs. Do a timing test.

template<typename T>
class Parser;

// TODO: This might not be good practice for header files.
template<typename T>
using result_t   = std::optional<std::pair<T, std::string>>;
using input_t    = const std::string;

// These templates allow detection of a Parser of any type and get the type of the Parser.
template<typename>
struct is_parser : std::false_type {};

template<typename U>
struct is_parser<Parser<U>> : std::true_type { using parser_type = U; };

template<typename U>
static inline constexpr bool is_parser_v = is_parser<U>::value;

template<typename U>
using is_parser_pt = typename is_parser<U>::parser_type;

template<typename T>
class Parser {
public:    
    // template<typename Fn>
    Parser();
    // The "invoke_result" part ensures that the parameter is a function (implements operator()).
    // This ensures that this constructor doesn't interfere with the move and copy constructors.
    template<
        typename Fn,
        typename = std::enable_if_t<
            std::is_convertible_v<result_t<T>, std::invoke_result_t<Fn, input_t&>>
        >
    >
    Parser(Fn&& f);
    ~Parser() = default;
    Parser(const Parser&) = default;
    Parser(Parser&&) = default;
    Parser& operator=(const Parser&) = default;
    Parser& operator=(Parser&&) = default;


    /* Combinators */
    template<typename Fn, typename Par = std::invoke_result_t<Fn, T>>
    std::enable_if_t<is_parser_v<Par>, Par> andThen(Fn&& pGenFn) const;

    Parser<T> alt(Parser<T> nextParser) const;

    // mapFn must accept an rvalue reference
    template<typename Fn, typename R = std::invoke_result_t<Fn, T>>
    Parser<std::decay_t<R>> andThenMap(Fn&& mapFn) const;

    template<typename R>
    Parser<std::pair<T,R>> combine(Parser<R> nextParser) const;

    template<
        typename Fn,
        typename = std::enable_if_t<
            std::is_convertible_v<bool, std::invoke_result_t<Fn, T>>
        >
    >
    Parser<T> verify(Fn&& boolFn) const;

    // Parser<std::list<T>> cons(Parser<std::list<T>>) const;

    // Hacky method to enable the correct overload depending on whether T is a char. 
    // https://stackoverflow.com/questions/52077051/sfinae-enable-if-cannot-be-used-to-disable-this-declaration
    template<typename U = T>
    Parser<std::enable_if_t<std::is_same_v<U, char>, std::string>> many() const;
    template<typename U = T>
    Parser<std::enable_if_t<!std::is_same_v<U, char>, std::vector<T>>> many() const;

    Parser<std::nullptr_t> ignore() const;

    template<typename R>
    Parser<R> ignoreAndThen(Parser<R> nextParser) const;

    template<typename R>
    Parser<T> thenIgnore(Parser<R> nextParser) const;

    T parse(input_t&) const;

private:    
    struct FnContainerAbstract {
        virtual ~FnContainerAbstract() {}
        virtual result_t<T> operator()(input_t&) = 0;
    };

    template<typename Fn>
    struct FnContainer : FnContainerAbstract {
        FnContainer(Fn&& f) : f_{ std::forward<Fn>(f) } {}
        // Since we have a mutable lambdas (e.g. in createBasic() and combine()), the () operator
        // can change the variables in the closure (i.e. change f_), so the () operator cannot be
        // const.
        result_t<T> operator()(input_t& input) override
        {
            return f_(input);
        }

        Fn f_;
    };

// TODO: Figure out how to make this private
public:  
    // Shared pointer makes a Parser copyable. If did not care about
    // that, we could use a unique pointer instead.
    std::shared_ptr<FnContainerAbstract> parseFn_;
};

namespace parsers {
    extern const Parser<char> anyChar;
    extern const Parser<int> anyInt;
    extern const Parser<std::string> whitespace;
    
    Parser<char> thisChar(char c);

    template<typename U>
    Parser<U> skipws(Parser<U>);

    template<typename U>
    result_t<std::decay_t<U>> createReturnObject(U&&, input_t);
    
    template<typename U>
    Parser<std::decay_t<U>> createBasic(U&&);
}

#include "parser_impl.hpp"
#include "parser_util.hpp"

#endif
