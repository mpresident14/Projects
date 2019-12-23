#include <string>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <utility>

template<typename T>
template<typename U>
Parser<U> Parser<T>::createBasic(U&& obj)
{
    return Parser{
        [obj = std::forward<T>(obj)](input_t& input) {
            return std::make_optional(std::make_pair(obj, input));
        }
    };
}

// Only need to specify default template arg (Void) once (in header file)
template<typename T>
template<typename F, typename Void> 
Parser<T>::Parser(F&& f)
    : parseFn_{ std::make_shared<FnContainer<F>>(std::forward<F>(f)) } 
{}

template<typename T>
T Parser<T>::parse(input_t& input) const
{
    using namespace std;

    result_t optResult = (*parseFn_)(input);
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

// See https://stackoverflow.com/questions/3786360/confusing-template-error
// for "template is_parser_v" explanation.
template<typename T>
template<typename F>
std::enable_if_t<Parser<T>::template is_parser<std::invoke_result_t<F, T>>::value, std::invoke_result_t<F, T>>
Parser<T>::andThen(F&& pGenFn) const
{
    using namespace std;

    // Note that we are capturing a parameter, and its lifetime ends after
    // this function (NOT the lambda) returns, so do not capture by reference
    return
        [this, pGenFn = forward<F>(pGenFn)](input_t& input) -> result_t {
            // Run first parser
            result_t optResult = (*parseFn_)(input);
            // If first parser fails, fail entire thing
            if (!optResult.has_value()) {
                return {};
            }

            pair<T, string>& pairResult = optResult.value();
            T& result = get<0>(pairResult);
            auto nextParser = pGenFn(move(result));
            // Run the next parser on the rest of the string
            return (*nextParser.parseFn_)(pairResult.second);
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
