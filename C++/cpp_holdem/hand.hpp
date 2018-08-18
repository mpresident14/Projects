#ifndef HAND_HPP
#define HAND_HPP 1

enum HandType{
    HIGH_CARD,
    PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
    ROYAL_FLUSH
};

class Hand {
  public:
    Hand() = default;
    Hand(HandType handType, unsigned long rank, unsigned long otherCards);
    int compareTo(const Hand& other) const;

  private:
    HandType handType_;
    unsigned long rank_;
    unsigned long otherCards_;
};

#endif
