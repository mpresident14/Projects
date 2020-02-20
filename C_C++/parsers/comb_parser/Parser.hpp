#ifndef PARSER_HPP
#define PARSER_HPP 1

#include <string_view>
#include <optional>
#include <utility>
#include <stdexcept>

template<typename Result>
class Parser {
public:
    using OptResult = std::optional<std::pair<Result, std::string_view>>;

    virtual ~Parser() { delete overrideErrMsg_; }

    Result parse(const std::string& input)
    {
        OptResult opt = maybeParse(input);
        if (opt.has_value()) {
            std::string_view& remaining = std::get<1>(opt.value());
            if (remaining.empty()) {
                return std::move(std::get<0>(opt.value()));
            } else {
                // TODO: use std::literals::string_view_literals
                throw std::invalid_argument("No parse: " + std::string(remaining) + " remained.");
            }
        } else {
            throw std::invalid_argument(overrideErrMsg_ ? overrideErrMsg_ : getErrorMsg());
        }
    }

protected:
    Parser() : overrideErrMsg_{nullptr} {}
    virtual OptResult maybeParse(std::string_view input) = 0;
    virtual std::string getErrorMsg() = 0;

    OptResult makeResult(Result result, std::string_view rest)
    {
        return std::make_optional(std::make_pair(result, rest));
    }

    char *overrideErrMsg_;

};

#endif
