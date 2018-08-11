#include "card.hpp"

using namespace std;

Card::Card(uchar val, Suit suit)
: suit_{suit}, encoding_{uchar(16 + (1 << suit))}, player_pos_{-1}
{
    if (val == 1){
        value_ = 12;
    }
    else{
        value_ = val - 2;
    }
}

Card::Card(uchar cardNum)
    : value_{uchar(cardNum % 13)}, suit_{Suit(cardNum / 13)}, 
    encoding_{uchar(16 + (1 << suit_))}, player_pos_{-1}
{
    // Nothing to do
}

void Card::setPlayerPos(short pos)
{
    player_pos_ = pos;
}

bool Card::operator==(const Card& otherCard)
{
    return value_ == otherCard.value_ && suit_ == otherCard.suit_;
}