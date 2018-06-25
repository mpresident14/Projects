#include "player.hpp"

using namespace std;

Player::Player()
    : num_cards_{0}
{
    // Initialize arrays
    for (uchar i = 0; i < NUMVALUES; ++i){
        value_arr_[i] = 0;
    }
    for (uchar i = 0; i < NUMSUITS; ++i){
        suit_arr_[i] = 0; 
    }
}



// Player::Player(Card* card1, Card* card2, uchar pos)
//     : num_cards_{0}, pos_{pos}
// {
//     // Initialize arrays
//     for (uchar i = 0; i < NUMVALUES; ++i){
//         value_arr_[i] = 0; 
//     }
//     for (uchar i = 0; i < NUMSUITS; ++i){
//         suit_arr_[i] = 0; 
//     }

//     // Update data members based on cards and their values
//     if (card1){
//         value_arr_[card1->getValue()] += 16 + (1 << card1->getSuit());
//         ++suit_arr_[card1->getSuit()];
//         ++num_cards_;
//     }
//     if (card2){
//         value_arr_[card2->getValue()] += 16 + (1 << card2->getSuit());
//         ++suit_arr_[card2->getSuit()];
//         ++num_cards_;
//     }
// }

void Player::addSingleCard(Card* card)
{   
    value_arr_[card->value_] += card->encoding_;
    ++suit_arr_[card->suit_];
    // ++num_cards_; (Now updated in addUserSelected)
    
}

void Player::addCards(Card* cards, uchar length)
{   
    for (uchar i = 0; i < length; ++i){
        addSingleCard(cards+i);
    }
}

void Player::removeSingleCard(Card* card)
{
    value_arr_[card->value_] -= card->encoding_;
    --suit_arr_[card->suit_];
    // --num_cards_; (Now updated in userSelected, so no need to remove)
}

void Player::removeCards(Card* cards, uchar length)
{   
    for (uchar i = 0; i < length; ++i){
        removeSingleCard(cards+i);
    }
}

HandType Player::isCounterHand() const
{
    uchar threeCnt = 0;
    uchar pairCnt = 0;

    // Get # of three of a kinds and pairs
    for (uchar i = 0; i < NUMVALUES; ++i){
        if (value_arr_[i] > 0x20){ // count >= 2
            if (value_arr_[i] >> 4 == 4){
                return FOUR_OF_A_KIND;
            }
            else if (value_arr_[i] >> 4 == 3){
                ++threeCnt;
            }
            else{
                ++pairCnt;
            }
        }
    }
    
    // Determine hand based on counts
    if ((pairCnt > 0 && threeCnt == 1) || (threeCnt == 2)){
        return FULL_HOUSE;
    }
    else if (threeCnt == 1){
        return THREE_OF_A_KIND;
    }
    else if (pairCnt >= 2){
        return TWO_PAIR;
    }
    else if (pairCnt == 1){
        return PAIR;
    }
    else{
        return HIGH_CARD;
    }
}

// ??? Might be faster to combine isStraight with isCounter
// so only looping through array once
HandType Player::isStraight() const
{

    uchar consecutive = 0;

    // Catches wrap around A2345 straight
    if (value_arr_[NUMVALUES-1] != 0){
        ++consecutive;
    }

    // Search for 5 consecutive 1s
    for (uchar i = 0; i < NUMVALUES; ++i){
        if (value_arr_[i] != 0){
            ++consecutive;
            if (consecutive == 5){
                return STRAIGHT;
            }
        }
        else{
            consecutive = 0;
            // If we reached the 10 slot, then no chance of straight
            if (i >= 8){
                break;
            }
        }
    }
    return HIGH_CARD;
}

