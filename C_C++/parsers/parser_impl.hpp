#include <string>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <utility>

template<typename T>
template<typename U>
Parser<U> Parser<T>::createBasic(U&& obj)
{
    return Parser<U>{
        [obj = std::forward<U>(obj)](input_t& input) {
            return std::make_optional(std::make_pair(obj, input));
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
        [this, pGenFn = forward<Fn>(pGenFn)](input_t& input) 
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

template<typename T>
template<typename R>
Parser<std::pair<T,R>> Parser<T>::combine(const Parser<R>& nextParser) const
{
    using namespace std;

    cout << "combine start" << endl;
    auto p1 = andThen(
        // Needs to be a copy in case the object to which nextParser refers goes out of scope
        [nextParser](T&& obj1) {
            cout << "outer lambda start" << endl;
            auto p2 = nextParser.andThen(
                [obj1 = forward<T>(obj1)](R&& obj2) {
                    cout << "inner lambda start" << endl;
                    auto ppair = Parser::createBasic(make_pair(obj1, obj2)); // TODO: Forward objs??? (need mutable)
                    cout << "inner lambda end" << endl;
                    return ppair;
                });
            cout << "outer lambda end" << endl;
            return p2;
        });
    cout << "combine end" << endl;
    return p1;
}
