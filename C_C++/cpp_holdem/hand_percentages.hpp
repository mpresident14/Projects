#ifndef HAND_PERCENTAGES_HPP_INCLUDED
#define HAND_PERCENTAGES_HPP_INCLUDED 1

#include <math.h>
#include "player.hpp"

class HandPercentages {
 public:
  /* info = [cardNums c1-c7] (length = 7 = 2 hole cards + 5 board cards)
   * flopTurnRiver: 3=flop, 4=turn, 5=river
   */
  HandPercentages(const int* info, int flopTurnRiver);
  ~HandPercentages() = default;

  // Puts card in user_selected vector
  // cardNum = 4*Suit + value (Each card will be mapped to correct num in
  // Android)
  void addUserSelected(uchar cardNum);
  unsigned nChooseK() const;  // n choosing k
  double* getHandPercentages();

  friend inline std::ostream& operator<<(std::ostream& out,
                                         const HandPercentages& hp);

  static const int FLOP = 3;
  static const int TURN = 4;
  static const int RIVER = 5;

 private:
  Player player_;
  std::vector<Card> deck_;
  int flopTurnRiver_;
};

inline std::ostream& operator<<(std::ostream& out, const HandPercentages& hp) {
  // Print players

  out << "Player: " << hp.player_ << std::endl;

  // Print deck
  size_t length = hp.deck_.size();
  out << "Deck:";
  out << "[";
  for (size_t i = 0; i < length; ++i) {
    out << (hp.deck_[i]);
    if (i != length - 1) {
      out << ", ";
    }
  }
  out << "]" << std::endl;

  return out;
}

// Remove and return card from vector
inline void removeFromVector(Card c, std::vector<Card>& vec) {
  for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
    if (c == *iter) {
      vec.erase(iter);
      return;
    }
  }
}

#endif