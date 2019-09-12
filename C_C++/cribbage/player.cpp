#include "player.hpp"

using namespace std;

// CONSIDER CONSTRUCTING A PLAYER WITH A HAND EACH TIME. THEN PUT THE SETUP IN
// THE CONSTRUCTOR. CREATE A DIFFERENT CLASS TO DEAL WITH PEGGING (class
// PeggingPlayer ???) GIVE PLAYERS A NAME SO THAT WE CAN DIFFERENTIATE THEM WHEN
// PLAYING AGAINST EACH OTHER IF COMPUTER DECISION TAKES A LONG TIME, HAVE
// ANOTHER THREAD RUNNING SO THAT ALGORITHM CAN RUN WHILE PLAYER MAKES THEIR
// DECISION
Player::Player(Card** hand)
    : hand_{hand},
      suitCounts_{{HEARTS, 0}, {DIAMONDS, 0}, {SPADES, 0}, {CLUBS, 0}} {
  // Sort cards in hand so that they're in order
  sort(hand_, hand_ + 6, [](Card* a, Card* b) { return *a < *b; });

  remainingCards_.reserve(52);  // Set vector capacity
  // Put all cards in the deck, and then remove the cards in the hand (faster
  // than checking if each card is in the hand)
  for (size_t suit = 0; suit < 4; ++suit) {
    for (size_t value = 1; value < 14; ++value) {  // 1 to 13 to match Enum
      remainingCards_.push_back(new Card{value, (Suit)suit});
    }
  }

  // Remove cards in hand from the deck and put suits into hashmap
  for (size_t i = 0; i < 6; ++i) {
    removeFromDeck(hand_[i]);
    ++suitCounts_[hand_[i]->suit()];
  }
}

Player::~Player() {
  for (size_t i = 0; i < remainingCards_.size(); ++i) {
    delete remainingCards_[i];
  }
}

void Player::removeFromDeck(Card*& cardPtr) {
  for (auto iter = remainingCards_.begin(); iter != remainingCards_.end();
       ++iter) {
    if ((*cardPtr).equals(**iter)) {
      delete *iter;
      remainingCards_.erase(iter);
      break;
    }
  }
}

Card*** Player::optimalDiscard(bool hasCrib) {
  // Initialize variables for the helper function
  Card*** split = new Card**[2]();
  Card** chosen = new Card*[5]();  // 4 card hand in cribbage, leave 1 open for
                                   // the turn-up card
  Card** crib = new Card*[5]();

  double bestScore = 0;
  split[0] = new Card*[5]();  // split[0] is the best choice, leave 1 spot open
                              // for the turn-up card

  optimalDiscardHelper(chosen, crib, 6, 0, 0, bestScore, split[0],
                       hasCrib);  // 6 cards to start (2-player game)

  // Print Results
  // cout << "Optimal choice: ";
  // printCardArray(split[0], 4);
  // cout << "Net Score with optimal discard: " << bestScore << endl;

  // Grab the discarded cards
  split[1] = new Card*[2];

  putDiscardsInCrib(split[0], split[1]);

  delete[] crib;
  delete[] chosen;
  return split;
}

