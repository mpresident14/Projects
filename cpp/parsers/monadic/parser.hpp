#ifndef PARSER_HPP
#define PARSER_HPP

#include <cctype>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <algorithm>

template <typename T>
class Parser;

// TODO: This might not be good practice for header files.
template <typename T>
using result_t = std::optional<T>;
using input_t = std::istream;

// These templates allow detection of a Parser of any type and get the type of
// the Parser.
template <typename U>
struct parser_info;

template <typename U>
struct parser_info<Parser<U>> {
  using type = U;
  static constexpr bool value = true;
};

template <typename U>
static inline constexpr bool is_parser_v = parser_info<U>::value;

template <typename U>
using ptype_t = typename parser_info<U>::type;

template <typename T>
class Parser {
private:
  struct FnContainerAbstract {
    virtual ~FnContainerAbstract() {}
    virtual result_t<T> operator()(input_t&, size_t*) = 0;
  };

  template <typename Fn>
  struct FnContainer : FnContainerAbstract {
    FnContainer(Fn&& f) : f_{std::forward<Fn>(f)} {}
    // Since we have a mutable lambdas (e.g. in createBasic() and combine()),
    // the () operator can change the variables in the closure (i.e. change f_),
    // so the () operator cannot be const.
    result_t<T> operator()(input_t& input, size_t* errPos) override {
      return f_(input, errPos);
    }

    Fn f_;
  };

  // The extra parentheses cause "(this)" to be an expression more complicated
  // than a name, so decltype returns a reference (see p.29 of EMC++)
  constexpr bool isThisRValue() const {
    return std::is_rvalue_reference_v<decltype((this))>;
  }

public:
  // Unneccesary, but being explicit never hurts
  Parser() = delete;
  // The "invoke_result" part ensures that the parameter is a function
  // (implements operator()). This ensures that this constructor doesn't
  // interfere with the move and copy constructors.
  template <
      typename Fn,
      typename = std::enable_if_t<std::is_convertible_v<
          result_t<T>,
          std::invoke_result_t<Fn, input_t&, size_t*>>>>
  Parser(Fn&& f);
  ~Parser() = default;
  Parser(const Parser&) = default;
  Parser(Parser&&) = default;
  Parser& operator=(const Parser&) = default;
  Parser& operator=(Parser&&) = default;

  /* Combinators */
  template <typename Fn, typename Par = std::invoke_result_t<Fn, T>>
  std::enable_if_t<is_parser_v<Par>, Par> andThen(Fn&& pGenFn) const;

  template <typename Fn, typename Par = std::invoke_result_t<Fn, T>>
  std::enable_if_t<is_parser_v<Par>, Par> andThenRef(Fn&& pGenFn) const;

  Parser<T> alt(Parser<T> nextParser) const;

  Parser<T> altRef(const Parser<T>& nextParser) const;

  // mapFn must accept an rvalue reference
  template <typename Fn, typename R = std::invoke_result_t<Fn, T&&>>
  Parser<std::decay_t<R>> andThenMap(Fn&& mapFn) const;

  template <typename R>
  Parser<std::pair<T, R>> combine(Parser<R> nextParser) const;

  template <typename R>
  Parser<std::pair<T, R>> combineRef(const Parser<R>& nextParser) const;

  template <
      typename Fn,
      typename =
          std::enable_if_t<std::is_convertible_v<bool, std::invoke_result_t<Fn, T>>>>
  Parser<T> verify(Fn&& boolFn) const;

  // Hacky method to enable the correct overload depending on whether T is a
  // char.
  // https://stackoverflow.com/questions/52077051/sfinae-enable-if-cannot-be-used-to-disable-this-declaration
  template <typename U = T>
  Parser<std::enable_if_t<std::is_same_v<U, char>, std::string>> many() const;
  template <typename U = T>
  Parser<std::enable_if_t<!std::is_same_v<U, char>, std::vector<T>>> many() const;

  template <typename U = T>
  Parser<std::enable_if_t<std::is_same_v<U, char>, std::string>> some() const;
  template <typename U = T>
  Parser<std::enable_if_t<!std::is_same_v<U, char>, std::vector<T>>> some() const;

