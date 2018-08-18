#ifndef HAND_HPP
#define HAND_HPP 1

#include <iostream>

enum HandType {
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

    friend inline std::ostream& operator<<(std::ostream& out, const Hand& hand);

  // private:
    HandType handType_;
    unsigned long rank_;        // 20 bits: 4 bits per value, highest number at msb
    unsigned long otherCards_;  // 20 bits: 4 bits per value, highest number at msb
};

inline std::ostream& operator<<(std::ostream& out, const HandType& hand)
{
    switch (hand) {
        case HIGH_CARD:
            out << "HIGH_CARD";
            break;
        case PAIR:
            out << "PAIR";
            break;
        case TWO_PAIR:
            out << "TWO_PAIR";
            break;
        case THREE_OF_A_KIND:
            out << "THREE_OF_A_KIND";
            break;
        case STRAIGHT:
            out << "STRAIGHT";
            break;
        case FLUSH:
            out << "FLUSH";
            break;
        case FULL_HOUSE:
            out << "FULL_HOUSE";
            break;
        case FOUR_OF_A_KIND:
            out << "FOUR_OF_A_KIND";
            break;
        case STRAIGHT_FLUSH:
            out << "STRAIGHT_FLUSH";
            break;
        case ROYAL_FLUSH:
            out << "ROYAL_FLUSH";
            break;
    }
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const Hand& hand)
{
  out << "{" << hand.handType_ 
      << ", " << std::hex << hand.rank_
      << ", "  << std::hex << hand.otherCards_ << "}";
  return out;
}

#endif
