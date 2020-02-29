#ifndef LAZY_PARSER_HPP
#define LAZY_PARSER_HPP

#include "Parser.hpp"

#include <utility>

namespace parsers
{
    template <typename T>
    LazyParser<T> lazy();
}

template <typename T>
class LazyParser: public Parser<T, LazyParser<T>> {

    template <typename T2, typename F2, typename P2>
    friend class MapParser;

    template<typename T2, typename F2, typename P2>
    friend class ConditionalParser;

    template <typename T2, typename... PTypes>
    friend class AltParser;

    template <typename T2, typename... PTypes>
    friend class SequenceParser;

    template<typename T2, typename Derived>
    friend class Parser;

    template <typename T2>
    friend LazyParser<T2> parsers::lazy();

public:
    ~LazyParser() { delete parser_; }

    template <typename P>
    void set(P&& parser)
    {
        if (parser_) {
            delete parser_;
        }
        parser_ = new P(parser);
    }


private:
    LazyParser() : parser_(nullptr) {}

    virtual std::optional<T> apply(const std::string& input, size_t *pos) const override
    {
        return parser_->apply(input, pos);
    }

    ParserBase<T> *parser_;
};

#endif
