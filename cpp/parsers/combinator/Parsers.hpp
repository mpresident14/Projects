#ifndef PARSERS_HPP
#define PARSERS_HPP

#include "AltParser.hpp"
#include "CharParser.hpp"
#include "ConditionalParser.hpp"
#include "IgnoreParser.hpp"
#include "LazyParser.hpp"
#include "ManyParser.hpp"
#include "MapParser.hpp"
#include "SequenceParser.hpp"
#include "StringParser.hpp"

#include <stdlib.h>

// TODO: Put this in templates.cpp
// template <typename T>
// struct p_result;

// template <template <typename> class P, typename T>
// struct p_result<P<T>> {};

namespace parsers {
  /**************************************************************************
   *                              COMBINATORS
   **************************************************************************/
  template <typename... ParserTypes>
  AltParser<parsers::p_first_t<ParserTypes...>, std::decay_t<ParserTypes>...>
  alt(ParserTypes&&... parsers) {
    return AltParser<
        parsers::p_first_t<ParserTypes...>, std::decay_t<ParserTypes>...>(
        std::forward<ParserTypes>(parsers)...);
  }

  template <typename... ParserTypes>
  SequenceParser<
      parsers::p_results_filtered_t<ParserTypes...>,
      std::decay_t<ParserTypes>...>
  seq(ParserTypes&&... parsers) {
    return SequenceParser<
        parsers::p_results_filtered_t<ParserTypes...>,
        std::decay_t<ParserTypes>...>(std::forward<ParserTypes>(parsers)...);
  }

  template <typename P>
  ManyParser<
      std::conditional_t<
          std::is_same_v<parsers::p_result_t<P>, char>, std::string,
          std::conditional_t<
              std::is_same_v<parsers::p_result_t<P>, parsers::ignore_t>,
              parsers::ignore_t, std::vector<parsers::p_result_t<P>>>>,
      std::decay_t<P>>
  many(P&& parser) {
    return ManyParser<
        std::conditional_t<
            std::is_same_v<parsers::p_result_t<P>, char>, std::string,
            std::conditional_t<
                std::is_same_v<parsers::p_result_t<P>, parsers::ignore_t>,
                parsers::ignore_t, std::vector<parsers::p_result_t<P>>>>,
        std::decay_t<P>>(std::forward<P>(parser));
  }

  template <typename P>
  auto some(P&& parser) {
    return transform(seq(parser, many(parser)), [](auto&& tup) {
      auto& vec = std::get<1>(tup);
      vec.insert(vec.begin(), std::move(std::get<0>(tup)));
      return std::move(vec);
    });
  }

  template <typename P>
  IgnoreParser<std::decay_t<P>> skip(P&& parser) {
    return IgnoreParser<std::decay_t<P>>(std::forward<P>(parser));
  }

  template <typename P, typename F>
  ConditionalParser<p_result_t<P>, std::decay_t<F>, std::decay_t<P>> doOnlyIf(
      P&& parser, F&& condFn) {
    return ConditionalParser<p_result_t<P>, std::decay_t<F>, std::decay_t<P>>(
        std::forward<P>(parser), std::forward<F>(condFn));
  }

  template <typename P, typename F>
  MapParser<
      std::decay_t<std::invoke_result_t<F, p_result_t<P>&&>>, std::decay_t<F>,
      std::decay_t<P>>
  transform(P&& parser, F&& mapFn) {
    return MapParser<
        std::decay_t<std::invoke_result_t<F, p_result_t<P>&&>>, std::decay_t<F>,
        std::decay_t<P>>(std::forward<P>(parser), std::forward<F>(mapFn));
  }

  /**************************************************************************
   *                            COMMON PARSERS
   **************************************************************************/
  CharParser anyChar() {
    static CharParser p;
    return p;
  }

  auto thisChar(char c) {
    return doOnlyIf(anyChar(), [c](char d) { return c == d; })
        .setCustomErrMsg(std::string(1, c));
  }

  StringParser thisString(const std::string& str) { return StringParser(str); }
  StringParser thisString(std::string&& str) { return StringParser(str); }
  StringParser thisString(const char* str) { return StringParser(str); }

  auto whiteSpace =
      many(doOnlyIf(anyChar(), [](char c) { return isspace(c); }));

  // TODO: This is really inefficient and super common. Define a new parser
  // instead.
  template <typename P1, typename P2>
  auto ignoreAndThen(P1&& toIgnore, P2&& parser) {
    return transform(
        seq(skip(std::forward<P1>(toIgnore)), std::forward<P2>(parser)),
        [](auto&& tup) { return std::move(std::get<0>(tup)); });
  }

  template <typename P1, typename P2>
  auto doThenIgnore(P1&& parser, P2&& toIgnore) {
    return transform(
        seq(std::forward<P1>(parser), skip(std::forward<P2>(toIgnore))),
        [](auto&& tup) { return std::move(std::get<0>(tup)); });
  }

  template <typename P>
  auto skipWs(P&& parser) {
    return ignoreAndThen(whiteSpace, std::forward<P>(parser));
  }

  auto anyDigitChar =
      doOnlyIf(anyChar(), [](char c) { return '0' <= c && c <= '9'; });

  auto anyULong = transform(
                      skipWs(some(anyDigitChar)),
                      [](std::string&& str) { return stoul(str); })
                      .setCustomErrMsg("an object of type <unsigned long>");

  template <typename T>
  LazyParser<T> lazy() {
    return LazyParser<T>();
  }
}  // namespace parsers

#endif
