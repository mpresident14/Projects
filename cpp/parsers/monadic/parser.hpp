#ifndef PARSER_HPP
#define PARSER_HPP

#include "simple_shptr.hpp"

#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/***************************************************************************************
 *                                PARSER CLASS GUARANTEE
 * -------------------------------------------------------------------------------------
 * Upon failure, the parser shall reset the input stream to the point at which it began
 * parsing and set errPos to this value if this value is larger than (i.e., further
 * along in the input) errPos (see andThen() for the only exception).
 ***************************************************************************************/

template <typename T>
class Parser {
private:
  struct FnContainerAbstract {
    virtual ~FnContainerAbstract() {}
    virtual std::optional<T> operator()(std::istream&, size_t*) = 0;
  };

  template <typename Fn>
  struct FnContainer : FnContainerAbstract {
    FnContainer(Fn&& f) : f_{std::forward<Fn>(f)} {}
    // Since we have a mutable lambdas (e.g. in createBasic() and combine()),
    // the () operator can change the variables in the closure (i.e. change f_),
    // so the () operator cannot be const.
    std::optional<T> operator()(std::istream& input, size_t* errPos) override {
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
  template <typename Fn,
      typename = std::enable_if_t<std::is_convertible_v<std::optional<T>,
          std::invoke_result_t<Fn, std::istream&, size_t*>>>>
  Parser(Fn&& f);
  ~Parser();
  Parser(const Parser&);
  Parser(Parser&&);
  // All parser copies are shallow by design. The only non-const method is
  // set(), which will change all other shallow copies as well, but set()
  // should only be called on fail<T> for recursive grammars.
  Parser& operator=(const Parser&) = default;
  Parser& operator=(Parser&&) = default;

  /* Combinators */
  template <typename Fn, typename P = std::invoke_result_t<Fn, T&&>>
  P andThen(Fn&& pGenFn) const;

  Parser<T> alt(Parser<T> nextParser) const;

  // mapFn must accept an rvalue reference and return a non-reference
  template <typename Fn, typename R = std::invoke_result_t<Fn, T&&>> requires !std::is_lvalue_reference_v<R>
  Parser<R> transform(Fn&& mapFn) const;

  template <typename R>
  Parser<std::pair<T, R>> combine(Parser<R> nextParser) const;

  template <typename A, typename B>
  Parser<std::tuple<T, A, B>> combine(Parser<A> p1, Parser<B> p2) const;

  template <typename A, typename B, typename C>
  Parser<std::tuple<T, A, B, C>> combine(Parser<A> p1, Parser<B> p2, Parser<C> p3) const;

  template <typename Fn,
      typename = std::enable_if_t<std::is_same_v<bool, std::invoke_result_t<Fn, T>>>>
  Parser<T> verify(Fn&& boolFn) const;

  // https://stackoverflow.com/questions/52077051/sfinae-enable-if-cannot-be-used-to-disable-this-declaration
  template <typename U = T>
  Parser<std::enable_if_t<std::is_same_v<U, char>, std::string>> many() const;
  template <typename U = T>
  Parser<std::enable_if_t<!std::is_same_v<U, char>, std::vector<T>>> many() const;

  template <typename U = T>
  Parser<std::enable_if_t<std::is_same_v<U, char>, std::string>> some() const;
  template <typename U = T>
  Parser<std::enable_if_t<!std::is_same_v<U, char>, std::vector<T>>> some() const;

  template <typename R>
  Parser<R> ignoreAndThen(Parser<R> nextParser) const;

  template <typename R>
  Parser<T> thenIgnore(Parser<R> nextParser) const;

  T parse(const std::string&) const;

  /* Lazily assign parser for recursive grammars */
  void set(Parser&& other);

  /*
   * unique_ptr is specifically to make it possible to
   * assign the parser lazily, which makes recursive grammars
   * easier to implement.
   */
  std::shared_ptr<std::unique_ptr<FnContainerAbstract>> parseFn_;
  /*
   * Keeps track of how many lazy parsers are pointing to a unique_ptr
   * so that we can properly break cycles manually on destruction.
   */
  size_t* lazyCount_ = nullptr;
};

namespace parsers {
  using namespace std;

