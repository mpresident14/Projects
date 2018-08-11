#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED 1

#include "card.hpp"
#include <chrono>
#include <vector>

#define NUMVALUES 13
#define NUMSUITS 4



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

class Player{

    public:
        Player();
        // Player(Card* card1, Card* card2, uchar pos);
        /* 
         * Use default destructor because we will want 
         * to reuse the cards in the main class
         */
        ~Player() = default;
        void addSingleCard(Card* card);
        void addCards(Card* cards, uchar length); /// Add cards to hand
        void removeSingleCard(Card* card); 
        void removeCards(Card* cards, uchar length); /// remove card from hand
        HandType isCounterHand() const;
        HandType isStraight() const;
        HandType isFlush() const;
        HandType bht() const;
        HandType bestHandType() const;
        size_t* getHandCounts(std::vector<Card>& deck, uchar comboSize);
        void getHandCountsHelper(std::vector<Card>& deck, Card* combo, uchar deckSize, uchar comboSize,
            uchar arrayIndex, uchar comboIndex, size_t* handCounts);

        /* Printing */
        void printArrays() const;
        friend inline std::ostream& operator<<(std::ostream& out, const Player& p);
        
        /* Each entry is encoded as a sequence of 7 bits
         * Bit 5-7: count (000 to 100 = 0 to 4)
         * Bit 4: spade (1000 = 8 = 2^3)
         * Bit 3: clubs (100 = 4 = 2^2)
         * Bit 2: diamond (10 = 2 = 2^1)
         * Bit 1: heat (1 = 1 = 2^0)
         */        
        uchar value_arr_[NUMVALUES]; // Array keeping track of count of each value (0=2, 1=3,..., 11=K, 12=A)
        uchar suit_arr_[NUMSUITS]; // Array keeping track of count of each suit (0=H, 1=D, 2=C, 3=S)
        uchar num_cards_;
};



inline std::ostream& operator<<(std::ostream& out, const HandType& hand)
{
    switch(hand){
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

inline char* dec_to_bin_6bit(uchar num){
    
    char* bin_str = new char[6];

    bin_str[5] = '\0'; // Terminate
    // Disable warning
    
    bin_str[0] = char((num / 16) + 48);    
 
    char i = 0;
    while (i < 4) {
 
        // Store remainder bit
        bin_str[4-i] = (num % 2) + 48; // Convert to char
        num = num / 2;
        i++;
    }
    return bin_str;
}

inline std::ostream& operator<<(std::ostream& out, const Player& p)
{
    out << "[";
    for (uchar i = 0; i < NUMVALUES; ++i){
        if (i != 0){
            out << ", ";
        }
        out << dec_to_bin_6bit(p.value_arr_[i]); 
    }
    out << "]";

    return out;
}

#endif