void Player::optimalDiscardHelper(Card**& chosen, Card**& crib, size_t length,
                                  size_t chosenIndex, size_t getFromIndex,
                                  double& bestScore, Card**& bestChoice,
                                  bool hasCrib) {
  // We have chosen 4 cards
  if (chosenIndex == 4) {  // 4 card hand in cribbage

    // Get scores from crib and hand
    putDiscardsInCrib(chosen, crib);
    double avgHandScore =
        nobsPercentage(chosen, 4) +
        double(flipTurnUpCard(chosen)) / 46;  // 46 possible turn-up cards
    double avgCribScore =
        nobsPercentage(crib, 2) + double(flipCardsForCrib(crib)) /
                                      15180;  // 46 choosing 3 possible combos
                                              // of turnup + 2 other crib cards

    // Alter the score depending on whether you have the crib
    double netScore = 0;
    if (hasCrib) {
      netScore = avgHandScore + avgCribScore;
    } else {
      netScore = avgHandScore - avgCribScore;
    }

    // Print stats about each option
    // printCardArray(chosen,4);
    // cout << "netScore: " << netScore << endl;

    if (netScore > bestScore) {
      // Update the best score and best cards
      bestScore = netScore;
      for (size_t i = 0; i < 4; ++i) {
        bestChoice[i] = chosen[i];
      }
    }
  } else {
    // Recursion in for-loop to get every combination of 4 cards from the 6
    // cards in hand
    for (size_t i = getFromIndex; i < 9 - length;
         ++i) {  // i < hand.length (6) - length + (hand.length - chosen.length
                 // + 1) (3)

      chosen[chosenIndex] = hand_[i];
      optimalDiscardHelper(chosen, crib, length - 1, chosenIndex + 1, i + 1,
                           bestScore, bestChoice, hasCrib);
    }
  }
}

size_t Player::flipTurnUpCard(Card**& chosen) {
  size_t score = 0;
  for (size_t i = 0; i < 46; ++i) {  // 46 cards remaining
    // Add turn-up card to hand of chosen cards, maintaining sorted order
    size_t insertedIndex =
        insertCardIntoSortedArray(chosen, remainingCards_[i]);
    score += scoreOf(chosen, 5, insertedIndex, false, false,
                     false);  // No need to sort, already sorted

    // Remove the card we inserted and shift elements back to the front
    removeInsertedCard(chosen, insertedIndex);

    // delete[] copy;
  }
  return score;
}

size_t Player::flipCardsForCrib(Card**& crib) {
  size_t score = 0;
  flipCardsForCribHelper(
      crib, 46, 2, 0, score);  // deck.length == 46, starting at index 2 of crib
                               // b/c first 2 spots are the discarded cards
  return score;
}

void Player::flipCardsForCribHelper(Card**& crib, size_t length,
                                    size_t cribIndex, size_t deckIndex,
                                    size_t& score) {
  // We have chosen 3 more cards to give a total of 5 cards
  if (cribIndex == 5) {
    Card** copy = new Card*[5];
    for (size_t i = 0; i < 5;
         ++i) {  // Make copy so crib is not changed by sort in scoreOf
      copy[i] = crib[i];
    }

    score += scoreOf(copy, 5, 0, true, true,
                     false);  // Need to sort these, 0 b/c no turnCardIndex
    delete[] copy;
  } else {
    // Recursion in for-loop to get every combination of 4 cards from the 6
    // cards in hand
    for (size_t i = deckIndex; i < 90 - length;
         ++i) {  // i < DECK.length (46) - length + (DECK.length - crib.length +
                 // 1) (44)____(crib.length is # cards to add == 3)
      crib[cribIndex] = remainingCards_[i];
      flipCardsForCribHelper(crib, length - 1, cribIndex + 1, i + 1, score);
    }
  }
}

void Player::removeInsertedCard(Card**& array,
                                size_t insertedIndex)  // Length of array is 4
{
  // Iterate backwards through the array
  for (int i = insertedIndex; i < 4; ++i) {
    // Shift elements right of insertion back to the left
    array[i] = array[i + 1];
  }
  array[4] = nullptr;
}

void Player::putDiscardsInCrib(Card**& chosen, Card**& crib) {
  size_t handIndex = 0;
  size_t choiceIndex = 0;
  size_t cribIndex = 0;

  while (cribIndex < 2) {  // Only 2 discards
    if (hand_[handIndex] != chosen[choiceIndex]) {
      crib[cribIndex] = hand_[handIndex];
      ++cribIndex;
    } else {
      ++choiceIndex;
    }
    ++handIndex;
  }
}

/************************************************************
 *--------------------SCORING FUNCTIONS-------------------- *
 ************************************************************/

