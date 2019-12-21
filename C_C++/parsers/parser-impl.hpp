#include <string>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <utility>


template<typename T>
Parser<T>::Parser(const std::function<std::optional<std::pair<T, std::string>>(const std::string&)>& parseFn)
    : parseFn_{parseFn}
{

}

template<typename T>
Parser<T>::Parser(std::function<std::optional<std::pair<T, std::string>>(const std::string&)>&& parseFn)
    : parseFn_{parseFn}
{

}

template<typename T>
Parser<T>::Parser(const T& obj)
    : parseFn_{
        [=](const std::string& input)
        {
            return std::make_optional(std::make_pair(obj, input));
        }
    }
{
    
}

template<typename T>
Parser<T>::Parser(T&& obj)
    : parseFn_{
        // Using &obj results in wrong answer
        [=, obj = move(obj)](const std::string& input)
        {
            return std::make_optional(std::make_pair(move(obj), input));
        }
    }
{
    
}

template<typename T>
T Parser<T>::parse(const std::string& input) const
{
    using namespace std;

    cout << "parse start" << endl;

    optional<pair<T, string>> optResult = parseFn_(input);
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
    cout << "parse end" << endl;
    return move(pairResult.first);
}

template<typename T>
template<typename R>
Parser<R> Parser<T>::andThen(std::function<Parser<R>(T&& obj)>&& pGenFn) const
{
    using namespace std;

    return Parser<R>{
        [=](const string& input) -> optional<pair<R, string>>
        {
            // Run first parser
            optional<pair<T, string>> optResult = parseFn_(input);
            // If first parser fails, fail entire thing
            if (!optResult.has_value()) {
                return {};
            }

            pair<T, string>& pairResult = optResult.value();
            Parser<R> nextParser = pGenFn(move(get<0>(pairResult)));
            // Run the next parser on the rest of the string
            return nextParser.parseFn_(pairResult.second);
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
        function{[=](T&& obj1)
        {
            cout << "outer lambda start" << endl;
            auto p2 = nextParser.andThen(
                function{[obj1 = move(obj1)](R&& obj2)
                {
                    cout << "inner lambda start" << endl;
                    Parser<pair<T,R>> ppair{make_pair(move(obj1), move(obj2))};
                    cout << "inner lambda end" << endl;
                    return ppair;
                }});
            cout << "outer lambda end" << endl;
            return p2;
        }});
    cout << "combine end" << endl;
    return p1;
}
