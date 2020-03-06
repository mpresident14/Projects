#ifndef PARSER_HPP
#define PARSER_HPP

#include <cstring>
#include <functional>
#include <istream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>

#include <boost/type_index.hpp>

#include <iostream>
#include <prez/timeit.hpp>


template <typename T>
class LazyParser;

/* Base class (only for SequenceParser errors). */
class ParserBase {
  template <typename T2, typename... PTypes>
  friend class SequenceParser;

public:
  virtual ~ParserBase() {}

protected:
  virtual std::string getErrMsgs(std::istream& input) = 0;
  virtual std::optional<T> apply(std::istream& input) = 0;
};

/* Base class (only for LazyParser). */
template <typename T>
class ParserBase2 : public ParserBase {
  template <typename T2>
  friend class LazyParser;

public:
  virtual ~ParserBase2() {}

protected:
  virtual std::optional<T> apply(std::istream& input) = 0;
};

/* Parser main abstract base class */
template <typename T, typename Derived>
class Parser : public ParserBase2<T> {
public:
  virtual ~Parser() {}

  T parse(std::istream& input) {
    std::optional<T> optResult = this->apply(input);
    if (optResult.has_value()) {
      return std::move(optResult.value());
    }
    throw std::invalid_argument("Parse error: " + this->getErrMsgs(input));
  }

  T parse(const std::string& input) {
    std::istringstream strStream(input);
    return parseAll(strStream);
  }

  T parseAll(std::istream& input) {
    T result = parse(input);
    if (input.peek() != EOF) {
      std::string s;
      std::getline(input, s);
      throw std::invalid_argument("Parse error: \"" + s + "\" remained.");
    }
    return result;
  }

  T parseAll(const std::string& input) {
    std::istringstream strStream(input);
    return parseAll(strStream);
  }

  /*
   * decltype is compile-time, so it cannot deduce the Parser type in the
   * template of the derived class constructor. Therefore, we use the
   * curiously recurring template pattern (CRTP) to allow the base class
   * Parser to know what type of Parser it is at compile-time. See
   * https://www.geeksforgeeks.org/curiously-recurring-template-pattern-crtp-2/)
   * for more on CRTP.
   *
   * Note that we static_cast to a Derived&, rather than just casting to
   * Derived, which would result in object slicing	(if we provided a Parser ->
   * Derived constructor). Note that we could	also do *static_cast<Derived*>
   * instead.
   */
  Derived& setCustomErrMsg(const std::string& expected) {
    customErrMsg_ = expected;
    return static_cast<Derived&>(*this);
  }

  using result_type = T;

protected:
  std::string myErrMsg(std::istream& input) const {
    std::string nextLine;
    input.seekg(errPos_);
    std::getline(input, nextLine);

    // Default to expected type
    if (customErrMsg_.empty()) {
      return "Expected an object of type <" +
             (customErrMsg_.empty()
                  ? boost::typeindex::type_id_with_cvr<T>().pretty_name()
                  : customErrMsg_) +
             ">, got \"" + nextLine + "\".";
    }

    return "Expected " + customErrMsg_ + ", got \"" + nextLine + "\".";
  }

  size_t errPos_ = 0;
  std::string customErrMsg_;
};

// TODO: Separate these into namespaces "parsers" (public) and "detail"
// (private)
/**************************************************************************
 *                          TEMPLATE UTILITIES
 **************************************************************************/
namespace parsers {
  /* Remove reference wrapper. */
  template <typename P>
  struct rm_ref_wrap {
    using type = P;
  };

  template <typename P>
  struct rm_ref_wrap<std::reference_wrapper<P>> {
    using type = P;
  };

  template <typename P>
  using rm_ref_wrap_t = typename rm_ref_wrap<P>::type;

  /* *
   * Get the type of the result the parser returns
   * NOTE: We can't use the strategy of using a template type Parser<T, Derived>
   * in our struct because we want to use this for the derived types.
   * */
  template <typename>
  struct p_result;

  template <typename P>
  struct p_result {
    using type = typename rm_ref_wrap_t<std::decay_t<P>>::result_type;
  };

  template <typename P>
  using p_result_t = typename p_result<P>::type;

  /* Get the result type of the first parser in a parameter pack */
  template <typename... ParserTypes>
  struct p_first {
    using type =
        p_result_t<std::tuple_element_t<0, std::tuple<ParserTypes...>>>;
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

  /* Get a tuple of all the types in the parser parameter pack that aren't
   * ignore_t */
  template <typename... ParserTypes>
  struct p_results_filtered {
    using type = decltype(std::tuple_cat(
        std::declval<std::conditional_t<
            std::is_same_v<p_result_t<ParserTypes>, ignore_t>, std::tuple<>,
            std::tuple<p_result_t<ParserTypes>>>>()...));
  };


  template <typename... ParserTypes>
  using p_results_filtered_t =
      typename p_results_filtered<ParserTypes...>::type;


  /* Check whether element I of Tuple is a Parser<ignore_t> */
  template <int I, typename Tuple>
  struct is_ignore {
    static constexpr bool value =
        std::is_same_v<p_result_t<std::tuple_element_t<I, Tuple>>, ignore_t>;
  };

  template <int I, typename Tuple>
  constexpr bool is_ignore_v = is_ignore<I, Tuple>::value;

  // From Effective Modern C++, Item 3: "auto specifies that the type is to be
  // deduced and decltype says that decltype rules should be used during the
  // deduction." This allows the function to return a reference (auto return
  // type deduction rules would strip the reference).
  template <int I, typename... ParserTypes>
  decltype(auto) getParser(std::tuple<ParserTypes...>& parsers) {
    return static_cast<
        rm_ref_wrap_t<std::tuple_element_t<I, std::tuple<ParserTypes...>>>&>(
        std::get<I>(parsers));
  }
}  // namespace parsers

#endif
