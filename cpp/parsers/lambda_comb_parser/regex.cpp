#include "regex.hpp"
#include "parser.hpp"

using namespace std;
using namespace parsers;
using RgxPtr = unique_ptr<Regex>;

void doParse(const char* rgx, const char* input) {
  /* Regex            := Concat | Alt | InitRgxWithStar
   * Concat           := InitRgxWithStar Regex
   * Alt              := InitRgxWithStar '|' Regex
   * InitRgxWithStar  := Star | InitRgx
   * Star             := InitRgx '*'
   * InitRgx          := Dot | (Regex) | Character
   * Dot              := '.'
   * Character        := char
   */

  Parser<RgxPtr> regex = fail<RgxPtr>;

  Parser<RgxPtr> dot = thisChar('.').andThenMap(
      [](char) -> RgxPtr { return make_unique<Dot>(); });

  unordered_set<char> specialChars = {'(', ')', '|', '*'};
  Parser<RgxPtr> character =
      anyChar.verify([&specialChars](char c) { return !specialChars.count(c); })
          .andThenMap(
              [](char c) -> RgxPtr { return make_unique<Character>(c); });

  Parser<RgxPtr> group =
      thisChar('(').ignoreAndThenRef(regex).thenIgnore(thisChar(')'));

  Parser<RgxPtr> initRgx = dot.alt(group).alt(character);

  Parser<RgxPtr> star =
      initRgx.thenIgnore(thisChar('*')).andThenMap([](auto&& rgx) -> RgxPtr {
        return make_unique<Star>(move(rgx));
      });

  Parser<RgxPtr> initRgxWithStar = star.alt(initRgx);

  Parser<RgxPtr> concat = initRgxWithStar.combineRef(regex).andThenMap(
      [](auto&& rgxPair) -> RgxPtr {
        return make_unique<Concat>(move(rgxPair.first), move(rgxPair.second));
      });

  Parser<RgxPtr> alternative =
      initRgxWithStar.thenIgnore(thisChar('|'))
          .combineRef(regex)
          .andThenMap([](auto&& rgxPair) -> RgxPtr {
            return make_unique<Alt>(move(rgxPair.first), move(rgxPair.second));
          });

  regex = concat.alt(alternative).alt(initRgxWithStar);

  try {
    auto result = regex.parse(rgx);
    cout << result->toString() << endl;
    cout << (result->matches(input) ? "Match" : "No Match") << endl;
  } catch (invalid_argument& e) {
    cerr << e.what() << endl;
  }
}

int main(int argc, char** argv) {
  if (argc != 3) {
    cerr << "Enter a regex to parse and a string to match." << endl;
    return 1;
  }

  doParse(argv[1], argv[2]);
}
