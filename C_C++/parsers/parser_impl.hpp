#include <string>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <utility>

template<typename T>
template<typename U>
Parser<U> Parser<T>::createBasic(U&& obj)
{
    // Lambda is mutable so we can forward obj
    return Parser<U>{
        [obj = std::forward<U>(obj)](input_t& input) mutable {
            return std::make_optional(std::make_pair(std::forward<U>(obj), input));
        }
    };
}

template<typename T>
template<typename Fn>
Parser<T>::Parser()
    : parseFn_{ 
        std::make_shared<FnContainer<Fn>>(
            [](input_t& input) -> result_t<T> {
                return {};
            }
        )
    }
{}

// Only need to specify default template arg (Void) once (in header file)
template<typename T>
template<typename Fn, typename Void> 
Parser<T>::Parser(Fn&& f)
    : parseFn_{ std::make_shared<FnContainer<Fn>>(std::forward<Fn>(f)) } 
{}

template<typename T>
T Parser<T>::parse(input_t& input) const
{
    using namespace std;

    result_t<T> optResult = (*parseFn_)(input);
    if (!optResult.has_value()) {
        throw invalid_argument("No parse for input \"" + input + "\"");
    }

    pair<T, string>& pairResult = optResult.value();
    string& rest = get<1>(pairResult);
    if (!rest.empty()) {
        throw invalid_argument("No parse: \"" + rest + "\" remained.");
    }
    
    // Need "move" since the parsed object is inside the pair.
    // Prevents a call to the copy constructor.
    return move(get<0>(pairResult));
}

template<typename T>
template<typename Fn, typename Par>
std::enable_if_t<is_parser_v<Par>, Par> Parser<T>::andThen(Fn&& pGenFn) const
{
    using namespace std;
    
    return {
        // Have to capture this by value in case we call andThen with a temporary Parser object (e.g.
        // with chaining)
        // This must be mutable to call pGenFn's non-const () operator
        [=, *this, pGenFn = forward<Fn>(pGenFn)](input_t& input) mutable
            -> result_t<is_parser_pt<std::invoke_result_t<Fn, T>>> {
                // Run first parser
                result_t<T> optResult = (*parseFn_)(input);
                // If first parser fails, fail entire thing
                if (!optResult.has_value()) {
                    return {};
                }

                pair<T, string>& pairResult = optResult.value();
                T& result = get<0>(pairResult);
                auto nextParser = pGenFn(move(result));
                // Run the next parser on the rest of the string
                return (*(nextParser.parseFn_))(get<1>(pairResult));
        }
    };
}

// TODO: expand this to use a tuple instead of a pair (for longer chains)
template<typename T>
template<typename R>
Parser<std::pair<T,R>> Parser<T>::combine(const Parser<R>& nextParser) const
{
    using namespace std;

    return andThen(
        // Parser must be captured by value in case object to which nextParser refers goes out of 
        // scope
        [nextParser](T&& obj1) {
            return nextParser.andThen(
                // In order to forward obj1, the lambda must be mutable
                [obj1 = forward<T>(obj1)](R&& obj2) mutable {
                    return Parser::createBasic(make_pair(forward<T>(obj1), forward<R>(obj2)));
                });
        });
}
