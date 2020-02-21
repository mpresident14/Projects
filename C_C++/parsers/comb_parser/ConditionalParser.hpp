#ifndef CONDITIONAL_CHAR_PARSER_HPP
#define CONDITIONAL_CHAR_PARSER_HPP 1

#include "ParserImpl.hpp"

#include <type_traits>
#include <memory>

template<typename T>
class ConditionalParser: public ParserImpl<T> {
public:
    ConditionalParser(Parser<T> p, bool (*condFn)(T&))
        : parser_(p), condFn_(condFn) {}

// protected:
    using OptResult = typename ParserImpl<T>::OptResult;

    OptResult maybeParse(std::string_view input) const override
    {
        std::cout << "DERIVED COND" << std::endl;
        OptResult opt = parserImpl_->maybeParse(input);

        if (opt.has_value()) {
            T& result = std::get<0>(opt.value());
            if (condFn_(result)) {
                return this->makeResult(result, std::get<1>(opt.value()));
            }
        }
        return {};
    }

private:
    Parser<T> parser_;
    // This won't allow capturing lambdas, but it's cheaper than std::function
    bool (*condFn_)(T&);
};

#endif

