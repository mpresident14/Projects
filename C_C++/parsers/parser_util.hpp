#include "parser.hpp"

#include <cctype>

namespace parsers {
    using namespace std;

    template<typename U>
    result_t<decay_t<U>> createReturnObject(U&& obj, input_t input)
    {
        return make_optional(make_pair(forward<U>(obj), input));
    }

    template<typename U>
    Parser<decay_t<U>> createBasic(U&& obj)
    {
        // Lambda is mutable so we can forward obj
        return Parser<decay_t<U>>{
            [obj = forward<U>(obj)](input_t& input) mutable {
                return createReturnObject(forward<U>(obj), input);
            }
        };
    }

    const Parser<char> anyChar{
        [](input_t& input) -> result_t<char>
        {
            if (input.empty()) {
                return {};
            }

            return createReturnObject(input[0], input.substr(1));
        }
    };

    const Parser<unsigned> anyDigit{
        anyChar.verify(
            [](char c) { return (bool) isdigit(c); } 
        )
        .andThenMap(
            [](char c) { return unsigned(c - '0'); }
        )
    };

    // Not worrying about overflow
    const Parser<unsigned> anyUnsigned{
        anyDigit.many().andThenMap(
            [](vector<unsigned>&& nums) {
                unsigned result = 0;
                for (auto iter = nums.begin(); iter != nums.end(); ++iter) {
                    result *= 10;
                    result += *iter;
                }
                return result;
            }
        )
    };

    // Not worrying about overflow or conversion errors
    const Parser<int> anyInt{
        thisChar('-').ignoreAndThen(anyUnsigned).andThenMap(
            [](unsigned&& num) { return -num; }
        )
        .alt(anyUnsigned).andThenMap(
            [](unsigned&& num) { return int(num); }
        )
    };

    const Parser<string> whitespace{
        anyChar.verify(
            [](char c) { return (bool) isspace(c); }
        )
        .many()
    };

    Parser<char> thisChar(char c)
    {
        return anyChar.verify(
            [c](char k) { return k == c; }
        );
    }

    template<typename U>
    Parser<U> skipws(Parser<U> p)
    {
        return whitespace.ignoreAndThen(move(p));
    }
}
