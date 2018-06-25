#ifndef WIN_PERCENTAGES_HPP_INCLUDED
#define WIN_PERCENTAGES_HPP_INCLUDED 1

#include "player.hpp"

class WinPercentages{
    public:
        WinPercentages() = delete;
        WinPercentages(int* info); // info = [numPlayers (-1 for single), b1-b5 (-1 for unknown), p0_c1 (-1 for unknown), p0_c2, p1_c1, p1_c2,..., p8_c1, p8_c2]
        ~WinPercentages();

        /***These fcns communicate with Android***/
        void addPlayer();
        void removePlayer(); // Need to remove players' cards
        // Puts card in user_selected vector
        // cardNum = 4*Suit + value (Each card will be mapped to correct num in Android)
        void addUserSelected(uchar cardNum, short pos); // pos=-1 for board
        void removeUserSelected(uchar cardNum, short pos);
        


        Card* getFromDeck(uchar val, Suit suit);
        void resetDeck(); // Resets for in between iterations
        // Puts card in used vector
        void addCard(uchar val, Suit suit, short pos);
        void removeCard(uchar val, Suit suit, short pos);

        friend inline std::ostream& operator<<(std::ostream& out, const WinPercentages& h);



    private:
        Player* players_;
        uchar num_players_;
        Card* board_;
        uchar board_size_;
        std::vector<Card*> deck_;
        std::vector<Card*> used_cards_;
        std::vector<Card*> user_selected_;

};

inline std::ostream& operator<<(std::ostream& out, const WinPercentages& h)
{   
    // Print players
    for (uchar i = 0; i < h.num_players_; ++i){
        out << "Player: " << i << h.players_[i] << std::endl;
    }

    // Print deck
    size_t length = h.deck_.size();
    out << "Deck:";
    out << "[";
    for (size_t i = 0; i < length; ++i){
        out << (*h.deck_[i]);        
        if (i != length - 1){
            out << ", ";
        }
    }
    out << "]" << std::endl;

    // Print used
    length = h.used_cards_.size();
    out << "Used:";
    out << "[";
    for (size_t i = 0; i < length; ++i){
        out << (*h.used_cards_[i]);        
        if (i != length - 1){
            out << ", ";
        }
    }
    out << "]" << std::endl;

    // Print user_selected
    length = h.user_selected_.size();
    out << "User Selected:";
    out << "[";
    for (size_t i = 0; i < length; ++i){
        out << (*h.user_selected_[i]);        
        if (i != length - 1){
            out << ", ";
        }
    }
    out << "]";

    return out;
}

// Remove and return card from vector
inline Card* removeFromVector(Card c, std::vector<Card*>& vec)
{
    Card* erased;
    for (auto iter = vec.begin(); iter != vec.end(); ++iter){
        if (c == **iter){
            erased = *iter;
            vec.erase(iter);
            return erased;
        }
    }
    return nullptr;
}

#endif