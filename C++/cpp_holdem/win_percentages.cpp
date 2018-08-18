#include "win_percentages.hpp"

using namespace std;

WinPercentages::WinPercentages(int* info)
    : players_ {new Player[9]}, num_players_{0}, board_size_{0}
{    
    deck_.reserve(52);

    srand(unsigned(time(NULL)));

    uchar i;

    // Populate the deck
    // TODO: emplace_back ???
    for (i = 0; i < 52; ++i){
        deck_.push_back(Card{i});
    }

    // Add Players
    for (i = 0; i < info[0]; ++i){
        addPlayer();
    }

    // Add board cards (slots 1 through 5)
    for (i = 1; i < 6; ++i){
        if (info[i] != UNKNOWN){
            addUserSelected(uchar(info[i]), BOARD);
        }
    }

    // Add cards to each player
    uchar upper_bound = 6 + num_players_ * 2;
    for (i = 6; i < upper_bound; ++i){
        if (info[i] != UNKNOWN){
            addUserSelected(uchar(info[i]), (i-6)/2); // Convert index to player position
        }
    }
}

WinPercentages::~WinPercentages()
{
    delete[] players_;
}

void WinPercentages::addPlayer()
{
    players_[num_players_] = Player();
    ++num_players_;
}


void WinPercentages::addUserSelected(uchar cardNum, short pos)
{   
    // TODO: Use move constructor here ???
    Card c{cardNum}; // Make card
    // Add to all players (board)
    if (pos == BOARD){
        ++board_size_;
        for (uchar i = 0; i < num_players_; ++i){
            players_[i].addOriginalCard(c); // Give card to player            
        }
    }
    // Add to single player
    else{
        players_[pos].addOriginalCard(c); // Give card to each player
    }

    // Remove card from deck
    removeFromVector(c, deck_);
}

// size_t WinPercentages::dealRandomCard(short pos)
// {
//     // Faster to check if card has been used vs erasing from deck ???
//     // If so, add isDealt field to Card and change assignment operator to not worry about this field.

//     // Get random card
//     int n = rand() % deck_.size();
//     Card* card = deck_[n];
//     deck_.erase(deck_.begin() + n); // Erase from deck
    
//     // Add to all players (board)
//     if (pos == BOARD){
//         ++board_size_;
//         for (uchar i = 0; i < num_players_; ++i){
//             players_[i].addSingleCard(*card); // Give card to each player            
//         }    
        
//     }
//     // Add to single player
//     else{
//          players_[pos].addSingleCard(*card); // Give card to player
//     }  

//     return 13*card->suit_ + card->value_;
// }

size_t WinPercentages::dealRandomCard(short pos)
{
    // Get random card
    Card card;
    bool isInDeck = false;
    while (!isInDeck) {
        int n = rand() % deck_.size();
        Card& deckCard = deck_[n];
        if (!deckCard.isDealt_) {
            card = deckCard;
            deckCard.isDealt_ = true;
            isInDeck = true;
        }
    }

    // Add to all players (board)
    if (pos == BOARD){
        ++board_size_;
        for (uchar i = 0; i < num_players_; ++i){
            players_[i].addSingleCard(card); // Give card to each player            
        }    
        
    }
    // Add to single player
    else{
         players_[pos].addSingleCard(card); // Give card to player
    }  

    return 13*card.suit_ + card.value_;
}

void WinPercentages::reset()
{
    // Reset deck
    for (Card& c : deck_) {
        c.isDealt_ = false;
    }

    // Reset players
    size_t copySize = NUMVALUES * sizeof(uchar);
    for (uchar i = 0; i < num_players_; ++i) {
        Player p = players_[i];
        memcpy(p.value_arr_, p.orig_value_arr_, copySize);
        p.suit_counts_ = p.orig_suit_counts_;
    }
}

void WinPercentages::printDeck()
{
    size_t length = deck_.size();
    cout << "Deck:";
    cout << "[";
    for (size_t i = 0; i < length; ++i){
        cout << (deck_[i]);        
        if (i != length-1){
            cout << ", ";
        }
    }
    cout << "]" << endl;
}

int main()
{  
    int* info = new int[24]{1, 0,-1,-1,-1,-1, 51,-1, -1,-1, -1,-1, -1,-1, -1,-1, -1,-1, -1,-1, -1,-1, -1,-1,};

    WinPercentages wp{info};


    auto start = chrono::steady_clock::now();
    
    size_t arr[52] = {0};
    for (size_t iters = 0; iters < 200000; ++iters){
        for (size_t i = 0; i < 20; ++i){
            size_t n = wp.dealRandomCard(BOARD);
            ++arr[n];
        }
        wp.reset();
    }

    auto stop = chrono::steady_clock::now();
    double elapsedTime = chrono::duration_cast<chrono::nanoseconds>(stop - start).count()/1000000000.0;
    cout << "Elapsed time: " << elapsedTime << endl;
    
    cout << "Counts:";
    cout << "[";
    for (size_t i = 0; i < 52; ++i){
        cout << (arr[i]);        
        if (i != 51){
            cout << ", ";
        }
    }
    cout << "]" << endl;

    //cout << wp << endl;

    delete[] info;
    return 0;
    
    
}