  /* Detect if parser, and if so, what type */
  template <typename T>
  struct parser_info;

  template <typename T>
  struct parser_info<Parser<T>> {
    using type = T;
    static constexpr bool value = true;
  };

  template <typename T>
  static inline constexpr bool is_parser_v = parser_info<T>::value;

  template <typename T>
  using ptype_t = typename parser_info<T>::type;

  /* The 2 functions below should only take in rvalues */
  template <typename T>
  concept rvalue = std::is_rvalue_reference_v<T>;

  template <typename T>
  requires rvalue<T&&> optional<T> createReturnObject(T&& obj) {
    return make_optional(move(obj));
  }

  template <typename T>
  requires rvalue<T&&> Parser<T> createBasic(T&& obj) {
    // Lambda is mutable so we can move obj
    return Parser<T>{[obj = move(obj)](istream&, size_t*) mutable {
      return createReturnObject(move(obj));
    }};
  }

  /* fail() and lazy() are the same under the hood, but identifying them
   * differently makes the intentions of the programmer clear. */
  template <typename T>
  Parser<T> fail() {
    return [](istream&, size_t*) -> optional<T> { return {}; };
  }

  template <typename T>
  const auto lazy = fail<T>;

  const Parser<char> anyChar([](istream& input, size_t* errPos) -> optional<char> {
    if (input.peek() == EOF) {
      input.clear();
      *errPos = max(*errPos, (size_t) input.tellg());
      return {};
    }

    return createReturnObject(input.get());
  });

  Parser<char> thisChar(char c) {
    return anyChar.verify([c](char k) { return k == c; });
  }

  Parser<string> thisString(const string& str) {
    return [str](istream& input, size_t* errPos) -> optional<string> {
      size_t oldPos = input.tellg();
      for (const char& c : str) {
        char inC = input.get();
        if (inC == EOF || inC != c) {
          input.clear();
          input.seekg(oldPos);
          *errPos = max(*errPos, oldPos);
          return {};
        }
      }
      return optional(move(str));
    };
  }

  const Parser<unsigned> anyDigit =
      anyChar.verify([](char c) { return (bool)isdigit(c); }).transform([](char c) {
        return unsigned(c - '0');
      });

  // Not worrying about overflow
  const Parser<unsigned> anyUnsigned =
      anyDigit.some().transform([](vector<unsigned>&& nums) {
        unsigned result = 0;
        for (auto iter = nums.begin(); iter != nums.end(); ++iter) {
          result *= 10;
          result += *iter;
        }
        return result;
      });

  // Not worrying about overflow or conversion errors
  // clang-format off
  const Parser<int> anyInt =
      thisChar('-')
        .ignoreAndThen(anyUnsigned)
        .transform([](unsigned&& num) { return -num; })
        .alt(anyUnsigned)
        .transform([](unsigned&& num) { return (int)num; });
  // clang-format on

  const Parser<double> decimal =
      thisChar('.')
          .ignoreAndThen(anyDigit.many())
          .transform([](vector<unsigned>&& nums) {
            double dec = 0;
            for (auto iter = nums.rbegin(); iter != nums.rend(); ++iter) {
              dec += *iter;
              dec /= 10;
            }
            return dec;
          });

  const Parser<double> anyUDouble =
      anyUnsigned.combine(decimal)
          .transform([](pair<unsigned, double>&& wholeAndDec) {
            return wholeAndDec.first + wholeAndDec.second;
          })
          .alt(anyUnsigned.transform([](int n) { return (double)n; }));

  // clang-format off
  const Parser<double> anyDouble =
      thisChar('-')
          .ignoreAndThen(anyUDouble)
          .transform([](double d) { return -d; })
          .alt(anyUDouble);
  // clang-format on

  const Parser<string> whitespace =
      anyChar.verify([](char c) { return (bool)isspace(c); }).many();

  template <typename T>
  Parser<T> skipws(Parser<T> p) {
    return whitespace.ignoreAndThen(move(p));
  }
}  // namespace parsers

#include "parser_impl.hpp"

#endif