double Player::nobsPercentage(Card**& array, size_t length) {
  double percentage = 0;
  for (size_t i = 0; i < length; ++i) {
    if (*(array[i]) == 11) {
      percentage += double(13 - suitCounts_[array[i]->suit()]) /
                    46;  // For each jack, (13 total of the suit - number of
                         // suits in the hand)
    }                    // all divided by 46 cards remaining in the deck
  }
  return percentage;
}

// Hand is sorted by Player constructor. Need to sort again if we are
// determining optimal discard for Crib
size_t Player::scoreOf(Card**& hand, size_t length, size_t turnCardIndex,
                       bool inCrib, bool needsSort, bool print) {
  if (needsSort) {
    sort(hand, hand + length, [](Card* a, Card* b) { return *a < *b; });
  }
  // Print Statements for Debugging
  // auto start = chrono::steady_clock::now();
  // size_t j = scoreFifteens(hand,length);
  // auto stop = chrono::steady_clock::now();
  // int64_t elapsedTime = chrono::duration_cast<chrono::nanoseconds>(stop -
  // start).count(); cout << elapsedTime << " seconds" << endl;

  // start = chrono::steady_clock::now();
  // size_t k = scoreRuns(hand, length);
  // stop = chrono::steady_clock::now();
  // elapsedTime = chrono::duration_cast<chrono::nanoseconds>(stop -
  // start).count(); cout << elapsedTime << " seconds" << endl;

  // start = chrono::steady_clock::now();
  // size_t m = scorePairs(hand, length);
  // stop = chrono::steady_clock::now();
  // elapsedTime = chrono::duration_cast<chrono::nanoseconds>(stop -
  // start).count(); cout << elapsedTime << " seconds" << endl;

  // start = chrono::steady_clock::now();
  // size_t n = scoreFlush(hand, length, inCrib);
  // stop = chrono::steady_clock::now();
  // elapsedTime = chrono::duration_cast<chrono::nanoseconds>(stop -
  // start).count(); cout << elapsedTime << " seconds" << endl; cout << endl;
  // return j + k + m + n;
  return scoreFifteens(hand, print) +  // Only works for an array of length 5
         scoreRuns(hand, length, print) + scorePairs(hand, length, print) +
         scoreFlush(hand, length, turnCardIndex, inCrib, print);
}

// For an array of length 5 ONLY!
size_t Player::scoreFifteens(Card**& hand, bool print) {
  // Need to convert card symbols to adding values
  // (example: King is represented by 13, but has adding value of 10)
  size_t* values = new size_t[5];
  for (size_t i = 0; i < 5; ++i) {
    values[i] = hand[i]->adder();
  }

  size_t count =
      findGroupsOf2Scoring15(values, 5) + findGroupsOf3Scoring15(values, 5);

  // Groups of 4
  if (values[0] + values[1] + values[2] + values[3] == 15) {
    ++count;
  }
  if (values[0] + values[1] + values[2] + values[4] == 15) {
    ++count;
  }
  if (values[0] + values[1] + values[3] + values[4] == 15) {
    ++count;
  }
  if (values[0] + values[2] + values[3] + values[4] == 15) {
    ++count;
  }
  if (values[1] + values[2] + values[3] + values[4] == 15) {
    ++count;
  }

  // Group of 5
  if (values[0] + values[1] + values[2] + values[3] + values[4] == 15) {
    ++count;
  }

  delete[] values;
  if (print && count != 0) {
    cout << count << " fifteen(s) for " << 2 * count << "." << endl;
  }
  return 2 * count;  // 2 points per combo of 15
}

size_t Player::findGroupsOf2Scoring15(size_t*& values, size_t length) {
  size_t count = 0;

  for (size_t left = 0; left < length - 1; ++left) {
    for (size_t right = length - 1; right > left; --right) {
      if (values[left] + values[right] < 15) {
        break;
      } else if (values[left] + values[right] == 15) {
        ++count;
      }
    }
  }
  return count;  // 2 points per combo of 15
}

