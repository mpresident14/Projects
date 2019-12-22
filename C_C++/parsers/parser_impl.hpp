#include <string>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <utility>


template<typename T>
Parser<T>::Parser(const parse_fn_t& parseFn) : parseFn_{parseFn} {}

template<typename T>
Parser<T>::Parser(parse_fn_t&& parseFn) : parseFn_{ std::move(parseFn) } {}

template<typename T>
Parser<T>::Parser(T&& obj)
    : parseFn_{
        MovableFn<return_t, input_t&>{
            [=, obj = std::forward<T>(obj)](input_t& input) {
                return std::make_optional(std::make_pair(obj, input));
            }
        }
    }
{}

template<typename T>
T Parser<T>::parse(input_t& input) const
{
    using namespace std;

    return_t optResult = parseFn_(input);
    if (!optResult.has_value()) {
        throw invalid_argument("No parse for input \"" + input + "\"");
    }

    pair<T, string>& pairResult = optResult.value();
    string& rest = pairResult.second;
    if (!rest.empty()) {
        throw invalid_argument("No parse: \"" + rest + "\" remained.");
    }
    
    // Need "move" since the parsed object is inside the pair.
    // Prevents a call to the copy constructor.
    return move(pairResult.first);
}

template<typename T>
template<typename R>
Parser<R> Parser<T>::andThen(MovableFn<Parser<R>, T&&> pGenFn) const
{
    using namespace std;

    // Note that we are capturing a parameter, and its lifetime ends after
    // this function (NOT the lambda) returns, so do not capture by reference
    return Parser<R>{
        MovableFn<return_t, input_t&> {
            [=, pGenFn = move(pGenFn)](input_t& input) -> return_t {
                // Run first parser
                return_t optResult = parseFn_(input);
                // If first parser fails, fail entire thing
                if (!optResult.has_value()) {
                    return {};
                }

                pair<T, string>& pairResult = optResult.value();
                T& result = get<0>(pairResult);
                Parser<R> nextParser = pGenFn(move(result));
                // Run the next parser on the rest of the string
                return nextParser.parseFn_(pairResult.second);
            }
        }
    };
}

// template<typename T>
// template<typename R>
// Parser<std::pair<T,R>> Parser<T>::combine(const Parser<R>& nextParser) const
// {
//     using namespace std;

//     cout << "combine start" << endl;
//     auto p1 = andThen(MovableFn<Parser<pair<T,R>>, T&&>{
//         function{[=](T&& obj1)
//         {
//             cout << "outer lambda start" << endl;
//             auto p2 = nextParser.andThen(MovableFn<Parser<pair<T,R>>, R&&>{
//                 function{[obj1 = move(obj1)](R&& obj2)
//                 {
//                     cout << "inner lambda start" << endl;
//                     Parser<pair<T,R>> ppair{make_pair(move(obj1), move(obj2))};
//                     cout << "inner lambda end" << endl;
//                     return ppair;
//                 }}});
//             cout << "outer lambda end" << endl;
//             return p2;
//         }}});
//     cout << "combine end" << endl;
//     return p1;
// }
