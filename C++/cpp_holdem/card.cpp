#include "card.hpp"

using namespace std;

// Default card just so we can subtract an uninitialized card in getHandPercentages
// without crashing or changing the value and suit arrays.
Card::Card()
    : Card(2, HEARTS)
{
    // Nothing to do
}    

Card::Card(uchar rank, Suit suit)
    : suit_{suit}, encoding_{uchar((1 << (suit+3)) + 1)}, isDealt_{false}
{
    if (rank == 1){
        value_ = 12;
    }
    else{
        value_ = rank - 2;
    }
}

Card::Card(uchar cardNum)
    : value_{uchar(cardNum % 13)}, suit_{Suit(cardNum / 13)},
    encoding_{uchar((1 << (suit_ + 3)) + 1)}, isDealt_{false}
{
    // Nothing to do
}

Card& Card::operator=(const Card& other)
{
    // Skipping self-id check for performance
    value_ = other.value_;
    suit_ = other.suit_;
    encoding_ = other.encoding_;
    return *this;
}

bool Card::operator==(const Card& otherCard) const
{
    return value_ == otherCard.value_ && suit_ == otherCard.suit_;
}