HandType Player::isFlush() const
{
    // If there is more than 5 of a suit, get that suit
    uchar best_suit = 5;
    for (uchar i = 0; i < NUMSUITS; ++i){
        if (suit_arr_[i] >= 5){
            best_suit = i;
            break;
        }
    }
    // We did not find a flush
    if (best_suit == 5){
        return HIGH_CARD;
    }

    // Search for straight among cards with best_suit
    uchar consecutive = 0;
    uchar suit_bit = 1 << best_suit;
    // Save straight flush if we find it
    bool is_sf = false;
    // Catches wrap around A2345 straight
    if (value_arr_[NUMVALUES-1] != 0 && value_arr_[NUMVALUES-1] & suit_bit){
        ++consecutive;
    }

    // Search for 5 consecutive 1s
    for (uchar i = 0; i < NUMVALUES; ++i){
        if (value_arr_[i] != 0 && value_arr_[i] & suit_bit){
            ++consecutive;
            if (consecutive == 5){
                is_sf = true;
            }
        }
        else{
            consecutive = 0;
            // If we reached the 10 slot, then no chance of straight
            if (i >= 8){ // 8 == NUMVALUES - 4
                break;
            }
        }
    }
    if (is_sf){
        // If consecutive is >=5 after loop, then last card of straight flush was an ace
        if (consecutive >= 5){
            return ROYAL_FLUSH;
        }
        return STRAIGHT_FLUSH;
    }
    return FLUSH;
}


// THIS IS NOT WORKING CORRECTLY, flushes being counted as straight flushes
HandType Player::bht() const
{
    // FLUSH: If there is more than 5 of a suit, get that suit
    bool is_f = false;
    uchar best_suit = 5;
    for (uchar i = 0; i < NUMSUITS; ++i){
        if (suit_arr_[i] >= 5){
            best_suit = i;
            is_f = true;
            break;
        }
    }

    // FLUSH: If flush, set up rf and sf
    uchar s_consecutive = 0;
    uchar sf_consecutive = 0;
    // Save straight and straight flush if we find it
    bool is_sf = false;
    bool is_s = false;

    // COUNTER: Set up counter variables
    uchar fourCnt = 0;
    uchar threeCnt = 0;
    uchar pairCnt = 0;
    uchar cur_cnt = 0;  

    // Catches wrap around A2345 straight
    // STRAIGHT
    if (value_arr_[NUMVALUES-1] > 0 ){
        ++s_consecutive;
        // SF/RF
        if (is_f && (value_arr_[NUMVALUES-1] & (1 << best_suit))){
            ++sf_consecutive;
        }
    }

    // Iterate through value_arr
    for (uchar i = 0; i < NUMVALUES; ++i){
        // STRAIGHT
        if (value_arr_[i] > 0){
            ++s_consecutive;
            if (s_consecutive == 5){
                is_s = true;
            }
            // SF/RF
            if (is_f && (value_arr_[i] & (1 << best_suit))){
                ++sf_consecutive;
                if (sf_consecutive == 5){
                    is_sf = true;
                }
            }           
        }
        else{
            // STRAIGHT
            s_consecutive = 0;
            // SF_RF
            if (is_f){
                sf_consecutive = 0;
            }
        }
        // COUNT
        // Count is bits 5 and 6
        cur_cnt = value_arr_[i] >> 4;
        if (cur_cnt > 1){
            if (cur_cnt == 4){
                fourCnt = 1;
            }
            else if (cur_cnt == 3){
                ++threeCnt;
            }
            else{
                ++pairCnt;
            }
        }
    }
    // Check which hand it is
    if (is_sf){
        // If consecutive is >=5 after loop, then last card of straight flush was an ace
        if (sf_consecutive >= 5){
            return ROYAL_FLUSH;
        }
        return STRAIGHT_FLUSH;
    }
    else if(fourCnt){
        return FOUR_OF_A_KIND;
    }
    else if ((threeCnt == 1 && pairCnt > 0) || (threeCnt == 2)){
        return FULL_HOUSE;
    }
    else if(is_f){
        return FLUSH;
    }
    else if (is_s){
        return STRAIGHT;
    }
    else if (threeCnt == 1){
        return THREE_OF_A_KIND;
    }
    else if (pairCnt >= 2){
        return TWO_PAIR;
    }
    else if (pairCnt == 1){
        return PAIR;
    }
    else{
        return HIGH_CARD;
    }
}

// double elapsedTime = 0;
// auto start = chrono::steady_clock::now();     
// auto stop = chrono::steady_clock::now();

