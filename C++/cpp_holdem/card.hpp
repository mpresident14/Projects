#ifndef CARD_HPP_INCLUDED
#define CARD_HPP_INCLUDED 1

#include <cstddef>
#include <iostream>
#include <string>

enum Suit{
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES,
};

typedef unsigned char uchar;

class Card{

    public:
        Card(); // default constructor
        Card(uchar rank, Suit suit); // parameterized constructor
        Card(uchar cardNum); // 0 = 2Hearts, 1 = 3Hearts, ... , 13 = 2Diamonds, ... , 26 = 2Clubs, ... , 50 = KSpades, 51 = ASpades
        Card(const Card& otherCard) = default; // default copy constructor
        // Card(Card&& otherCard);
        Card& operator=(const Card& otherCard);
        ~Card() = default;

        bool operator==(const Card& otherCard);
        friend inline std::ostream& operator<<(std::ostream& out, const Card& card);

    //private:
        uchar value_; // Card's index in Player value_arr
        Suit suit_;
        uchar encoding_; // 7 bits: 4 msbs for suits (S,C,D,H), and 3 lsbs for value count
        bool isDealt_;
};

inline std::ostream& operator<<(std::ostream& out, const Card& card)
{
    // Restore rank from Player value_arr index
    size_t rank = card.value_ == 12 ? 1 : card.value_ + 2;
    std::string s;
    switch(card.suit_){
        case 0:
            s = "HEARTS";
            break;
        case 1:
            s = "DIAMONDS";
            break;
        case 2:
            s = "CLUBS";
            break;
        default:
            s = "SPADES";
    }
    out << rank << " " << s;
    return out;
}

#endif 