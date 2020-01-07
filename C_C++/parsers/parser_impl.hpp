template<typename T>
const Parser<T> Parser<T>::fail = Parser<T>{
        [](input_t& input) -> result_t<T> {
            return {};
        }
    };

// Only need to specify default template arg (Void) once (in header file)
template<typename T>
template<typename Fn, typename Void> 
Parser<T>::Parser(Fn&& f)
    : parseFn_{ std::make_shared<FnContainer<Fn>>(std::forward<Fn>(f)) } 
{}

template<typename T>
T Parser<T>::parse(const std::string& input) const
{
    using namespace std;

    result_t<T> optResult = (*parseFn_)(input);
    if (!optResult.has_value()) {
        throw invalid_argument("No parse for input \"" + input + "\"");
    }

    pair<T, string_view>& pairResult = optResult.value();
    string_view& rest = get<1>(pairResult);
    if (!rest.empty()) {
        throw invalid_argument("No parse: \"" + string(rest) + "\" remained.");
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
        // This must be mutable to call pGenFn's non-const () operator
        [parseFn = isThisRValue() ? move(parseFn_) : parseFn_, pGenFn = forward<Fn>(pGenFn)]
        (input_t& input) mutable -> result_t<is_parser_pt<Par>> {
                // Run first parser
                result_t<T> optResult = (*parseFn)(input);
                // If first parser fails, fail entire thing
                if (!optResult.has_value()) {
                    return {};
                }

                pair<T, string_view>& pairResult = optResult.value();
                T& result = get<0>(pairResult);
                auto nextParser = pGenFn(move(result));
                // Run the next parser on the rest of the string
                return (*nextParser.parseFn_)(get<1>(pairResult));
        }
    };
}

template<typename T>
template<typename Fn, typename Par>
std::enable_if_t<is_parser_v<Par>, Par> Parser<T>::andThenRef(Fn&& pGenFn) const
{
    using namespace std;
    
    return {
        // This must be mutable to call pGenFn's non-const () operator
        [this, pGenFn = forward<Fn>(pGenFn)]
        (input_t& input) mutable -> result_t<is_parser_pt<Par>> {
                // Run first parser
                result_t<T> optResult = (*parseFn_)(input);
                // If first parser fails, fail entire thing
                if (!optResult.has_value()) {
                    return {};
                }

                pair<T, string_view>& pairResult = optResult.value();
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
        [parseFn = isThisRValue() ? move(parseFn_) : parseFn_, nextParser = move(nextParser)]
        (input_t& input) {
                // Run first parser
                result_t<T> optResult = (*parseFn)(input);
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
Parser<T> Parser<T>::altRef(const Parser<T>& nextParser) const
{
    using namespace std;

    return {
        [parseFn = isThisRValue() ? move(parseFn_) : parseFn_, &nextParser]
        (input_t& input) {
                // Run first parser
                result_t<T> optResult = (*parseFn)(input);
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

// TODO: expand this to use a tuple instead of a pair (for longer chains) (use varargs)

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
template<typename R>
Parser<std::pair<T,R>> Parser<T>::combineRef(const Parser<R>& nextParser) const
{
    using namespace std;

    return andThen(
        [&nextParser](T&& obj1) {
            return nextParser.andThenRef(
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
        [boolFn = forward<Fn>(boolFn)](T&& obj) mutable -> Parser<T> {
            if (!boolFn(forward<T>(obj))) {
                return Parser<T>::fail;
            }
            return parsers::createBasic(forward<T>(obj));
        }
    );
}

template<typename T>
template<typename U>
Parser<std::enable_if_t<std::is_same_v<U, char>, std::string>> Parser<T>::many() const
{
    using namespace std;

    return Parser<string>{
        [parseFn = isThisRValue() ? move(parseFn_) : parseFn_](input_t& input) {
            // Run parser until it fails and put each result in the list
            unsigned i = 0;
            while ((*parseFn)(input.substr(i)).has_value()) {
                ++i;
            }

            return parsers::createReturnObject(string(input.substr(0, i)), input.substr(i));
        }
    };
}

template<typename T>
template<typename U>
Parser<std::enable_if_t<!std::is_same_v<U, char>, std::vector<T>>> Parser<T>::many() const
{
    using namespace std;

    return Parser<vector<T>>{
        [parseFn = isThisRValue() ? move(parseFn_) : parseFn_](input_t& input) {
            // Run parser until it fails and put each result in the list
            vector<T> listResult;
            unsigned i = 0;
            result_t<T> optResult = (*parseFn)(input);
            while (optResult.has_value()) {
                pair<T, string_view>& pairResult = optResult.value();
                listResult.push_back(move(get<0>(pairResult)));
                i = input.size() - get<1>(pairResult).size();
                optResult = (*parseFn)(get<1>(pairResult));
            }

            return parsers::createReturnObject(move(listResult), input.substr(i));
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
            return move(nextParser);
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

template<typename T>
template<typename R>
Parser<R> Parser<T>::ignoreAndThenRef(const Parser<R>& nextParser) const
{
    using namespace std;

    return andThen(
        [&nextParser](T&&) {
            return nextParser;
        }
    );
}

template<typename T>
template<typename R>
Parser<T> Parser<T>::thenIgnoreRef(const Parser<R>& nextParser) const
{
    using namespace std;

    return andThen(
        [&nextParser](T&& obj1) {
            return nextParser.andThenRef(
                // In order to forward obj1, the lambda must be mutable
                [obj1 = forward<T>(obj1)](R&&) mutable {
                    return parsers::createBasic(forward<T>(obj1));
                });
        });
}
