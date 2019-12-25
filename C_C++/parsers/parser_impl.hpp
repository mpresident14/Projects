#include <string>
#include <optional>
#include <stdexcept>
#include <utility>
#include <iostream>

// TODO: Fix this so that failFn is in initialization
template<typename T>
// template<typename Fn>
Parser<T>::Parser()
    // : parseFn_{ 
    //     std::make_shared<FnContainer<Fn>(
    //         [](input_t& input) -> result_t<T> {
    //             return {};
    //         }
    //     )
    // }
{
    auto failFn = 
        [](input_t& input) -> result_t<T> {
            return {};
        };
    
    parseFn_ = std::make_shared<FnContainer<decltype(failFn)>>(std::move(failFn));
}

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
        // Have to capture "this" by value in case we call andThen with a temporary Parser object (e.g.
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
                return (*nextParser.parseFn_)(get<1>(pairResult));
        }
    };
}

template<typename T>
Parser<T> Parser<T>::alt(Parser<T> nextParser) const
{
    using namespace std;

    return {
        [=, *this, nextParser = move(nextParser)](input_t& input) {
                // Run first parser
                result_t<T> optResult = (*parseFn_)(input);
                // If first parser succeeds, return the result
                if (optResult.has_value()) {
                    return optResult;
                }

                // Otherwise, try the next parser on the input
                return (*nextParser.parseFn_)(input);
        }
    };
}

template<typename T>
template<typename Fn, typename R>
Parser<std::decay_t<R>> Parser<T>::andThenMap(Fn&& mapFn) const
{
    using namespace std;

    return andThen(
        [mapFn = forward<Fn>(mapFn)](T&& obj) mutable {
            return parsers::createBasic(mapFn(forward<T>(obj)));
        }
    );
}

// TODO: expand this to use a tuple instead of a pair (for longer chains)
// Pass nextParser by value since we have to copy it into the lambda anyways.
template<typename T>
template<typename R>
Parser<std::pair<T,R>> Parser<T>::combine(Parser<R> nextParser) const
{
    using namespace std;

    return andThen(
        [nextParser = move(nextParser)](T&& obj1) {
            return nextParser.andThen(
                // In order to forward obj1, the lambda must be mutable
                [obj1 = forward<T>(obj1)](R&& obj2) mutable {
                    return parsers::createBasic(make_pair(forward<T>(obj1), forward<R>(obj2)));
                });
        });
}

template<typename T>
template<typename Fn, typename Void>
Parser<T> Parser<T>::verify(Fn&& boolFn) const
{
    using namespace std;

    return andThen(
        [=, *this, boolFn = forward<Fn>(boolFn)](T&& obj) mutable -> Parser<T> {
            if (!boolFn(forward<T>(obj))) {
                return {};
            }
            return parsers::createBasic(forward<T>(obj));
        }
    );
}

// template<typename T>
// Parser<std::list<T>> Parser<T>::cons(Parser<std::list<T>> nextParser) const
// {
//     using namespace std;
//     cout << "cons" << endl;

//     return combine(move(nextParser))
//         .andThenMap(
//             [](pair<T, list<T>>&& myPair) {
//                 list<T>& myList = get<1>(myPair);
//                 myList.push_front(get<0>(myPair));
//                 return move(myList);
//             }
//         );
// }

template<typename T>
template<typename U>
Parser<std::enable_if_t<std::is_same_v<U, char>, std::string>> Parser<T>::many() const
{
    using namespace std;

    return Parser<string>{
        [=, *this](input_t& input) {
            // Run parser until it fails and put each result in the list
            unsigned i = 0;
            // TODO: Really inefficient. Avoid copies of the string (boost::string_view ???)
            while ((*parseFn_)(input.substr(i)).has_value()) {
                ++i;
            }

            return parsers::createReturnObject(input.substr(0, i), input.substr(i));
        }
    };
}

// So many string copies !!!
template<typename T>
template<typename U>
Parser<std::enable_if_t<!std::is_same_v<U, char>, std::vector<T>>> Parser<T>::many() const
{
    using namespace std;

    return Parser<vector<T>>{
        [=, *this](input_t& input) {
            // Run parser until it fails and put each result in the list
            vector<T> listResult;
            string restString = input;
            result_t<T> optResult = (*parseFn_)(input);
            while (optResult.has_value()) {
                pair<T, string>& pairResult = optResult.value();
                listResult.push_back(move(get<0>(pairResult)));
                restString = get<1>(pairResult);
                optResult = (*parseFn_)(get<1>(pairResult));
            }

            return parsers::createReturnObject(move(listResult), restString);
        }
    };
}

template<typename T>
Parser<std::nullptr_t> Parser<T>::ignore() const
{
    return andThenMap(
        [](T&&) {
            return nullptr;
        }
    );
}

template<typename T>
template<typename R>
Parser<R> Parser<T>::ignoreAndThen(Parser<R> nextParser) const
{
    using namespace std;

    return andThen(
        [nextParser = move(nextParser)](T&&) {
            return nextParser; // TODO: move ???
        }
    );
}

template<typename T>
template<typename R>
Parser<T> Parser<T>::thenIgnore(Parser<R> nextParser) const
{
    using namespace std;

    return andThen(
        [nextParser = move(nextParser)](T&& obj1) {
            return nextParser.andThenMap(
                // In order to forward obj1, the lambda must be mutable
                [obj1 = forward<T>(obj1)](R&&) mutable {
                    return forward<T>(obj1);
                });
        });
}
