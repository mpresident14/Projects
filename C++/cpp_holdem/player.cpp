#include "player.hpp"

using namespace std;

Player::Player()
    : suit_counts_{0}, orig_suit_counts_{0}, num_cards_{0}, handCounts_{new size_t[10]}
{
    // Initialize arrays
    uchar i;
    for (i = 0; i < NUMVALUES; ++i){
        value_arr_[i] = 0;
        orig_value_arr_[i] = 0;
    }

    for (size_t i = 0; i < 10; ++i){
        handCounts_[i] = 0;
    }
}

void Player::addSingleCard(const Card& card)
{   
    value_arr_[card.value_] += card.encoding_;
    suit_counts_ += 1 << (card.suit_ << 2);
    // ++suit_arr_[card.suit_];
    // ++num_cards_; (Now updated in addUserSelected)  
}

void Player::removeSingleCard(const Card& card)
{  
    value_arr_[card.value_] -= card.encoding_;
    suit_counts_ -= 1 << (card.suit_ << 2);
    // --suit_arr_[card.suit_];
    // ++num_cards_; (Now updated in addUserSelected)  
}

void Player::addOriginalCard(const Card& card)
{   
    value_arr_[card.value_] += card.encoding_;
    suit_counts_ += 1 << (card.suit_ << 2);
    // ++suit_arr_[card->suit_];
    ++num_cards_;
}

HandType Player::isCounterHand() const
{
    uchar threeCnt = 0;
    uchar pairCnt = 0;

    // Get # of three of a kinds and pairs
    for (uchar i = 0; i < NUMVALUES; ++i){
        // The count is the first 3 bits of the encoding
        uchar cur_val = value_arr_[i] & 7;
        if (cur_val >= 2){ 
            if (cur_val == 4){
                return FOUR_OF_A_KIND;
            }
            else if (cur_val == 3){
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
            // If we reached the 10 slot, then no chance of straight.
            if (i >= 8){
                break;
            }
        }
    }
    return HIGH_CARD;
}

HandType Player::isFlush() const
{
    // If there is more than 5 of a suit, get that suit.
    uchar best_suit = 5u;
    for (uchar i = 0; i < NUMSUITS; ++i){
        // Bit mask for the count of each suit
        if ((suit_counts_ & (15u << (i << 2))) >= (5u << (i << 2))){
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
    uchar suit_bit = 1 << (best_suit + 3); // Because 3 bits for the count
    // Save straight flush if we find it
    bool is_sf = false;
    // Catches wrap around A2345 straight
    if (value_arr_[NUMVALUES-1] & suit_bit){
        ++consecutive;
    }

    // Search for 5 consecutive 1s
    for (uchar i = 0; i < NUMVALUES; ++i){
        if (value_arr_[i] & suit_bit){
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


size_t* Player::getHandCounts(const vector<Card>& deck, uchar comboSize)
{
    Card* combo = new Card[comboSize];
    // We add these default cards so that we can avoid a check for a null card in 
    // removeSingleCard() and subtract normally.
    for (size_t i = 0; i < comboSize; ++i) {
        addSingleCard(combo[i]);
    }
    
    getHandCountsHelper(deck, combo, uchar(deck.size()), comboSize, 0, 0);
    // Restore value_arr_ and suit_arr_ to their initial states
    for (size_t i = 0; i < comboSize; ++i) {
        removeSingleCard(combo[i]);
    }

    delete[] combo;
    return handCounts_;
}

void Player::getHandCountsHelper(const vector<Card>& deck, Card* combo, uchar deckSize, 
        uchar comboSize, uchar deckIndex, uchar comboIndex)
{
    // When combo is filled, figure out the hand.
    if (comboIndex == comboSize){  
        // Use value_arr_ and suit_arr_ to figure out what hand the player has
        ++handCounts_[bestHandType()];
    }
    else{
        for (uchar i = deckIndex; i <= deckSize - comboSize + comboIndex; ++i){
            // Update value count based on the card being added and the one being removed from the
            // current combo.
            removeSingleCard(combo[comboIndex]);
            const Card& deck_card = deck[i];
            addSingleCard(deck_card);
            combo[comboIndex] = deck_card;
            getHandCountsHelper(deck, combo, deckSize, comboSize, i+1, comboIndex+1);
        }
    }
}

Hand Player::getCounterHand() const
{
    uchar threeRank = 0;
    uchar pairRank1 = 0;
    uchar pairRank2 = 0;

    // Get # of three of a kinds and pairs
    for (int i = NUMVALUES - 1; i >= 0; --i){
        // The count is the first 3 bits of the encoding
        uchar cur_val = value_arr_[i] & 7;
        if (cur_val >= 2){ 
            if (cur_val == 4){
                return FOUR_OF_A_KIND;
            }
            else if (cur_val == 3){
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

void Player::printArrays() const
{
    cout << "Value Array: [";
    for (uchar i = 0; i < NUMVALUES; ++i){
        if (i != 0){
            cout << ", ";
        }
        cout << dec_to_bin_nbit(value_arr_[i], 7); 
    }
    cout << "]" << endl;

    // cout << "Suit Array: [";
    // for (uchar i = 0; i < NUMSUITS; ++i){
    //     if (i != 0){
    //         cout << ", ";
    //     }
    //     cout << int(suit_arr_[i]); 
    // }
    // cout << "]" << endl;
}
