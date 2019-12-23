#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <optional>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <memory>

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
    template<typename U>
    static Parser<U> createBasic(U&&);
    
    template<typename Fn>
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

    template<typename R>
    Parser<std::pair<T,R>> combine(Parser<R>& nextParser);


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

#include "parser_impl.hpp"

#endif