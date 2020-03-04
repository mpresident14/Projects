#ifndef IGNORE_PARSER_HPP
#define IGNORE_PARSER_HPP

#include "Parser.hpp"

template <typename P>
class IgnoreParser;

namespace parsers {
  template <typename P>
  IgnoreParser<std::decay_t<P>> skip(P&& parser);
}

template <typename P>
class IgnoreParser : public Parser<parsers::ignore_t, IgnoreParser<P>> {
  template <typename T2, typename F2, typename P2>
  friend class MapParser;

  template <typename T2, typename F2, typename P3>
  friend class ConditionalParser;

  template <typename T2, typename... PTypes>
  friend class AltParser;

  template <typename T2, typename... PTypes>
  friend class SequenceParser;

  template <typename T2, typename P2>
  friend class ManyParser;

  template <typename P2>
  friend class IgnoreParser;

  template <typename P2>
  friend IgnoreParser<std::decay_t<P2>> parsers::skip(P2&& parser);

private:
  IgnoreParser(const P& parser) : parser_(parser) {}

  IgnoreParser(P&& parser) : parser_(std::move(parser)) {}

  virtual std::optional<parsers::ignore_t> apply(std::istream& input) override {
    auto optResult =
        static_cast<parsers::rm_ref_wrap_t<P>&>(parser_).apply(input);
    if (optResult.has_value()) {
      return std::optional(parsers::ignore_t());
    }

    return {};
  }

  virtual std::string getErrMsgs(std::istream& input) override {
    if (!this->customErrMsg_.empty()) {
      return this->myErrMsg(input);
    }
    return parser_.getErrMsgs(input);
  }

  P parser_;
};

#endif
