#include "player.hpp"

using namespace std;

Player::Player() : suit_counts_{0}, orig_suit_counts_{0}, num_cards_{0} {
  // Initialize arrays
  uchar i;
  for (i = 0; i < NUMVALUES; ++i) {
    value_arr_[i] = 0;
    orig_value_arr_[i] = 0;
  }

  for (size_t i = 0; i < 10; ++i) {
    handCounts_[i] = 0;
  }
}

void Player::addSingleCard(const Card& card) {
  value_arr_[card.value_] += card.encoding_;
  suit_counts_ += 1 << (card.suit_ << 2);
}

void Player::removeSingleCard(const Card& card) {
  value_arr_[card.value_] -= card.encoding_;
  suit_counts_ -= 1 << (card.suit_ << 2);
}

void Player::addOriginalHoleCard(const Card& card) {
  addOriginalBoardCard(card);
  ++num_cards_;
}

void Player::addOriginalBoardCard(const Card& card) {
  value_arr_[card.value_] += card.encoding_;
  suit_counts_ += 1 << (card.suit_ << 2);
  orig_value_arr_[card.value_] += card.encoding_;
  orig_suit_counts_ += 1 << (card.suit_ << 2);
}

HandType Player::isCounterHand() const {
  uchar threeCnt = 0;
  uchar pairCnt = 0;

  // Get # of three of a kinds and pairs
  for (uchar i = 0; i < NUMVALUES; ++i) {
    // The count is the first 3 bits of the encoding
    uchar cur_val = value_arr_[i] & 7;
    if (cur_val >= 2) {
      if (cur_val == 4) {
        return FOUR_OF_A_KIND;
      } else if (cur_val == 3) {
        ++threeCnt;
      } else {
        ++pairCnt;
      }
    }
  }

  // Determine hand based on counts
  if ((pairCnt > 0 && threeCnt == 1) || (threeCnt == 2)) {
    return FULL_HOUSE;
  } else if (threeCnt == 1) {
    return THREE_OF_A_KIND;
  } else if (pairCnt >= 2) {
    return TWO_PAIR;
  } else if (pairCnt == 1) {
    return PAIR;
  } else {
    return HIGH_CARD;
  }
}

HandType Player::isStraight() const {
  uchar consecutive = 0;

  // Catches wrap around A2345 straight
  if (value_arr_[NUMVALUES - 1] != 0) {
    ++consecutive;
  }

  // Search for 5 consecutive 1s
  for (uchar i = 0; i < NUMVALUES; ++i) {
    if (value_arr_[i] != 0) {
      ++consecutive;
      if (consecutive == 5) {
        return STRAIGHT;
      }
    } else {
      consecutive = 0;
      // If we reached the 10 slot, then no chance of straight.
      if (i >= 8) {
        return HIGH_CARD;
      }
    }
  }
  return HIGH_CARD;
}

HandType Player::isFlush() const {
  // If there is more than 5 of a suit, get that suit.
  uchar best_suit = 5u;
  for (uchar i = 0; i < NUMSUITS; ++i) {
    // Bit mask for the count of each suit
    if ((suit_counts_ & (15u << (i << 2))) >= (5u << (i << 2))) {
      best_suit = i;
      break;
    }
  }
  // We did not find a flush
  if (best_suit == 5) {
    return HIGH_CARD;
  }

  // Search for straight among cards with best_suit
  uchar consecutive = 0;
  uchar suit_bit = 1 << (best_suit + 3);  // Because 3 bits for the count

  // Search for 5 consecutive 1s
  for (int i = NUMVALUES - 1; i >= 0; --i) {
    if (value_arr_[i] & suit_bit) {
      ++consecutive;
      if (consecutive == 5) {
        return i == 8 ? ROYAL_FLUSH : STRAIGHT_FLUSH;
      }
    } else {
      consecutive = 0;
      // If we reached the 5 slot, then no chance of straight
      if (i <= 3) {
        return FLUSH;
      }
    }
  }

  // Catches wrap around A2345 straight
  if (value_arr_[NUMVALUES - 1] & suit_bit) {
    if (consecutive == 4) {
      return STRAIGHT_FLUSH;
    }
  }

  return FLUSH;
}

HandType Player::bestHandType() const {
  HandType flushes = isFlush();

  // Royal/Straight Flush?
  if (flushes >= STRAIGHT_FLUSH) {
    return flushes;
  }

  HandType counters = isCounterHand();

  // Four of a kind/Full house?
  if (counters >= FULL_HOUSE) {
    return counters;
  }
  // Flush?
  else if (flushes == FLUSH) {
    return FLUSH;
  }
  // Straight?
  else if (isStraight()) {
    return STRAIGHT;
  }
  // Everything else?
  else {
    return counters;
  }
}

void Player::getHandCounts(const vector<Card>& deck, uchar comboSize) {
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
}

void Player::getHandCountsHelper(const vector<Card>& deck, Card* combo,
                                 uchar deckSize, uchar comboSize,
                                 uchar deckIndex, uchar comboIndex) {
  // When combo is filled, figure out the hand.
  if (comboIndex == comboSize) {
    // Use value_arr_ and suit_arr_ to figure out what hand the player has
    ++handCounts_[bestHandType()];
  } else {
    for (uchar i = deckIndex; i <= deckSize - comboSize + comboIndex; ++i) {
      // Update value count based on the card being added and the one being
      // removed from the current combo.
      removeSingleCard(combo[comboIndex]);
      const Card& deck_card = deck[i];
      addSingleCard(deck_card);
      combo[comboIndex] = deck_card;
      getHandCountsHelper(deck, combo, deckSize, comboSize, i + 1,
                          comboIndex + 1);
    }
  }
}

