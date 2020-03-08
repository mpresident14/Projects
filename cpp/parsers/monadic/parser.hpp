#ifndef PARSER_HPP
#define PARSER_HPP

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

template <typename T, template <typename> class PtrType>
class Parser;

// TODO: why isn't this inside the class?
template <typename T>
using result_t = std::optional<T>;
using input_t = std::istream;

template <typename T>
struct RawPtr {
  RawPtr() : ptr_(nullptr){}
  ~RawPtr() { delete ptr_; }
  void setToShared(std::shared_ptr<T> sPtr) {
    if (ptr_) {
      delete ptr_;
    }
    ptr_ = sPtr.get();
  }
  T operator*() { return *ptr_; }
  T* ptr_;
};


// These templates allow detection of a Parser of any type and get the type of
// the Parser.
template <typename P>
struct parser_info {
  using type = typename P::result_type;
  // static constexpr bool value = true;
};

// template <typename T>
// static inline constexpr bool is_parser_v = parser_info<T>::value;

template <typename P>
using ptype_t = typename parser_info<P>::type;

template <typename T>
struct ptr_type;
template <typename T>
struct ptr_type<std::shared_ptr<T>> {
  using type = T;
};
template <typename T>
struct ptr_type<RawPtr<T>> {
  using type = T;
};
template <typename T>
using ptrtype_t = typename ptr_type<T>::type;

template <typename T>
struct is_rawptr : std::false_type {};
template <typename T>
struct is_rawptr<RawPtr<T>> : std::true_type {};
template <typename T>
constexpr bool is_rawptr_v = is_rawptr<T>::value;

/***************************************************************************************
 *                                PARSER CLASS GUARANTEE
 * -------------------------------------------------------------------------------------
 * Upon failure, the parser shall reset the input stream to the point at which it began
 * parsing and set errPos to this value if this value is larger than (i.e., further
 * along in the input) errPos (see andThen() for the only exception).
 ***************************************************************************************/

template <typename T, template <typename> class PtrType = std::shared_ptr>
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
  using result_type = T;

  template <typename U = PtrType<FnContainerAbstract>,
      std::enable_if_t<is_rawptr_v<U>, int> = 0>
  Parser() : parseFn_(std::make_shared<RawPtr<FnContainerAbstract>>(RawPtr<FnContainerAbstract>())) {}
  // The "invoke_result" part ensures that the parameter is a function
  // (implements operator()). This ensures that this constructor doesn't
  // interfere with the move and copy constructors.
  template <typename Fn,
      typename U = PtrType<FnContainerAbstract>,
      std::enable_if_t<!is_rawptr_v<U> &&
                           std::is_convertible_v<result_t<T>,
                               std::invoke_result_t<Fn, input_t&, size_t*>>,
          int> = 0>
  Parser(Fn&& f)
      : parseFn_(std::make_shared<std::shared_ptr<FnContainerAbstract>>(
            std::make_shared<FnContainer<Fn>>(std::forward<Fn>(f)))) {}
  ~Parser() = default;
  Parser(const Parser&) = default;
  Parser(Parser&&) = default;
  // All parser copies are shallow by design. The only non-const method is
  // set(), which will change all other shallow copies as well, but set()
  // should only be called on fail<T> for recursive grammars.
  Parser& operator=(const Parser&) = default;
  Parser& operator=(Parser&&) = default;

  /* Combinators */
  template <typename Fn, typename P = std::invoke_result_t<Fn, T&&>>
  P andThen(Fn&& pGenFn) const;

  Parser<T> alt(Parser<T> nextParser) const;

  // mapFn must accept an rvalue reference
  // TODO: Decay is questionable
  template <typename Fn, typename R = std::invoke_result_t<Fn, T&&>>
  Parser<std::decay_t<R>> andThenMap(Fn&& mapFn) const;

  template <typename R>
  Parser<std::pair<T, R>> combine(Parser<R> nextParser) const;

  // TODO: Replace with c++20 concepts (and everywhere else where applicable)
  template <typename Fn,
      typename = std::enable_if_t<std::is_same_v<bool, std::invoke_result_t<Fn, T>>>>
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
  Parser<T> thenIgnore(Parser<R> nextParser) const;

  T parse(const std::string&) const;

  // Lazily assign parser for recursive grammars
  template <typename R>
  void set(const Parser<R>& other);

  // template <typename R>
  // void set(Parser<R>&& other);

  static Parser<T, RawPtr> lazy() {
    return Parser<T, RawPtr>();
  }

  // Second shared_ptr is specifically to make it possible to
  // assign the parser lazily, which makes recursive grammars
  // easier to implement.
  std::shared_ptr<PtrType<FnContainerAbstract>> parseFn_;
};


namespace parsers {
  using namespace std;

  template <typename U>
  result_t<decay_t<U>> createReturnObject(U&& obj) {
    // TODO: For all functions with similar comments, restrict the value to rvalue refs
    // only. Safe to move obj because functions calling it will not need it again.
    return make_optional(move(obj));
  }

  template <typename U>
  Parser<decay_t<U>> createBasic(U&& obj) {
    // Lambda is mutable so we can move obj
    return Parser<decay_t<U>>{[obj = move(obj)](input_t&, size_t*) mutable {
      return createReturnObject(move(obj));
    }};
  }

  Parser<nullptr_t> success() {
    return [](input_t&, size_t*) { return createReturnObject(nullptr); };
  };

  template <typename U>
  Parser<U> fail() {
    return [](input_t&, size_t*) -> result_t<U> { return {}; };
  };

  const Parser<char> anyChar{[](input_t& input, size_t* errPos) -> result_t<char> {
    if (input.peek() == EOF) {
      input.clear();
      *errPos = input.tellg();
      return {};
    }

    return createReturnObject(input.get());
  }};

  // TODO: Why?
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
