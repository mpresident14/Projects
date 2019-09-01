#ifndef PEGGER_HPP_INCLUDED
#define PEGGER_HPP_INCLUDED 1

#include <stdlib.h>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>
#include "card.hpp"

class Pegger {
 public:
  Pegger() = delete;
  Pegger(std::vector<Card*> chosenHand_);
  ~Pegger() = default;  // chosenHand_ destroyed in Cribbage class
  void addToBoard(Card*& card, std::vector<Card*>& board, size_t& runningTotal);
  size_t scorePeg(Card*& card, std::vector<Card*>& board, size_t& runningTotal,
                  bool print) const;
  Card* optimalPegCard(std::vector<Card*>& board, size_t& runningTotal);
  bool hasCards() const;
  bool canPlayUnder31(size_t& runningTotal_) const;

 private:
  size_t pegPair(Card*& card, std::vector<Card*>& board, bool print) const;
  size_t peg15(Card*& card, size_t& runningTotal, bool print) const;
  size_t peg31(Card*& card, size_t& runningTotal, bool print) const;
  size_t pegRun(Card*& card, std::vector<Card*>& board, bool print) const;
  /*****Functions for optimal pegging*************/
  bool makesTotal(Card*& card, size_t& runningTotal, size_t total)
      const;  // returns true if adding card results in specified running total
  bool makesRunOpportunity(Card*& cardToPlay, std::vector<Card*>& board,
                           size_t& runningTotal) const;
  bool sub5StartNo10(Card*& cardToPlay, std::vector<Card*>& board,
                     size_t& runningTotal) const;
  bool startWithSub5(Card*& cardToPlay, size_t& runningTotal) const;
  bool pairRoyalChance(size_t cardIndex, size_t& runningTotal) const;

  std::vector<Card*> chosenHand_;  // This will be sorted
  size_t cardsLeft_;

  friend class Cribbage;
};

#endif