#ifndef SEQUENCE_PARSER_HPP
#define SEQUENCE_PARSER_HPP

#include "Parser.hpp"

#include <any>
#include <tuple>
#include <utility>
#include <variant>

template <typename T, typename... PTypes>
class SequenceParser;

namespace parsers {
  template <typename... PTypes>
  SequenceParser<p_results_filtered_t<PTypes...>, std::decay_t<PTypes>...> seq(
      PTypes&&... parsers);
}

template <typename T, typename... ParserTypes>
class SequenceParser : public Parser<T, SequenceParser<T, ParserTypes...>> {
  static constexpr size_t NUM_RESULTS = std::tuple_size_v<T>;

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

private:
  SequenceParser(const ParserTypes&... parsers)
      : parsers_(std::tuple<ParserTypes...>(parsers...)) {}

  SequenceParser(ParserTypes&&... parsers)
      : parsers_(std::tuple<ParserTypes...>(std::move(parsers)...)) {}

  virtual std::optional<T> apply(std::istream& input) override {
    std::cout << "NUMRESULTS = " << NUM_RESULTS << std::endl;
    size_t oldPos = input.tellg();

    std::optional<T> optResult =
        applyHelper1(input, std::make_index_sequence<std::tuple_size_v<T>>());
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

  template <size_t... Ints>
  std::optional<T> applyHelper1(
      std::istream& input, const std::index_sequence<Ints...>& intSeq) {
    std::array<std::variant<std::remove_reference_t<std::tuple_element_t<Ints, T>>...>, NUM_RESULTS> results;
    std::cout << "Before applyHelper2<0>" << std::endl;
    return applyHelper2<0>(input, results, 0, intSeq);
  }

  /* Recursive tuple iteration via templates */
  // Base case
  template <
      int I, std::enable_if_t<I == sizeof...(ParserTypes), int> = 0,
      size_t... Ints>
  std::optional<T> applyHelper2(
      std::istream&,
      std::array<std::variant<std::remove_reference_t<std::tuple_element_t<Ints, T>>...>, NUM_RESULTS>& results,
      size_t, const std::index_sequence<Ints...>&) {
    return std::make_tuple(std::get<Ints>(results[Ints])...);
  }

  // If not an ignore_t (should go in result tuple)
  template <
      int I,
      std::enable_if_t<
          // conjunction short-circuits, i.e. doesn't instantiate ::value
          // if it doesn't have to, so we hide the tuple_element_t request
          // in the ::value member of is_ignore.
          std::conjunction_v<
              std::bool_constant<I != sizeof...(ParserTypes)>,
              std::negation<parsers::is_ignore<I, std::tuple<ParserTypes...>>>>,
          int> = 0,
      size_t... Ints>
  std::optional<T> applyHelper2(
      std::istream& input,
      std::array<std::variant<std::remove_reference_t<std::tuple_element_t<Ints, T>>...>, NUM_RESULTS>& results,
      size_t i, const std::index_sequence<Ints...>& intSeq) {
    auto& p = parsers::getParser<I>(parsers_);
    auto optResult = p.apply(input);

    if (optResult.has_value()) {
      results[i].emplace(optResult.value());
      return applyHelper2<I + 1>(input, results, i + 1, intSeq);
    }

    // failedParser_ = &p;
    return {};
  }

  // If an ignore_t (should not go in result tuple)
  template <
      int I,
      std::enable_if_t<
          std::conjunction_v<
              std::bool_constant<I != sizeof...(ParserTypes)>,
              parsers::is_ignore<I, std::tuple<ParserTypes...>>>,
          int> = 0,
      size_t... Ints>
  std::optional<T> applyHelper2(
      std::istream& input,
      std::array<std::variant<std::remove_reference_t<std::tuple_element_t<Ints, T>>...>, NUM_RESULTS>& results,
      size_t i, const std::index_sequence<Ints...>& intSeq) {
    auto& p = parsers::getParser<I>(parsers_);
    auto optResult = p.apply(input);

    if (optResult.has_value()) {
      return applyHelper2<I + 1>(input, results, i, intSeq);
    }

    // failedParser_ = &p;
    return {};
  }

  virtual std::string getErrMsgs(std::istream& input) override {
    if (!this->customErrMsg_.empty()) {
      return this->myErrMsg(input);
    }
    // return failedParser_->getErrMsgs(input);
    return "";
  }

  std::tuple<ParserTypes...> parsers_;
};

#endif
