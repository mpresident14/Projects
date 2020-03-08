#include "regex.hpp"
#include "parser.hpp"

#include <prez/timeit.hpp>

using namespace std;
using namespace parsers;
using RgxPtr = unique_ptr<Regex>;

RgxPtr doParse(const char* rgx) {
  /* Regex            := Concat | Alt | InitRgxWithStar
   * Concat           := InitRgxWithStar Regex
   * Alt              := InitRgxWithStar '|' Regex
   * InitRgxWithStar  := Star | InitRgx
   * Star             := InitRgx '*'
   * InitRgx          := Dot | (Regex) | Character
   * Dot              := '.'
   * Character        := char
   */

  Parser<RgxPtr> regex = fail<RgxPtr>();

  Parser<RgxPtr> dot =
      thisChar('.').andThenMap([](char) -> RgxPtr { return make_unique<Dot>(); });

  unordered_set<char> specialChars = {'(', ')', '|', '*'};
  Parser<RgxPtr> character =
      anyChar.verify([&specialChars](char c) { return !specialChars.count(c); })
          .andThenMap([](char c) -> RgxPtr { return make_unique<Character>(c); });

  Parser<RgxPtr> group = thisChar('(').ignoreAndThen(regex).thenIgnore(thisChar(')'));

  Parser<RgxPtr> initRgx = dot.alt(group).alt(character);

  Parser<RgxPtr> star =
      initRgx.thenIgnore(thisChar('*')).andThenMap([](auto&& rgx) -> RgxPtr {
        return make_unique<Star>(move(rgx));
      });

  Parser<RgxPtr> initRgxWithStar = star.alt(initRgx);

  Parser<RgxPtr> concat =
      initRgxWithStar.combine(regex).andThenMap([](auto&& rgxPair) -> RgxPtr {
        return make_unique<Concat>(move(rgxPair.first), move(rgxPair.second));
      });

  Parser<RgxPtr> alternative =
      initRgxWithStar.thenIgnore(thisChar('|'))
          .combine(regex)
          .andThenMap([](auto&& rgxPair) -> RgxPtr {
            return make_unique<Alt>(move(rgxPair.first), move(rgxPair.second));
          });

  regex.set(concat.alt(alternative).alt(initRgxWithStar));

  return regex.parse(rgx);
}

// for i in {1..10}; do ./regex "a*b|cd|(ef)*(abc|d)" a | grep milliseconds >>
// <filename>.txt; done
int main(int argc, char** argv) {
  if (argc != 3) {
    cerr << "Enter a regex to parse and a string to match." << endl;
    return 1;
  }

  // cout << prez::timeit<std::chrono::milliseconds>(1000, doParse, argv[1])
  //      << " milliseconds" << endl;

  try {
    auto result = doParse(argv[1]);
    cout << result->toString() << endl;
    cout << (result->matches(argv[2]) ? "Match" : "No Match") << endl;
  } catch (invalid_argument& e) {
    cerr << e.what() << endl;
  }
}
