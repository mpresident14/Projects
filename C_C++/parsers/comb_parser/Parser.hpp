#ifndef PARSER_HPP
#define PARSER_HPP 1

#include "ParserImpl.hpp"

template<typename T>
class Parser {
public:
    using OptResult = typename ParserImpl<T>::OptResult;

    Parser(ParserImpl<T> *impl) : impl_(impl) {}

    T parse(const std::string& input) const
    {
        OptResult opt = impl_->maybeParse(input);
        if (opt.has_value()) {
            std::string_view& remaining = std::get<1>(opt.value());
            if (remaining.empty()) {
                return std::move(std::get<0>(opt.value()));
            } else {
                // TODO: use std::literals::string_view_literals
                throw std::invalid_argument("No parse: " + std::string(remaining) + " remained.");
            }
        } else {
            // throw std::invalid_argument(overrideErrMsg_ ? overrideErrMsg_ : getErrorMsg());
            throw std::invalid_argument("No parse.");
        }
    }

// private:
    ParserImpl<T> *impl_;
};

#endif