  Parser<std::nullptr_t> ignore() const;

  template <typename R>
  Parser<R> ignoreAndThen(Parser<R> nextParser) const;

  template <typename R>
  Parser<R> ignoreAndThenRef(const Parser<R>& nextParser) const;

  template <typename R>
  Parser<T> thenIgnore(Parser<R> nextParser) const;

  template <typename R>
  Parser<T> thenIgnoreRef(const Parser<R>& nextParser) const;

  T parse(const std::string&) const;

  // Shared pointer makes a Parser copyable. If did not care about
  // that, we could use a unique pointer instead.
  std::shared_ptr<FnContainerAbstract> parseFn_;
};

namespace parsers {
  using namespace std;

  template <typename U>
  result_t<decay_t<U>> createReturnObject(U&& obj) {
    return make_optional(forward<U>(obj));
  }

  template <typename U>
  Parser<decay_t<U>> createBasic(U&& obj) {
    // Lambda is mutable so we can forward obj
    return Parser<decay_t<U>>{[obj = forward<U>(obj)](input_t&, size_t*) mutable {
      return createReturnObject(forward<U>(obj));
    }};
  }

  // TODO: WHY A NULLPTR
  const Parser<nullptr_t> success{
      [](input_t&, size_t*) { return createReturnObject(nullptr); }};

  template <typename U>
  const Parser<U> fail{
    [](input_t&, size_t*) -> result_t<U> {
      // TODO: Maybe pass errPos into this function
      // Do not set the error because then the error position will be at the end of
      // the failure, not at the chars before
      return {};
    }
  };

  const Parser<char> anyChar{
    [](input_t& input, size_t* errPos) -> result_t<char> {
      if (input.peek() == EOF) {
        input.clear();
        *errPos = input.tellg();
        return {};
      }

      return createReturnObject(input.get());
    }
  };

  // "inline" to prevent redefinition linker error
  inline Parser<char> thisChar(char c) {
    return anyChar.verify([c](char k) { return k == c; });
  }

  const Parser<unsigned> anyDigit{
      anyChar.verify([](char c) { return (bool)isdigit(c); }).andThenMap([](char c) {
        return unsigned(c - '0');
      })};

  // Not worrying about overflow
  const Parser<unsigned> anyUnsigned{
      anyDigit.some().andThenMap([](vector<unsigned>&& nums) {
        unsigned result = 0;
        for (auto iter = nums.begin(); iter != nums.end(); ++iter) {
          result *= 10;
          result += *iter;
        }
        return result;
      })};

  // Not worrying about overflow or conversion errors
  const Parser<int> anyInt{thisChar('-')
                               .ignoreAndThen(anyUnsigned)
                               .andThenMap([](unsigned&& num) { return -num; })
                               .alt(anyUnsigned)
                               .andThenMap([](unsigned&& num) { return (int)num; })};

  const Parser<double> decimal(thisChar('.')
                                   .ignoreAndThen(anyDigit.many())
                                   .andThenMap([](vector<unsigned>&& nums) {
                                     double dec = 0;
                                     for (auto iter = nums.rbegin(); iter != nums.rend();
                                          ++iter) {
                                       dec += *iter;
                                       dec /= 10;
                                     }
                                     return dec;
                                   }));

  const Parser<double> anyUDouble{
      anyUnsigned.combine(decimal)
          .andThenMap([](pair<unsigned, double>&& wholeAndDec) {
            return wholeAndDec.first + wholeAndDec.second;
          })
          .alt(anyUnsigned.andThenMap([](int n) { return (double)n; }))};

  const Parser<double> anyDouble{thisChar('-')
                                     .ignoreAndThen(anyUDouble)
                                     .andThenMap([](double d) { return -d; })
                                     .alt(anyUDouble)};

  const Parser<string> whitespace{
      anyChar.verify([](char c) { return (bool)isspace(c); }).many()};

  template <typename U>
  Parser<U> skipws(Parser<U> p) {
    return whitespace.ignoreAndThen(move(p));
  }
}  // namespace parsers

#include "parser_impl.hpp"

#endif
