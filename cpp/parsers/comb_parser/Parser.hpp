#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <stdexcept>
#include <type_traits>
#include <istream>
#include <sstream>
#include <tuple>
#include <optional>


template <typename T>
class LazyParser;

/* Parser abstract base class */
template <typename T>
class Parser {

    template <typename T2>
    friend class LazyParser;

public:
    virtual ~Parser(){}

    T parse(std::istream& input) const
    {
        std::optional<T> optResult = apply(input);
        if (input.peek() != EOF) {
            // TODO: Replace with appropriate parse error.
            throw std::invalid_argument("Characters remained.");
        }
        if (optResult.has_value()) {
            return optResult.value();
        }
        // TODO: Replace with appropriate parse error.
        throw std::invalid_argument("No parse for input.");
    }


    T parse(const std::string& input) const
    {
        std::istringstream strStream(input);
        return parse(strStream);
    }


    using result_type = T;

protected:
    virtual std::optional<T> apply(std::istream& input) const = 0;
};

// TODO: Separate these into namespaces "parsers" (public) and "detail" (private)
/**************************************************************************
 *                          TEMPLATE UTILITIES
 **************************************************************************/
namespace parsers
{
    /* *
     * Get the type of the result the parser returns
     * NOTE: We can't use the strategy of using a template type Parser<T, Derived> in our
     * struct because we want to use this for the derived types.
     * */
    template<typename>
    struct p_result;

    template<typename P>
    struct p_result { using type = typename std::decay_t<P>::result_type; };

    template<typename P>
    using p_result_t = typename p_result<P>::type;

    /* Get the result type of the first parser in a parameter pack */
    template <typename... ParserTypes>
    struct p_first
    {
        using type = p_result_t<std::tuple_element_t<0, std::tuple<ParserTypes...>>>;
    };

    template <typename... ParserTypes>
    using p_first_t = typename p_first<ParserTypes...>::type;

    /* Get a tuple of all the types in the parser parameter pack */
    // template <typename... ParserTypes>
    // struct p_tuple_results
    // {
    //     using type = std::tuple<p_result_t<ParserTypes>...>;
    // };

    // template <typename... ParserTypes>
    // using p_tuple_results_t = typename p_tuple_results<ParserTypes...>::type;

    struct ignore_t {};

    /* Get a tuple of all the types in the parser parameter pack that aren't ignore_t */
    template <typename... ParserTypes>
    struct p_results_filtered
    {
        using type =
            decltype(std::tuple_cat(
                std::declval<
                    std::conditional_t<
                        std::is_same_v<
                            p_result_t<ParserTypes>,
                            ignore_t>,
                        std::tuple<>,
                        std::tuple<p_result_t<ParserTypes>>>>()...));
    };

    template <typename... ParserTypes>
    using p_results_filtered_t = typename p_results_filtered<ParserTypes...>::type;


    /* Check if tuple element I is a Parser<ignore_t> */
    template <int I, typename Tuple>
    struct is_ignore
    {
        static constexpr bool value =
            std::is_same_v<
                p_result_t<std::tuple_element_t<I, Tuple>>,
                ignore_t>;
    };

    template <int I, typename Tuple>
    constexpr bool is_ignore_v = is_ignore<I, Tuple>::value;


    template <typename>
    struct is_lazy : std::false_type {};

    template <typename T>
    struct is_lazy<LazyParser<T>> : std::true_type {};

    template <typename P>
    constexpr bool is_lazy_v = is_lazy<P>::value;

    template <typename P>
    struct decay_ifn_lazy {
        using type = std::conditional_t<is_lazy_v<P>, P, std::decay<P>>
    };

    template <typename P>
    using decay_ifn_lazy_t = typename decay_ifn_lazy<P>::type;

}



#endif
