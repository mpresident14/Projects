#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED 1

#include "card.hpp"
#include "hand.hpp"
#include <chrono>
#include <vector>
#include <cstring>
#include <algorithm>

#define NUMVALUES 13
#define NUMSUITS 4
#define ABSENT 0xFF

class Player{

    public:
        Player();
        /* 
         * Use default destructor because we will want 
         * to reuse the cards in the main class
         */
        ~Player() = default;
        void addSingleCard(const Card& card);
        void removeSingleCard(const Card& card);
        void addOriginalHoleCard(const Card& card);
        void addOriginalBoardCard(const Card& card);

        /* Hand Type */
        HandType isCounterHand() const;
        HandType isStraight() const;
        HandType isFlush() const;
        HandType bestHandType() const;
        void getHandCounts(const std::vector<Card>& deck, uchar comboSize);
        void getHandCountsHelper(const std::vector<Card>& deck, Card* combo, uchar deckSize, uchar comboSize,
                uchar deckIndex, uchar comboIndex);

        /* Hand comparison */   
        Hand getCounterHand() const;
        Hand getStraight() const;
        Hand getFlush() const;
        Hand getBestHand() const;

        /* Printing */
        void printArrays() const;
        friend inline std::ostream& operator<<(std::ostream& out, const Player& p);
        
        /* Each entry is encoded as a sequence of 7 bits
         *
         * Bit 7: spade (1000000 = 64 = 2^(3+3) )
         * Bit 6: clubs (100000 = 32 = 2^(2+3) )
         * Bit 5: diamond (10000 = 16 = 2^(1+3) )
         * Bit 4: heart (1000 = 8 = 2^(0+3) )
         * Bit 1-3: count (000 to 100 = 0 to 4)
         */        
        uchar value_arr_[NUMVALUES];    // Array keeping track of count of each value (0=2, 1=3,..., 11=K, 12=A)
        unsigned int suit_counts_;      // 4 bits per suit: SSSS|CCCC|DDDD|HHHH
        uchar orig_value_arr_[NUMVALUES];
        unsigned int orig_suit_counts_;
        uchar num_cards_;
        size_t handCounts_[10];
};

inline char* dec_to_bin_nbit(uchar num, size_t n){
    
    char* bin_str = new char[n+1];

    bin_str[n] = '\0'; // Terminate
    
 
    size_t i = 0;
    while (i < n) {
 
        // Store remainder bit
        bin_str[n-1-i] = (num % 2) + 48; // +48 converts '0' and '1' to 0 and 1, respectively
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
        out << std::dec << int(i) << ": " << dec_to_bin_nbit(p.value_arr_[i], 7); 
    }
    out << "]";

    return out;
}

// Remove and return card from vector
inline void removeFromVector(Card c, std::vector<Card>& vec)
{
    for (auto iter = vec.begin(); iter != vec.end(); ++iter){
        if (c == *iter){
            vec.erase(iter);
            return;
        }
    }
}

#endif