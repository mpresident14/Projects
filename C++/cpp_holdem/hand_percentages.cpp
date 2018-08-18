#include "hand_percentages.hpp"

using namespace std;

HandPercentages::HandPercentages(int* info, int flopTurnRiver)
    : flopTurnRiver_{flopTurnRiver}
{
    // TODO: convert to an array after I erase the necessary cards ???
    deck_.reserve(52);
    
    // Populate the deck
    for (uchar suit = 0; suit < 4; ++suit){
        for (uchar value = 1; value < 14; ++value){ // 1 to 25 to match Enum
            deck_.push_back(Card{value, (Suit) suit} );
        }
    }

    // Add cards
    for (uchar i = 0; i < 7; ++i){ // 7 = 2 hole cards + 5 board cards
        if (info[i] != -1){
            addUserSelected(uchar(info[i]));
        }
    }
}

void HandPercentages::addUserSelected(uchar cardNum)
{   
    Card c{cardNum}; // Make card
    player_.addOriginalHoleCard(c); // Give card to player
    removeFromVector(c, deck_); // Remove card from deck
}

unsigned HandPercentages::nChooseK(){
    uchar n = uchar(52 - player_.num_cards_);
    uchar k = uchar(2 + flopTurnRiver_ - player_.num_cards_);
    double answer = 1;
    while (k != 0){
        answer *= (n-k+1)*1.0/k;
        k -= 1;
    }
    return unsigned(round(answer));
}

// TODO: Add zero card and single card probabilities

double* HandPercentages::getHandPercentages()
{
    /* Takes too long to calculate with 6 or 7 unknown cards, just use predetermined percentages */
    // Zero cards
    if (player_.num_cards_ == 0){ // To river
        if (flopTurnRiver_ == 5) {
            return new double[10]{17.4119, 43.8225, 23.4955, 4.82987, 4.61938, 3.02549, 2.5961, 0.168067, 0.0278507, 0.00323206};
        }
        else if (flopTurnRiver_== 4){ // To turn
            return new double[10]{32.4822, 47.7969, 12.4411, 3.59633, 1.77626, 1.01084, 0.815305, 0.0720288, 0.00813419, 0.000923446};
        }
    }
    // One card
    else if (player_.num_cards_ == 1 && flopTurnRiver_ == 5){
        // Need to figure out what value the player has
        uchar val;
        uchar i = 0;
        while (player_.value_arr_[i] == 0){
            ++i;
        }
        val = i + 2;
        switch (val){
            case 2: return new double[10] {17.9695, 44.4441, 23.5769, 4.84802, 3.34071, 3.03557, 2.5961, 0.168067, 0.0199895, 0.00102169};
            case 3: return new double[10] {17.6459, 44.0978, 23.5328, 4.83819, 4.06451, 3.03007, 2.5961, 0.168067, 0.0254866, 0.00102169};
            case 4: return new double[10] {17.3223, 43.7515, 23.4888, 4.82836, 4.78832, 3.02457, 2.5961, 0.168067, 0.0309837, 0.00102169};
            case 5: return new double[10] {16.9987, 43.4052, 23.4447, 4.81852, 5.51213, 3.01907, 2.5961, 0.168067, 0.0364808, 0.00102169};
            case 6: return new double[10] {17.1282, 43.4629, 23.4447, 4.81852, 5.32498, 3.01932, 2.5961, 0.168067, 0.036231,  0.00102169};
            case 7: return new double[10] {17.1282, 43.4629, 23.4447, 4.81852, 5.32498, 3.01932, 2.5961, 0.168067, 0.036231,  0.00102169};
            case 8: return new double[10] {17.1282, 43.4629, 23.4447, 4.81852, 5.32498, 3.01932, 2.5961, 0.168067, 0.036231,  0.00102169};
            case 9: return new double[10] {17.1282, 43.4629, 23.4447, 4.81852, 5.32498, 3.01932, 2.5961, 0.168067, 0.036231,  0.00102169};
            case 10: return new double[10] {16.9987, 43.4052, 23.4447, 4.81852, 5.51213, 3.01907, 2.5961, 0.168067, 0.0307338, 0.00676866};
            case 11: return new double[10] {17.3223, 43.7515, 23.4888, 4.82836, 4.78832, 3.02457, 2.5961, 0.168067, 0.0252367, 0.00676866}; // Jack
            case 12: return new double[10] {17.6459, 44.0978, 23.5328, 4.83819, 4.06451, 3.03007, 2.5961, 0.168067, 0.0197396, 0.00676866}; // Queen
            case 13: return new double[10] {17.9695, 44.4441, 23.5769, 4.84802, 3.34071, 3.03557, 2.5961, 0.168067, 0.0142425, 0.00676866}; // King
            case 14: return new double[10] {17.9695, 44.4441, 23.5769, 4.84802, 3.34071, 3.03557, 2.5961, 0.168067, 0.0142425, 0.00676866}; // Ace
        }
    }
    // Otherwise, run the algorithm
    uchar comboSize = 2 + uchar(flopTurnRiver_) - player_.num_cards_;
    size_t* counts = player_.getHandCounts(deck_, comboSize);

    // cout << "Hand Counts:";
    // cout << "[";
    // for (size_t i = 0; i < 10; ++i){
    //     cout << (counts[i]);        
    //     if (i != 9){
    //         cout << ", ";
    //     }
    // }
    // cout << "]" << endl;

    size_t num_combos = nChooseK();
    double* percentages = new double[10];
    for (uchar i = 0; i < 10; ++i){
        percentages[i] = double(counts[i])/num_combos * 100;
    }
    delete[] counts;
    return percentages;
}

int main()
{
    // int* arr = new int[5]{1,2,3,4,5};
    // int* cpy = new int[5];
    // memcpy(cpy, arr, 20);
    // for (size_t i = 0; i < 5; ++i){
    //     cout << cpy[i] << endl;
    // }
    int* info = new int[7]{-1,-1,-1,-1,-1,4,9}; // 2, 14};
    
    /* TIMING */
    auto start = chrono::steady_clock::now();
    HandPercentages h{info, HandPercentages::RIVER};
    double* handPerc = h.getHandPercentages();
    auto stop = chrono::steady_clock::now();
    double elapsedTime = chrono::duration_cast<chrono::nanoseconds>(stop - start).count()/1000000000.0;
    cout << elapsedTime << " seconds" << endl;

    /* PRINT */
    cout << "Hand Percentages:";
    cout << "[";
    for (size_t i = 0; i < 10; ++i){
        cout << (handPerc[i]);        
        if (i != 9){
            cout << ", ";
        }
    }
    cout << "]" << endl;

    delete[] info;
    delete[] handPerc;
    
    return 0;   
}