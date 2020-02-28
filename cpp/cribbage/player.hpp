#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED 1

#include <time.h>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <vector>
#include "card.hpp"

class Player {
 public:
  Player(Card** hand);
  ~Player();                             // hand_ destroyed in Cribbage class
  Card*** optimalDiscard(bool hasCrib);  // Hand is always length of 6
  size_t scoreOf(Card**& hand, size_t length, size_t turnCardIndex, bool inCrib,
                 bool needsSort, bool print);
  static size_t insertCardIntoSortedArray(Card**& array,
                                          Card*& card)  // Length of array is 4
  {
    int index = 0;
    // Iterate backwards through the array
    for (int i = 4; i >= 0; --i) {
      // Shift element to the right if card is less
      if (i != 0 && *card < *(array[i - 1])) {
        array[i] = array[i - 1];
      }
      // Insert card into the appropriate position
      else {
        array[i] = card;
        index = i;
        break;
      }
    }
    return index;
  }

  // private:
  size_t findGroupsOf2Scoring15(size_t*& values,
                                size_t length);  // Requires sorted array
  size_t findGroupsOf3Scoring15(size_t*& values,
                                size_t length);    // Requires sorted array
  size_t scoreFifteens(Card**& hand, bool print);  // Requires sorted array
  size_t scoreRuns(Card**& hand, size_t length,
                   bool print);  // Requires sorted array
  size_t scorePairs(Card**& hand, size_t length,
                    bool print);  // Requires sorted array
  size_t scoreFlush(Card**& hand, size_t length, size_t turnCardIndex,
                    bool inCrib, bool print);
  bool scoreNobs(Card**& hand, Card*& turnUpCard, size_t length);
  void optimalDiscardHelper(Card**& chosen, Card**& crib, size_t length,
                            size_t chosenIndex, size_t getFromIndex,
                            double& bestScore, Card**& bestChoice,
                            bool hasCrib);
  size_t flipTurnUpCard(
      Card**& chosen);  // Gets total score from all turn cards
  double nobsPercentage(Card**& array, size_t length);
  void removeFromDeck(Card*& card);
  void removeInsertedCard(Card**& array, size_t insertedIndex);
  void putDiscardsInCrib(
      Card**& chosen,
      Card**& crib);  // Put two discards in crib after putting 4 into chosen
  size_t flipCardsForCrib(Card**& crib);
  void flipCardsForCribHelper(
      Card**& crib, size_t length, size_t cribIndex, size_t deckIndex,
      size_t& score);  // Fill crib with a turn-up card and 2 other cards

  Card** hand_;
  std::vector<Card*> remainingCards_;
  std::unordered_map<Suit, size_t, std::hash<int>> suitCounts_;

  friend class Cribbage;
};

#endif