#ifndef WIN_PERCENTAGES_HPP_INCLUDED
#define WIN_PERCENTAGES_HPP_INCLUDED 1

#include "player.hpp"
#include "hand.hpp"
#include "time.h"
#include <random>
#include <algorithm>
#include <chrono>

#define BOARD -1
#define UNKNOWN -1

class WinPercentages{
    public:
        WinPercentages() = delete;
        WinPercentages(int* info); // info = [numPlayers, b1-b5 (-1 for unknown), p0_c1 (-1 for unknown), p0_c2, p1_c1, p1_c2,..., p8_c1, p8_c2]
        ~WinPercentages();

        /***These fcns communicate with Android***/
        void addPlayer();
        void removePlayer(); // Need to remove players' cards
        // Puts card in user_selected vector
        // cardNum = 4*Suit + value (Each card will be mapped to correct num in Android)
        void addUserSelected(uchar cardNum, short pos); // pos=-1 for board
        void removeUserSelected(uchar cardNum, short pos);
        
        /*** Iterations ***/
        double** getWinAndTiePercentages();
        double** convertToPercentages(size_t* winCounts, size_t* tieCounts);
        double** avgEmptyHands(double* winPercentages, double* c);

        void dealRandomCard(short pos);
        void reset();

        void printDeck();
        void printWinAndTiePercentages(double** result);

        friend inline std::ostream& operator<<(std::ostream& out, const WinPercentages& h);

    // private:
        Player* players_;
        uchar num_players_;
        uchar board_size_;

        std::vector<Card> deck_;
        static const size_t NUM_ITERS = 200000;
};

inline std::ostream& operator<<(std::ostream& out, const WinPercentages& wp)
{   
    // Print players
    for (size_t i = 0; i < wp.num_players_; ++i){
        out << "Player: " << i << wp.players_[i] << std::endl;
    }

    // // Print bool array
    // size_t length = 52;
    // out << "User Selected:";
    // out << "[";
    // for (size_t i = 0; i < length; ++i){
    //     out << wp.bool_arr_[i];        
    //     if (i != length - 1){
    //         out << ", ";
    //     }
    // }
    // out << "]";

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