HandType Player::bestHandType() const
{
    HandType flushes = isFlush();
    
    // Royal/Straight Flush?
    if (flushes >= STRAIGHT_FLUSH){
        return flushes;
    }
    HandType counters = isCounterHand();
    
    // Four of a kind/Full house?
    if (counters >= FULL_HOUSE){
        return counters;
    }
    
    // Flush?
    else if (flushes == FLUSH){
        return FLUSH;
    }
    
    // Straight?    
    else if (isStraight()){
        
        return STRAIGHT;
    }
    
    // Everything else?
    else{
        return counters;
    }
}


size_t* Player::getHandCounts(vector<Card>& deck, uchar comboSize)
{
    Card* combo = new Card[comboSize];
    size_t* handCounts = new size_t[10];
    for (size_t i = 0; i < 10; ++i){
        handCounts[i] = 0;
    }
   
    getHandCountsHelper(deck, combo, uchar(deck.size()), comboSize, 0, 0, handCounts);
    delete[] combo;
    // cout << elapsedTime << " seconds" << endl;
    return handCounts;
}



void Player::getHandCountsHelper(vector<Card>& deck, Card* combo, uchar deckSize, 
uchar comboSize, uchar deckIndex, uchar comboIndex, size_t* handCounts)
{
    // When combo is filled, add Cards to Player, get the best hand type,
    // and update handCounts accordingly
    if (comboIndex == comboSize){
        
        addCards(combo, comboSize);
        
        // auto start = chrono::steady_clock::now();     
        ++handCounts[bestHandType()];
        // auto stop = chrono::steady_clock::now();

        removeCards(combo, comboSize);
        // elapsedTime += chrono::duration_cast<chrono::nanoseconds>(stop - start).count()/1000000000.0;

    }
    else{
        for (uchar i = deckIndex; i <= deckSize - comboSize + comboIndex; ++i){
            combo[comboIndex] = deck[i];
            getHandCountsHelper(deck, combo, deckSize, comboSize, i+1, comboIndex+1, handCounts);
        }
    }
}


void Player::printArrays() const
{
    cout << "Value Array: [";
    for (uchar i = 0; i < NUMVALUES; ++i){
        if (i != 0){
            cout << ", ";
        }
        cout << dec_to_bin_6bit(value_arr_[i]); 
    }
    cout << "]" << endl;

    cout << "Suit Array: [";
    for (uchar i = 0; i < NUMSUITS; ++i){
        if (i != 0){
            cout << ", ";
        }
        cout << suit_arr_[i]; 
    }
    cout << "]" << endl;
}


// int main()
// {
//     Card c1{10, HEARTS};
//     Card c2{11, HEARTS};
//     Card c3{4, DIAMONDS};
//     Card c4{3, HEARTS};
//     Card c5{3, DIAMONDS};
//     Card c6{2, SPADES};
//     Card c7{1, DIAMONDS};

//     Player p1;
//     p1.addSingleCard(c3);
//     p1.addSingleCard(c4);
//     p1.addSingleCard(c5);
//     p1.addSingleCard(c6);
//     p1.addSingleCard(c7);

//     cout << p1.bestHandType() << endl;
//     cout << p1.bht() << endl;
//     // p1.printArrays();
//     // cout << p1 << endl;


//     /* TIMING */
//     auto start = chrono::steady_clock::now(); 
//     for (size_t i = 0; i < 0xFFFFFF; ++i){
//         p1.bestHandType();
//     }
//     auto stop = chrono::steady_clock::now();
//     double elapsedTime = chrono::duration_cast<chrono::nanoseconds>(stop - start).count()/1000000000.0;
//     cout << elapsedTime << " seconds" << endl;

//     start = chrono::steady_clock::now(); 
//     for (size_t i = 0; i < 0xFFFFFF; ++i){
//         p1.bht();
//     }
//     stop = chrono::steady_clock::now();
//     elapsedTime = chrono::duration_cast<chrono::nanoseconds>(stop - start).count()/1000000000.0;
//     cout << elapsedTime << " seconds" << endl;

//     // int x = 43;
//     // cout << (x & 15) << endl;
    
//     return 0;
// }