size_t Player::findGroupsOf3Scoring15(size_t*& values, size_t length) {
  size_t count = 0;

  for (size_t left = 0; left < length - 2; ++left) {
    for (size_t right = length - 1; right > left; --right) {
      bool hitAtLeast15 = false;
      for (size_t i = left + 1; i < right; ++i) {
        if (values[left] + values[right] + values[i] > 15) {
          hitAtLeast15 = true;
          break;
        } else if (values[left] + values[right] + values[i] == 15) {
          ++count;
          hitAtLeast15 = true;
        }
      }
      if (!hitAtLeast15) {
        break;
      }
    }
  }
  return count;  // 2 points per combo of 15
}

size_t Player::scoreRuns(Card**& hand, size_t length, bool print) {
  size_t score = 1;
  size_t multiplier = 1;

  // Loop through and check whether the next element is one greater than the
  // current
  for (size_t i = 0; i < length - 1; ++i) {
    if (*(hand[i + 1]) == *(hand[i]) + 1) {
      ++score;
    }

    else if (*(hand[i + 1]) == *(hand[i])) {
      // If three of one value, triples the number of runs
      if (i > 0 && *(hand[i + 1]) == *(hand[i - 1])) {
        ++multiplier;
      }
      // A duplicate value doubles the number of runs
      // (Two duplicates quadruples)
      else {
        multiplier *= 2;
      }
    } else {
      // If we already have a run and it has ended
      // return its score now
      if (score >= 3) {
        if (print) {
          cout << multiplier << " run(s) of " << score << " for "
               << score * multiplier << "." << endl;
        }
        return score * multiplier;
      }
      score = 1;
      multiplier = 1;
    }
  }
  // If we didn't make a run
  if (score < 3) {
    return 0;
  }
  if (print) {
    cout << multiplier << " run(s) of " << score << " for "
         << score * multiplier << "." << endl;
  }
  return score * multiplier;
}

size_t Player::scorePairs(Card**& hand, size_t length, bool print) {
  size_t group1 = 1;
  size_t group2 = 1;
  bool nextGroup = false;

  for (size_t i = 0; i < length - 1; ++i) {
    if (*(hand[i]) == *(hand[i + 1])) {
      if (nextGroup) {
        ++group2;
      } else {
        ++group1;
      }
    } else {
      // If we've already made at least a pair, start the next group
      if (group1 > 1) {
        nextGroup = true;
      }
    }
  }

  // Convert number of cards in the set to points
  if (group1 > 2) {
    if (group1 == 4) {
      group1 = 12;
    } else {
      group1 = 6;
    }
  }
  if (group1 == 1) {
    group1 = 0;
  }

  if (group2 > 2) {
    if (group2 == 4) {
      group2 = 12;
    } else {
      group2 = 6;
    }
  }
  if (group2 == 1) {
    group2 = 0;
  }
  if (print && group1 + group2 != 0) {
    cout << (group1 + group2) / 2 << " pair(s) for " << group1 + group2 << "."
         << endl;
  }
  return group1 + group2;
}

size_t Player::scoreFlush(Card**& hand, size_t length, size_t turnCardIndex,
                          bool inCrib, bool print) {
  int counts[4]{0, 0, 0, 0};  // These are the counts of each suit, index
                              // corresponds to Enum Suit value

  if (inCrib) {  // In crib, doesn't matter which is turn card b/c all five need
                 // to be same suit
    for (size_t i = 0; i < length; ++i) {
      ++counts[hand[i]->suit()];
    }
  } else {  // Outside crib, we need to keep track of turn card b/c it is a
            // flush only if the 4 cards in your hand are the same suit
    for (size_t i = 0; i < length; ++i) {
      if (i != turnCardIndex) {
        ++counts[hand[i]->suit()];
      }
    }
  }

  for (size_t currentSuit = 0; currentSuit < 4; ++currentSuit) {
    if (counts[currentSuit] > 3) {
      if (counts[currentSuit] == 5) {
        if (print) {
          cout << "Flush for 5." << endl;
        }
        return 5;
      }
      if (!inCrib) {
        if (size_t(hand[turnCardIndex]->suit()) ==
            currentSuit) {  // If turnCard suit is same as 4 cards in your hand,
                            // flush is worth 5
          if (print) {
            cout << "Flush for 5." << endl;
          }
          return 5;
        }
        if (print) {
          cout << "Flush for 4." << endl;
        }
        return 4;
      }
    }
  }

  return 0;
}

