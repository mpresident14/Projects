#include "hand.hpp"

using namespace std;

Hand::Hand(HandType handType, unsigned long rank, unsigned long otherCards)
    : handType_{handType}, rank_{rank}, otherCards_{otherCards} {
  // Nothing to do
}

short Hand::compareTo(const Hand& other) const {
  if (handType_ != other.handType_) {
    return handType_ < other.handType_ ? -1 : 1;
  }

  if (rank_ != other.rank_) {
    return rank_ < other.rank_ ? -1 : 1;
  }

  if (otherCards_ != other.otherCards_) {
    return otherCards_ < other.otherCards_ ? -1 : 1;
  }

  return 0;
}