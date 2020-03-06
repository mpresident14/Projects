#ifndef MAP_PARSER_HPP
#define MAP_PARSER_HPP

#include "Parser.hpp"

#include <utility>

template <typename T, typename F, typename P>
class MapParser;

namespace parsers {
  template <typename P, typename F>
  MapParser<
      std::decay_t<std::invoke_result_t<F, p_result_t<P>&&>>, std::decay_t<F>,
      std::decay_t<P>>
  transform(P&& parser, F&& mapFn);
}

template <typename T, typename F, typename P>
class MapParser : public Parser<T, MapParser<T, F, P>> {
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

  template <typename P2, typename F2>
  friend MapParser<
      std::decay_t<std::invoke_result_t<F2, parsers::p_result_t<P2>&&>>,
      std::decay_t<F2>, std::decay_t<P2>>
  parsers::transform(P2&& parser, F2&& mapFn);

private:
  // Note that for member functions, the typename P has already been deduced at
  // the class level, so P&& is not a universal reference.
  MapParser(const P& parser, const F& mapFn) : parser_(parser), mapFn_(mapFn) {}

  MapParser(const P& parser, F&& mapFn)
      : parser_(parser), mapFn_(std::move(mapFn)) {}

  MapParser(P&& parser, const F& mapFn)
      : parser_(std::move(parser)), mapFn_(mapFn) {}

  MapParser(P&& parser, F&& mapFn)
      : parser_(std::move(parser)), mapFn_(std::move(mapFn)) {}

  virtual std::optional<T> apply(std::istream& input) override {
    auto optResult =
        static_cast<parsers::rm_ref_wrap_t<P>&>(parser_).apply(input);
    if (optResult.has_value()) {
      return std::optional<T>(mapFn_(std::move(optResult.value())));
    }
    this->errPos_ = input.tellg();
    return {};
  }

  virtual std::string getErrMsgs(std::istream& input) override {
    if (!this->customErrMsg_.empty()) {
      return this->myErrMsg(input);
    }
    return parser_.getErrMsgs(input) + " (To transform into type <" +
           boost::typeindex::type_id_with_cvr<T>().pretty_name() + ">)";
  }

  P parser_;
  F mapFn_;
};

#endif
