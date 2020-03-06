#ifndef SEQUENCE_PARSER_HPP
#define SEQUENCE_PARSER_HPP

#include "Parser.hpp"

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
      : parsers_{ (new ParserTypes(static_cast<const parsers::rm_ref_wrap_t<ParserTypes>&>(parsers)))...  } {}

  SequenceParser(ParserTypes&&... parsers)
      : parsers_{ (new ParserTypes(std::move(static_cast<const parsers::rm_ref_wrap_t<ParserTypes>&>(parsers))))... } {}

  virtual std::optional<T> apply(std::istream& input) override {
    size_t oldPos = input.tellg();
    std::optional<T> optResult = applyHelper(input, std::make_index_sequence<std::tuple_size_v<T>>());
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
  std::optional<T> applyHelper(std::istream& input, std::index_sequence<Ints...>) {
    std::variant<std::remove_reference_t<decltype(std::get<Ints...>(std::declval<T>()))>> results[std::tuple_size_v<T>];
    size_t varI = 0;
    for (ParserBase *p : parsers_) {
      auto optResult = p->apply(input);
      if (!optResult.has_value()) {
        return {};
      }
      results[varI] = std::move(optResult.value());
      ++varI;
    }
    return std::optional(std::make_tuple(std::get<Ints>(results[Ints])...));
  }


  virtual std::string getErrMsgs(std::istream& input) override {
    if (!this->customErrMsg_.empty()) {
      return this->myErrMsg(input);
    }
    // return failedParser_->getErrMsgs(input);
    return "";
  }

  std::array<ParserBase *, sizeof...(ParserTypes)> parsers_;
};

#endif