Hand Player::getCounterHand() const {
  unsigned int threeRank = ABSENT;
  unsigned int pairRank1 = ABSENT;
  unsigned int pairRank2 = ABSENT;
  uchar otherCardCount = 0;
  unsigned int otherCards = 0;

  // Get # of three of a kinds and pairs
  for (int i = NUMVALUES - 1; i >= 0; --i) {
    // The count is the first 3 bits of the encoding
    uchar cur_val = value_arr_[i] & 7;
    if (cur_val >= 2) {
      if (cur_val == 4) {
        if (i == NUMVALUES - 1) {
          otherCards = NUMVALUES - 2;
        } else {
          otherCards = NUMVALUES - 1;
        }
        return Hand{FOUR_OF_A_KIND, (unsigned int)i, otherCards};
      }

      else if (cur_val == 3) {
        if (threeRank == ABSENT) {
          threeRank = i;
        } else if (pairRank1 == ABSENT) {
          pairRank1 = i;
        } else if (pairRank2 == ABSENT) {
          pairRank2 = i;
        }
      }

      else {
        if (pairRank1 == ABSENT) {
          pairRank1 = i;
        } else if (pairRank2 == ABSENT) {
          pairRank2 = i;
        }
      }
    }

    else if (cur_val != 0) {
      if (otherCardCount < 5) {
        // Shift larger value to msb and add smaller value to lsb
        otherCards <<= 4;
        otherCards += i;
        ++otherCardCount;
      }
    }
  }

  // Determine hand based on counts
  if (threeRank != ABSENT) {
    if (pairRank1 != ABSENT) {
      return Hand{FULL_HOUSE, (threeRank << 4) + pairRank1, 0};
    } else {
      // 4 cards in otherCards, only need 2
      return Hand{THREE_OF_A_KIND, threeRank, otherCards >> 8};
    }
  }

  else if (pairRank1 != ABSENT) {
    if (pairRank2 != ABSENT) {
      // 3 cards in otherCards, only need 1
      return Hand{TWO_PAIR, (pairRank1 << 4) + pairRank2, otherCards >> 8};
    } else {
      // 5 cards in otherCards, only need 3
      return Hand{PAIR, pairRank1, otherCards >> 8};
    }
  }

  return Hand{HIGH_CARD, /* unused rank = */ 1, otherCards};
}

Hand Player::getStraight() const {
  uchar consecutive = 0;

  // Search for 5 consecutive 1s
  for (int i = NUMVALUES - 1; i >= 0; --i) {
    if (value_arr_[i] != 0) {
      ++consecutive;
      if (consecutive == 5) {
        return Hand{STRAIGHT, uchar(i + 4), 0};
      }
    } else {
      consecutive = 0;
      // If we reached the 5 slot, then no chance of straight.
      if (i <= 3) {
        // No need to keep track of otherCards because getCounterHand already
        // does this
        return Hand{HIGH_CARD, 0, 0};
      }
    }
  }

  // Catches wrap around A2345 straight
  if (value_arr_[NUMVALUES - 1] != 0) {
    if (consecutive == 4) {
      return Hand{STRAIGHT, 3u, 0};
    }
  }

  return Hand{HIGH_CARD, 0, 0};
}

Hand Player::getFlush() const {
  // If there is more than 5 of a suit, get that suit.
  uchar best_suit = 5u;
  for (uchar i = 0; i < NUMSUITS; ++i) {
    // Bit mask for the count of each suit
    if ((suit_counts_ & (15u << (i << 2))) >= (5u << (i << 2))) {
      best_suit = i;
      break;
    }
  }
  // We did not find a flush
  if (best_suit == 5) {
    return Hand{HIGH_CARD, 0, 0};
  }

  // Search for straight among cards with best_suit
  uchar consecutive = 0;
  uchar suit_bit = 1 << (best_suit + 3);  // Because 3 bits for the count

  unsigned int flushRank = 0;

  // Search for 5 consecutive 1s
  for (int i = NUMVALUES - 1; i >= 0; --i) {
    if (value_arr_[i] & suit_bit) {
      ++consecutive;
      flushRank <<= 4;
      flushRank += i;
      if (consecutive == 5) {
        return Hand{STRAIGHT_FLUSH, uchar(i + 4), 0};
      }
    } else {
      consecutive = 0;
    }
  }

  // Catches wrap around A2345 straight
  if (value_arr_[NUMVALUES - 1] & suit_bit) {
    if (consecutive == 4) {
      return Hand{STRAIGHT_FLUSH, 3, 0};
    }
  }

  // If more than 5 cards in flush, only the top 5 count.
  uchar best_suit_count = suit_counts_ & (15u << (best_suit << 2));
  flushRank >>= (best_suit_count - 5u) << 2;
  return Hand{FLUSH, flushRank, 0};
}

Hand Player::getBestHand() const {
  Hand flushes = getFlush();

  // Royal/Straight Flush?
  if (flushes.handType_ >= STRAIGHT_FLUSH) {
    return flushes;
  }

  Hand counters = getCounterHand();

  // Four of a kind/Full house?
  if (counters.handType_ >= FULL_HOUSE) {
    return counters;
  }
  // Flush?
  if (flushes.handType_ == FLUSH) {
    return flushes;
  }
  // Straight?
  Hand straight = getStraight();
  if (straight.handType_ == STRAIGHT) {
    return straight;
  }
  // Everything else?
  else {
    return counters;
  }
}

void Player::printArrays() const {
  cout << "Value Array: [";
  for (uchar i = 0; i < NUMVALUES; ++i) {
    if (i != 0) {
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
