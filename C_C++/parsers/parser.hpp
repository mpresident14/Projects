#ifndef PARSER_HPP
#define PARSER_HPP

// #include "movable_fn.hpp"

#include <string>
#include <optional>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <memory>

template<typename T>
class Parser {
public:
    using result_t   = std::optional<std::pair<T, std::string>>;
    using input_t    = const std::string;

    // These templates allow detection of a Parser of any type.
    template<typename>
    struct is_parser : std::false_type {};

    template<typename U>
    struct is_parser<Parser<U>> : std::true_type {};


    template<typename U>
    static Parser<U> createBasic(U&&);
    
    // The "invoke_result" part ensures that the parameter is a function (implements operator()).
    // This ensures that this constructor doesn't interfere with the move and copy constructors.
    template<
        typename F,
        typename = std::enable_if_t<
            std::is_convertible_v<result_t, std::invoke_result_t<F, input_t&>>
        >
    >
    Parser(F&& f);
    ~Parser() = default;
    Parser(const Parser&) = default;
    Parser(Parser&&) = default;
    Parser& operator=(const Parser&) = default;
    Parser& operator=(Parser&&) = default;


    /* Combinators */
    template<typename F>
    std::enable_if_t<is_parser<std::invoke_result_t<F, T>>::value, std::invoke_result_t<F, T>>
    andThen(F&& pGenFn) const;

    
    // Parser<std::pair<T,R>> combine(const Parser<R>& nextParser) const;

    T parse(input_t&) const;

private:
    
    
    struct FnContainerAbstract {
        virtual ~FnContainerAbstract() {}
        virtual result_t operator()(input_t&) const = 0;
    };

    template<typename F>
    struct FnContainer : FnContainerAbstract {
        FnContainer(F&& f) : f_{ std::forward<F>(f) } {}
        result_t operator()(input_t& input) const override
        {
            return f_(input);
        }

        F f_;
    };

    // Shared pointer makes a Parser copyable. If did not care about
    // that, we could use a unique pointer instead.
    std::shared_ptr<FnContainerAbstract> parseFn_;
};

#include "parser_impl.hpp"

#endif