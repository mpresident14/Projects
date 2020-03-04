#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <stdexcept>
#include <type_traits>
#include <istream>
#include <sstream>
#include <tuple>
#include <optional>
#include <functional>
#include <cstring>

#include <boost/type_index.hpp>

#include <iostream>


template <typename T>
class LazyParser;

/* Basest class (only for SequenceParser errors). */
class ParserBase {
    template <typename T2, typename... PTypes>
    friend class SequenceParser;
protected:
    virtual std::string getErrMsgs(std::istream& input) = 0;
};

/* Parser abstract base class */
template <typename T>
class Parser : public ParserBase {

    template <typename T2>
    friend class LazyParser;

public:
    Parser() : errPos_(0) {}
    virtual ~Parser() {}


    T parse(std::istream& input)
    {
        std::optional<T> optResult = apply(input);
        if (optResult.has_value()) {
            return optResult.value();
        }
        throw std::invalid_argument("Parse error: " + getErrMsgs(input));
    }


    T parse(const std::string& input)
    {
        std::istringstream strStream(input);
        return parseAll(strStream);
    }


    T parseAll(std::istream& input)
    {
        T result = parse(input);
        if (input.peek() != EOF) {
            std::string s;
            std::getline(input, s);
            throw std::invalid_argument("Parse error: \"" + s + "\" remained.");
        }
        return result;
    }


    T parseAll(const std::string& input)
    {
        std::istringstream strStream(input);
        return parseAll(strStream);
    }


    void setCustomErrMsg(const std::string& expected)
    {
        customErrMsg_ = expected;
    }


    using result_type = T;

protected:
    virtual std::optional<T> apply(std::istream& input) = 0;

    std::string myErrMsg(std::istream& input) const
    {
        std::string nextLine;
        input.seekg(errPos_);
        std::getline(input, nextLine);
        return
            "Expected " +  (customErrMsg_.empty() ? boost::typeindex::type_id_with_cvr<T>().pretty_name() : customErrMsg_) +
            ", got \"" + nextLine + "\".";
    }


    // template <typename P>
    // P& customErr(const std::string& msg)
    // {
    //     size_t len = msg.length();
    //     customErrMsg_ = new char[len + 1];
    //     customErrMsg_[len] = '\0';
    //     strncpy(customErrMsg_, msg.c_str(), len);
    //     return *static_cast<P*>(this);
    // }

    size_t errPos_;
    std::string customErrMsg_;
};

// TODO: Separate these into namespaces "parsers" (public) and "detail" (private)
/**************************************************************************
 *                          TEMPLATE UTILITIES
 **************************************************************************/
namespace parsers
{
    /* Remove reference wrapper. */
    template<typename P>
    struct rm_ref_wrap { using type = P; };

    template<typename P>
    struct rm_ref_wrap<std::reference_wrapper<P>> { using type = P; };

    template<typename P>
    using rm_ref_wrap_t = typename rm_ref_wrap<P>::type;

    /* *
     * Get the type of the result the parser returns
     * NOTE: We can't use the strategy of using a template type Parser<T, Derived> in our
     * struct because we want to use this for the derived types.
     * */
    template<typename>
    struct p_result;

    template<typename P>
    struct p_result { using type = typename rm_ref_wrap_t<std::decay_t<P>>::result_type; };

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


    /* Check whether element I of Tuple is a Parser<ignore_t> */
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
}

#endif