bool Player::scoreNobs(Card**& hand, Card*& turnUpCard, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    if (hand[i]->value() == 11 && hand[i]->suit() == turnUpCard->suit()) {
      return true;
    }
  }
  return false;
}

// int main()
// {
//     Card** hand = new Card*[6];
//     hand[0] = new Card(2, CLUBS);
//     hand[1] = new Card(2, HEARTS);
//     hand[2] = new Card(3, SPADES);
//     hand[3] = new Card(4, HEARTS);
//     hand[4] = new Card(6, DIAMONDS);
//     hand[5] = new Card(10, DIAMONDS);

//     Player p{hand};
//     // Player::insertCardIntoSortedArray(hand, card);
//     printCardArray(hand, 5);
//     p.scoreOf(hand, 5, 4, false, true);

//     destroyCardArray(hand, 6);
// }

//     // auto start = chrono::steady_clock::now();
//     // Card** bestChoice = p.optimalDiscard(false);
//     // auto stop = chrono::steady_clock::now();
//     // double elapsedTime = chrono::duration_cast<chrono::nanoseconds>(stop -
//     start).count() / 1000000000.0;
//     // cout << elapsedTime << " seconds" << endl;
//     // delete[] bestChoice;

// //     /*** Execution time tests ***/
// //     // Card* c = new Card(1, DIAMONDS);
// //     // Card* d = new Card(1, CLUBS);
// //     // Card* e = new Card(6, SPADES);
// //     // Card* f = new Card(11, HEARTS);
// //     // Card* g = new Card(11, SPADES);
// //     // Card* h = new Card(5, DIAMONDS);
// //     // Card** cardPtrs = new Card*[6] {c,d,e,f,g,h};

// //     // Card j(1, DIAMONDS);
// //     // Card k(1, CLUBS);
// //     // Card l(6, SPADES);
// //     // Card m(11, HEARTS);
// //     // Card n(11, SPADES);
// //     // Card o(5, DIAMONDS);
// //     // Card* cards = new Card[6] {j,k,l,m,n,o};

// //     // size_t iterations = 1000000;
// //     // auto start = chrono::steady_clock::now();
// //     // for (size_t i = 0; i < iterations; ++i){
// //     //     Card** copy = new Card*[6];
// //     //     for (size_t x = 0; x < 6; ++x){
// //     //         copy[x] = cardPtrs[x];
// //     //     }
// //     //     sort(copy, copy+6, [](Card* a, Card* b) {return *a < *b;});
// //     //     delete[] copy;
// //     // }
// //     // auto stop = chrono::steady_clock::now();
// //     // cout << chrono::duration_cast<chrono::nanoseconds>(stop -
// start).count() / 1000000000.0
// //     //     << " seconds" << endl;

// //     // start = chrono::steady_clock::now();
// //     // for (size_t i = 0; i < iterations; ++i){
// //     //     Card* copy = new Card[6];
// //     //     for (size_t x = 0; x < 6; ++x){
// //     //         copy[x] = cards[x];
// //     //     }
// //     //     sort(copy, copy+6);
// //     // }
// //     // stop = chrono::steady_clock::now();
// //     // cout << chrono::duration_cast<chrono::nanoseconds>(stop -
// start).count() / 1000000000.0
// //     //     << " seconds" << endl;

// //     // destroyCardArray(cardPtrs,6);
// //     // delete[] cards;
//     return 0;
// }