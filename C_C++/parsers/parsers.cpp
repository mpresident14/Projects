#include "parsers.hpp"

namespace parsers {

    using namespace std;

    const Parser<char> getSingleChar{
        [](const string& input) -> optional<pair<char, string>>
        {
            if (input.empty()) {
                return {};
            }

            return make_optional(make_pair(input[0], input.substr(1)));
        }
    };

    const Parser<int> getSingleInt{
        [](const string& input) -> optional<pair<int, string>>
        {
            if (input.empty()) {
                return {};
            }

            return make_optional(make_pair((int) input[0], input.substr(1)));
        }
    };
}
