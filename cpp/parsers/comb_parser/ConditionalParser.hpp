#ifndef CONDITIONAL_PARSER_HPP
#define CONDITIONAL_PARSER_HPP

#include "Parser.hpp"


template <typename T, typename F, typename P>
class ConditionalParser;

namespace parsers
{
    template<typename P, typename F>
    ConditionalParser<p_result_t<P>, std::decay_t<F>, std::decay_t<P>>
    doOnlyIf(P&& parser, F&& condFn);
}

template <typename T, typename F, typename P>
class ConditionalParser: public Parser<T> {

    template <typename T2, typename F2, typename P2>
    friend class MapParser;

    template<typename T2, typename F2, typename P2>
    friend class ConditionalParser;

    template <typename T2, typename... PTypes>
    friend class AltParser;

    template <typename T2, typename... PTypes>
    friend class SequenceParser;

    template <typename T2, typename P2>
    friend class ManyParser;

    template <typename P2>
    friend class IgnoreParser;

    template<typename P2, typename F2>
    friend ConditionalParser<parsers::p_result_t<P2>, std::decay_t<F2>, std::decay_t<P2>>
    parsers::doOnlyIf(P2&& parser, F2&& condFn);

private:
    ConditionalParser(const P& parser, const F& condFn)
        : parser_(parser), condFn_(condFn) {}

    ConditionalParser(const P& parser, F&& condFn)
        : parser_(parser), condFn_(std::move(condFn)) {}

    ConditionalParser(P&& parser, const F& condFn)
        : parser_(std::move(parser)), condFn_(condFn) {}

    ConditionalParser(P&& parser, F&& condFn)
        : parser_(std::move(parser)), condFn_(std::move(condFn)) {}



    virtual std::optional<T> apply(std::istream& input) const override
    {
        size_t oldPos = input.tellg();
        std::optional<T> optResult =
            static_cast<const parsers::rm_ref_wrap_t<P>&>(parser_).apply(input);
        if (optResult.has_value() && condFn_(optResult.value())) {
            return optResult;
        }

        // TODO: optResult will leak upon condFn failure if T is a pointer

        // Restore the position if the condFn fails.
        input.clear();
        input.seekg(oldPos);
        return {};
    }

    P parser_;
    F condFn_;
};

#endif
