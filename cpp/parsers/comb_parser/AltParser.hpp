#ifndef ALT_PARSER_HPP
#define ALT_PARSER_HPP

#include "Parser.hpp"

#include <tuple>
#include <utility>

template <typename T2, typename... PTypes>
class AltParser;

namespace parsers {
  template <typename... ParserTypes>
  AltParser<p_first_t<ParserTypes...>, std::decay_t<ParserTypes>...> alt(
      ParserTypes&&... parsers);
}

template <typename T, typename... ParserTypes>
class AltParser : public Parser<T, AltParser<T, ParserTypes...>> {
  template <typename T2, typename F, typename P2>
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
  friend AltParser<parsers::p_first_t<PTypes...>, std::decay_t<PTypes>...>
  parsers::alt(PTypes&&... parsers);

private:
  AltParser(const ParserTypes&... parsers)
      : parsers_(std::tuple<ParserTypes...>(parsers...)) {}

  AltParser(ParserTypes&&... parsers)
      : parsers_(
            std::tuple<ParserTypes...>(std::forward<ParserTypes>(parsers)...)) {
  }

  virtual std::optional<T> apply(std::istream& input) override {
    return applyHelper<0>(input);
  }

  /* Recursive tuple iteration via templates */
  // Base case
  template <int I, std::enable_if_t<I == sizeof...(ParserTypes), int> = 0>
  std::optional<T> applyHelper(std::istream&) {
    return {};
  }

  // General case
  template <int I, std::enable_if_t<I != sizeof...(ParserTypes), int> = 0>
  std::optional<T> applyHelper(std::istream& input) {
    auto& p = static_cast<parsers::rm_ref_wrap_t<
        std::tuple_element_t<I, std::tuple<ParserTypes...>>>&>(
        std::get<I>(parsers_));

    auto optResult = p.apply(input);

    if (optResult.has_value()) {
      return optResult;
    }
    return applyHelper<I + 1>(input);
  }

  virtual std::string getErrMsgs(std::istream& input) override {
    if (!this->customErrMsg_.empty()) {
      return this->myErrMsg(input);
    }

    std::stringstream sstream;
    sstream << "All alternatives failed." << '\n';
    getErrMsgsHelper<0>(input, sstream);
    return sstream.str();
  }

  /* Recursive tuple iteration via templates */
  // Base case
  template <int I, std::enable_if_t<I == sizeof...(ParserTypes), int> = 0>
  void getErrMsgsHelper(std::istream&, std::stringstream&) {}

  // General case
  template <int I, std::enable_if_t<I != sizeof...(ParserTypes), int> = 0>
  void getErrMsgsHelper(std::istream& input, std::stringstream& sstream) {
    auto& p = static_cast<parsers::rm_ref_wrap_t<
        std::tuple_element_t<I, std::tuple<ParserTypes...>>>&>(
        std::get<I>(parsers_));

    sstream << "\t" << I << ": " << p.getErrMsgs(input) << '\n';
    getErrMsgsHelper<I + 1>(input, sstream);
  }

  std::tuple<ParserTypes...> parsers_;
};

#endif
