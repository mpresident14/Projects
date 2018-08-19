#include "hand_percentages.hpp"

using namespace std;

HandPercentages::HandPercentages(const int* info, int flopTurnRiver)
    : flopTurnRiver_{flopTurnRiver}
{
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

unsigned HandPercentages::nChooseK() const
{
    uchar n = uchar(52 - player_.num_cards_);
    uchar k = uchar(2 + flopTurnRiver_ - player_.num_cards_);
    double answer = 1;
    while (k != 0){
        answer *= (n-k+1)*1.0/k;
        k -= 1;
    }
    return unsigned(round(answer));
}

double* HandPercentages::getHandPercentages()
{
    /* Takes too long to calculate with 6 or 7 unknown cards, just use predetermined percentages */
    // Zero cards
    if (player_.num_cards_ == 0){ // To river
        if (flopTurnRiver_ == 5) {
            return new double[10]{0.174119, 0.438225, 0.234955, 0.0482987, 0.0461938, 0.0302549, 0.025961, 0.00168067, 0.000278507, 0.0000323206};
        }
        else if (flopTurnRiver_== 4){ // To turn
            return new double[10]{0.324822, 0.477969, 0.124411, 0.0359633, 0.0177626, 0.0101084, 0.00815305, 0.000720288, 0.0000813419, 0.00000923446};
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
            case 2: return new double[10] {0.179695, 0.444441, 0.235769, 0.0484802, 0.0334071, 0.0303557, 0.025961, 0.00168067, 0.000199895, 0.0000102169};
            case 3: return new double[10] {0.176459, 0.440978, 0.235328, 0.0483819, 0.0406451, 0.0303007, 0.025961, 0.00168067, 0.000254866, 0.0000102169};
            case 4: return new double[10] {0.173223, 0.437515, 0.234888, 0.0482836, 0.0478832, 0.0302457, 0.025961, 0.00168067, 0.000309837, 0.0000102169};
            case 5: return new double[10] {0.169987, 0.434052, 0.234447, 0.0481852, 0.0551213, 0.0301907, 0.025961, 0.00168067, 0.000364808, 0.0000102169};
            case 6: return new double[10] {0.171282, 0.434629, 0.234447, 0.0481852, 0.0532498, 0.0301932, 0.025961, 0.00168067, 0.00036231,  0.0000102169};
            case 7: return new double[10] {0.171282, 0.434629, 0.234447, 0.0481852, 0.0532498, 0.0301932, 0.025961, 0.00168067, 0.00036231,  0.0000102169};
            case 8: return new double[10] {0.171282, 0.434629, 0.234447, 0.0481852, 0.0532498, 0.0301932, 0.025961, 0.00168067, 0.00036231,  0.0000102169};
            case 9: return new double[10] {0.171282, 0.434629, 0.234447, 0.0481852, 0.0532498, 0.0301932, 0.025961, 0.00168067, 0.00036231,  0.0000102169};
            case 10: return new double[10] {0.169987, 0.434052, 0.234447, 0.0481852, 0.0551213, 0.0301907, 0.025961, 0.00168067, 0.000307338, 0.0000676866};
            case 11: return new double[10] {0.173223, 0.437515, 0.234888, 0.0482836, 0.0478832, 0.0302457, 0.025961, 0.00168067, 0.000252367, 0.0000676866}; // Jack
            case 12: return new double[10] {0.176459, 0.440978, 0.235328, 0.0483819, 0.0406451, 0.0303007, 0.025961, 0.00168067, 0.000197396, 0.0000676866}; // Queen
            case 13: return new double[10] {0.179695, 0.444441, 0.235769, 0.0484802, 0.0334071, 0.0303557, 0.025961, 0.00168067, 0.000142425, 0.0000676866}; // King
            case 14: return new double[10] {0.179695, 0.444441, 0.235769, 0.0484802, 0.0334071, 0.0303557, 0.025961, 0.00168067, 0.000142425, 0.0000676866}; // Ace
        }
    }
    // Otherwise, run the algorithm
    uchar comboSize = 2 + uchar(flopTurnRiver_) - player_.num_cards_;
    player_.getHandCounts(deck_, comboSize);
    size_t counts[10];
    memcpy(counts, player_.handCounts_, sizeof(size_t) * 10);

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
        percentages[i] = double(counts[i])/num_combos;
    }

    return percentages;
}
