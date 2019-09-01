#ifndef CRIBBAGE_HPP_INCLUDED
#define CRIBBAGE_HPP_INCLUDED 1

#include <time.h>
#include <windows.h>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>
#include "card.hpp"
#include "pegger.hpp"
#include "player.hpp"

class Cribbage {
 public:
  Cribbage();
  ~Cribbage();
  void play();

 private:
  Card*** dealCards();  // Returns a hand for each player
  Card* Cribbage::flipTurnCard();
  void resetDeck();
  bool decideFirstCrib();
  Card*** userSelectDiscards(
      Card**& hand);  // Returns a chosenHand and a discarded hand
  Card* userSelectCardToPlay(Card**& hand);
  Card** makeCrib(
      Card**& discard1,
      Card**& discard2);  // Both discards are sorted, crib stays sorted
  void printBoard();
  void printScore();

  size_t runningTotal_;
  std::vector<Card*> board_;

  std::vector<Card*> deck_;
  std::vector<Card*> usedCards_;
  size_t player1Score_;
  size_t player2Score_;
};

#endif
