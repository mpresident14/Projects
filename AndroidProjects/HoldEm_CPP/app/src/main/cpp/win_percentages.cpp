#include "win_percentages.hpp"

using namespace std;

// TODO: WE CAN PROBABLY GET RID OF BOARD ARRAY

WinPercentages::WinPercentages(int* info)
    : players_ {new Player[9]}, num_players_{0}, board_{new Card[5]}, board_size_{0}
{
    deck_.reserve(52);
    used_cards_.reserve(23);
    user_selected_.reserve(10);
    
    // Populate the deck
    for (uchar suit = 0; suit < 4; ++suit){
        for (uchar value = 1; value < 14; ++value){ // 1 to 25 to match Enum
            deck_.push_back(new Card{value, (Suit) suit} );
        }
    }

    uchar i;
    // Add Players
    for (i = 0; i < info[0]; ++i){
        addPlayer();
    }

    // Add board cards (slots 1 through 5)
    for (i = 1; i < 6; ++i){
        if (info[i] != -1){
            addUserSelected(uchar(info[i]), -1);
        }
    }

    // Add cards to each player
    uchar upper_bound = 6 + num_players_ * 2;
    for (i = 6; i < upper_bound; ++i){
        if (info[i] != -1){
            addUserSelected(uchar(info[i]), (i-6)/2); // Convert index to player position
        }
    }
}

WinPercentages::~WinPercentages()
{
    // Delete cards in each vector
    for (uchar i = 0; i < deck_.size(); ++i){
        delete deck_[i];
    }
    for (uchar i = 0; i < used_cards_.size(); ++i){
        delete used_cards_[i];
    }
    for (uchar i = 0; i < user_selected_.size(); ++i){
        delete user_selected_[i];
    }
}

void WinPercentages::addPlayer()
{
    players_[num_players_] = Player();
    ++num_players_;
}


void WinPercentages::addUserSelected(uchar cardNum, short pos)
{   
    // Add to all players (board)
    if (pos == -1){
        // TODO: DO THIS FOR EVERY PLAYER
        Card c{cardNum}; // Make card
        players_[0].addSingleCard(&c); // Give card to player
        Card* added = removeFromVector(c, deck_); // Remove card from deck
        user_selected_.push_back(added); // Add to user selected list
    }
    else{
        Card c{cardNum}; // Make card
        players_[pos].addSingleCard(&c); // Give card to player
        Card* added = removeFromVector(c, deck_); // Remove card from deck
        user_selected_.push_back(added); // Add to user selected list
    }
}

// void WinPercentages::removeUserSelected(uchar cardNum, short pos)
// {
//     // Remove from all players (board)
//     if (pos == -1){
//         // TODO: DO THIS FOR EVERY PLAYER
//         Card c{cardNum}; // Make card
//         players_[0].removeSingleCard(&c); // Give card to player
//         Card* added = removeFromVector(c, user_selected_); // Remove card from user selected list
//         deck_.push_back(added); // Add to deck
//     }
// }


void WinPercentages::resetDeck()
{
    for (size_t i = 0; i < used_cards_.size(); ++i){
        // Move cards from usedCards back into the deck
        deck_.push_back(used_cards_[i]);
    }
    // Delete ptrs from usedCards
    used_cards_.clear();
}

int main()
{
    // /**************
    //  * One player *
    //  **************/
    // int* info = new int[24]{1, -1,-1,-1,-1,-1, 2,14, -1,-1, -1,-1, -1,-1, -1,-1, -1,-1, -1,-1, -1,-1, -1,-1,};
    
    // auto start = chrono::steady_clock::now();
    // WinPercentages h{info};
    // // h.addPlayer();
    // /* Add user-selected cards (for 1 player, board or hand doesn't matter) */
    // // h.addUserSelected(2, -1);
    // // h.addUserSelected(14, -1);
    // // h.removeUserSelected(14, -1);
    // // h.addUserSelected(7, -1);
    // // h.addUserSelected(3, -1);
    // // h.addUserSelected(8, -1);
    // // h.addUserSelected(4, -1);
    // // cout << h << endl;

    // /* TIMING */
    // double iterations = 1;
    // size_t* handCounts = nullptr;
     
    // // for (size_t i = 0; i < iterations; ++i){
    // //     handCounts = h.getHandPercentages();
    // // } 
    // handCounts = h.getHandPercentages();
    // auto stop = chrono::steady_clock::now();
    // double elapsedTime = chrono::duration_cast<chrono::nanoseconds>(stop - start).count()/1000000000.0;
    // cout << elapsedTime/iterations << " seconds" << endl;

    
    // cout << "Hand Counts:";
    // cout << "[";
    // for (size_t i = 0; i < 10; ++i){
    //     cout << (handCounts[i]);        
    //     if (i != 9){
    //         cout << ", ";
    //     }
    // }
    // cout << "]" << endl;
    
    // // h.getHandPercentages()

    // /********************
    //  * Multiple players *
    //  ********************/
    // // Player p0;
    // // Player p1;
    // // Player p2;
    

    // // WinPercentages g;
    // // Add player
    // /* Add user-selected cards */
    // // h.addUserSelect(card, 0)
    // // h.addUserSelect(card, 1)
    // // h.addUserSelect(card, 2)
    // // h.addUserSelect(card, 2)
    // // h.addUserSelect(card, BOARD) (define BOARD = -2)
    // // h.addUserSelect(card, BOARD)
    // // h.addUserSelect(card, BOARD)

    // // h.getWinPercentages()

    // return 0;
    
    
}