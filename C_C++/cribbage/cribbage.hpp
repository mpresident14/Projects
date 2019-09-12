#ifndef CRIBBAGE_HPP_INCLUDED
#define CRIBBAGE_HPP_INCLUDED 1

#include <time.h>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include "card.hpp"
#include "pegger.hpp"
#include "player.hpp"

class Cribbage {
 public:
  Cribbage();
  ~Cribbage();
  void play();

  // private:
  Card*** dealCards();  // Returns a hand for each player
  Card* flipTurnCard();
  void resetDeck();
  bool decideFirstCrib();
  Card*** userSelectDiscards(
      Card**& hand);  // Returns a chosenHand and a discarded hand
  Card* userSelectCardToPlay(std::vector<Card*>& hand, size_t& cardsLeft,
                             size_t& runningTotal);
  Card** makeCrib(
      Card**& discard1,
      Card**& discard2);  // Both discards are sorted, crib stays sorted
  void printBoard();
  void printScore();
  void theGo(std::vector<Card*>& userChoiceVec,
             std::vector<Card*>& compChoiceVec, bool userHasCrib);
  bool checkForWin();

  size_t runningTotal_;
  std::vector<Card*> board_;

  std::vector<Card*> deck_;
  std::vector<Card*> usedCards_;
  size_t player1Score_;
  size_t player2Score_;

  // For training my comp, printing statistics
  size_t goPoints1_;
  size_t goPoints2_;
  size_t handPoints1_;
  size_t handPoints2_;
  size_t cribPoints1_;
  size_t cribPoints2_;
};

#endif
