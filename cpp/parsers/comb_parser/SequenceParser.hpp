#ifndef SEQUENCE_PARSER_HPP
#define SEQUENCE_PARSER_HPP

#include "Parser.hpp"

#include <tuple>
#include <utility>

template <typename T, typename... PTypes>
class SequenceParser;

namespace parsers {
  template <typename... PTypes>
  SequenceParser<p_results_filtered_t<PTypes...>, std::decay_t<PTypes>...> seq(
      PTypes&&... parsers);

  template <typename P1, typename P2>
  SequenceParser<
        parsers::p_result_t<P2>,
        std::decay_t<decltype(skip(std::forward<P1>(std::declval<P1>())))>,
        std::decay_t<P2>> ignoreAndThen(P1&& toIgnore, P2&& parser);
}

template <typename T, typename... ParserTypes>
class SequenceParser : public Parser<T, SequenceParser<T, ParserTypes...>> {
  template <typename T2, typename F2, typename P2>
  friend class ConditionalParser;

  template <typename T2, typename F2, typename P2>
  friend class MapParser;

  template <typename T2, typename... PTypes>
  friend class AltParser;

  template <typename T2, typename... PTypes>
  friend class SequenceParser;

  template <typename T2, typename P2>
  friend class ManyParser;

  template <typename P2>
  friend class IgnoreParser;

  template <typename... PTypes>
  friend SequenceParser<
      parsers::p_results_filtered_t<PTypes...>, std::decay_t<PTypes>...>
  parsers::seq(PTypes&&... parsers);


  template <typename P1, typename P2>
  friend SequenceParser<
        parsers::p_result_t<P2>,
        std::decay_t<decltype(skip(std::forward<P1>(std::declval<P1>())))>,
        std::decay_t<P2>> parsers::ignoreAndThen(P1&& toIgnore, P2&& parser);

private:
  SequenceParser(const ParserTypes&... parsers)
      : parsers_(std::tuple<ParserTypes...>(parsers...)) {}

  // SequenceParser(ParserTypes&&... parsers)
  //     : parsers_(std::tuple<ParserTypes...>(std::move(parsers)...)) {}

  virtual std::optional<T> apply(std::istream& input) override {
    size_t oldPos = input.tellg();
    std::optional<T> optResult = applyHelper<0>(input, std::tuple<>());
    if (optResult.has_value()) {
      return optResult;
    }
    // TODO: optResult will leak upon condFn failure if T is a pointer

    // Reset position if parse failed
    input.clear();
    input.seekg(oldPos);
    this->errPos_ = oldPos;
    return optResult;
  }

  /**/
  template <size_t I, typename Tup, typename U = T, std::enable_if_t<!parsers::is_tuple_v<U>, int> = 0>
  std::optional<T> applyHelper(std::istream& input, Tup&&) {
    if (parsers::is_ignore_v<0, std::tuple<ParserTypes...>>) {
      auto ignoreResult = std::get<0>(parsers_).apply(input);
      if (ignoreResult.has_value()) {
        return std::get<1>(parsers_).apply(input);
      }
    } /* else {
      auto optResult = std::get<0>(parsers_).apply(input);
      if (optResult.has_value() && std::get<1>(parsers_).apply(input).has_value()) {
        return optResult;
      }
    } */
  }

  /* Recursive tuple iteration via templates */
  // Base case
  template <int I, typename U = T, std::enable_if_t<I == sizeof...(ParserTypes) && parsers::is_tuple_v<U>, int> = 0>
  std::optional<T> applyHelper(std::istream&, T&& tup) {
    return std::optional(std::move(tup));
  }

  // If not an ignore_t (should go in result tuple)
  template <
      int I, typename Tup,typename U = T,
      std::enable_if_t<
          // conjunction short-circuits, i.e. doesn't instantiate ::value
          // if it doesn't have to, so we hide the tuple_element_t request
          // in the ::value member of is_ignore.
          parsers::is_tuple_v<U> &&
          std::conjunction_v<
              std::bool_constant<I != sizeof...(ParserTypes)>,
              std::negation<parsers::is_ignore<I, std::tuple<ParserTypes...>>>>,
          int> = 0>
  std::optional<T> applyHelper(std::istream& input, Tup&& currentTuple) {
    auto& p = parsers::getParser<I>(parsers_);
    auto optResult = p.apply(input);

    if (optResult.has_value()) {
      return applyHelper<I + 1>(
          input, std::tuple_cat(
                     std::move(currentTuple), std::tuple(std::move(optResult.value()))));
    }

    failedParser_ = &p;
    return {};
  }

  // If an ignore_t (should not go in result tuple)
  template <
      int I, typename Tup, typename U = T,
      std::enable_if_t<
          parsers::is_tuple_v<U> &&
          std::conjunction_v<
              std::bool_constant<I != sizeof...(ParserTypes)>,
              parsers::is_ignore<I, std::tuple<ParserTypes...>>>,
          int> = 0>
  std::optional<T> applyHelper(std::istream& input, Tup&& currentTuple) {
    auto& p = parsers::getParser<I>(parsers_);
    auto optResult = p.apply(input);

    if (optResult.has_value()) {
      return applyHelper<I + 1>(input, std::forward<Tup>(currentTuple));
    }

    failedParser_ = &p;
    return {};
  }

  virtual std::string getErrMsgs(std::istream& input) override {
    if (!this->customErrMsg_.empty()) {
      return this->myErrMsg(input);
    }
    return failedParser_->getErrMsgs(input);
  }

  std::tuple<ParserTypes...> parsers_;
  ParserBase* failedParser_;
};

#endif
