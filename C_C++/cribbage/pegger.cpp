#include "pegger.hpp"

using namespace std;

// CHANGE CHOSEN CARDS AFTER addToBoard()
Pegger::Pegger(vector<Card*> chosenHand)
    : chosenHand_{chosenHand}, cardsLeft_{4}
{
    // Nothing to do
}

void Pegger::addToBoard(Card*& card, vector<Card*>& board, size_t& runningTotal)
{
    board.push_back(card);
    runningTotal += card->adder();
    --cardsLeft_;
    
    auto iter = chosenHand_.begin();
    while (*iter != card){
        ++iter;
    }
    chosenHand_.erase(iter);
}

Card* Pegger::optimalPegCard(vector<Card*>& board, size_t& runningTotal)
{
    Card* card = nullptr;
    size_t bestScore = 0;
    for (size_t i = 0; i < cardsLeft_; ++i){
        if (runningTotal + chosenHand_[i]->adder() <= 31){
            size_t score = scorePeg(chosenHand_[i], board, runningTotal, false);
            if (score > bestScore){
                bestScore = score;
                card = chosenHand_[i];
            }
        }
    }
    // Strategy of how to play cards:
    // Loop through greatest to least until one matches criteria,
    // drop one criterion every for-loop down
    bool foundOne = false;
    Card* toReturn = nullptr;

    if (bestScore == 0){
        // Start with a card below 5 if possible
        if (runningTotal == 0){
            for (int i = cardsLeft_-1; i >= 0; --i){
                if (startWithSub5(chosenHand_[i], runningTotal)){
                    return chosenHand_[i];
                }
            }
        }
        
        for (int i = cardsLeft_-1; i >= 0; --i){
            if (runningTotal + chosenHand_[i]->adder() <= 31 &&
                !makesTotal(chosenHand_[i], runningTotal, 5) &&
                !makesTotal(chosenHand_[i], runningTotal, 10) &&
                !makesTotal(chosenHand_[i], runningTotal, 21) &&
                !makesRunOpportunity(chosenHand_[i], board, runningTotal) &&
                !sub5StartNo10(chosenHand_[i], board, runningTotal) ){
                // If all criteria are met and I have a might have a pair royal chance, play this card
                if (pairRoyalChance(i, runningTotal) ){
                    return chosenHand_[i];
                }
                // Save the highest card we've found that met these criteria
                if (!foundOne){
                    toReturn = chosenHand_[i];
                    foundOne = true;
                }
            }
            // Return it if we found anything
            if (toReturn != nullptr){
                return toReturn;
            }
        }

        for (int i = cardsLeft_-1; i >= 0; --i){
            if (runningTotal + chosenHand_[i]->adder() <= 31 &&
                !makesTotal(chosenHand_[i], runningTotal, 5) &&
                !makesTotal(chosenHand_[i], runningTotal, 10) &&
                !makesTotal(chosenHand_[i], runningTotal, 21) &&
                !makesRunOpportunity(chosenHand_[i], board, runningTotal) &&
                !sub5StartNo10(chosenHand_[i], board, runningTotal) ){
                if (pairRoyalChance(i, runningTotal) ){
                    return chosenHand_[i];
                }
                if (!foundOne){
                    toReturn = chosenHand_[i];
                    foundOne = true;
                }
            }
            if (toReturn != nullptr){
                return toReturn;
            }
        }

        for (int i = cardsLeft_-1; i >= 0; --i){
            if (runningTotal + chosenHand_[i]->adder() <= 31 &&
                !makesTotal(chosenHand_[i], runningTotal, 5) &&
                !makesTotal(chosenHand_[i], runningTotal, 10) &&
                !makesTotal(chosenHand_[i], runningTotal, 21) &&
                !makesRunOpportunity(chosenHand_[i], board, runningTotal) &&
                !sub5StartNo10(chosenHand_[i], board, runningTotal) ){
                if (pairRoyalChance(i, runningTotal) ){
                    return chosenHand_[i];
                }
                if (!foundOne){
                    toReturn = chosenHand_[i];
                    foundOne = true;
                }
            }
            if (toReturn != nullptr){
                return toReturn;
            }
        }

        for (int i = cardsLeft_-1; i >= 0; --i){
            if (runningTotal + chosenHand_[i]->adder() <= 31 &&
                !makesTotal(chosenHand_[i],runningTotal, 5) &&
                !makesTotal(chosenHand_[i],runningTotal, 21) &&
                !makesRunOpportunity(chosenHand_[i], board, runningTotal) &&
                !sub5StartNo10(chosenHand_[i], board, runningTotal) ){
                if (pairRoyalChance(i, runningTotal) ){
                    return chosenHand_[i];
                }
                if (!foundOne){
                    toReturn = chosenHand_[i];
                    foundOne = true;
                }
            }
            if (toReturn != nullptr){
                return toReturn;
            }
        }

        for (int i = cardsLeft_-1; i >= 0; --i){
            if (runningTotal + chosenHand_[i]->adder() <= 31 &&
                !makesTotal(chosenHand_[i],runningTotal, 5) &&
                !makesRunOpportunity(chosenHand_[i], board, runningTotal) &&
                !sub5StartNo10(chosenHand_[i], board, runningTotal) ){
                if (pairRoyalChance(i, runningTotal) ){
                    return chosenHand_[i];
                }
                if (!foundOne){
                    toReturn = chosenHand_[i];
                    foundOne = true;
                }
            }
            if (toReturn != nullptr){
                return toReturn;
            }
        }

        for (int i = cardsLeft_-1; i >= 0; --i){
            if (runningTotal + chosenHand_[i]->adder() <= 31 &&
                !makesTotal(chosenHand_[i],runningTotal, 5) &&
                !makesRunOpportunity(chosenHand_[i], board, runningTotal) ){
                if (pairRoyalChance(i, runningTotal) ){
                    return chosenHand_[i];
                }
                if (!foundOne){
                    toReturn = chosenHand_[i];
                    foundOne = true;
                }
            }
            if (toReturn != nullptr){
                return toReturn;
            }
        }

        for (int i = cardsLeft_-1; i >= 0; --i){
            if (runningTotal + chosenHand_[i]->adder() <= 31 &&
                !makesTotal(chosenHand_[i],runningTotal, 5) ){
                if (pairRoyalChance(i, runningTotal) ){
                    return chosenHand_[i];
                }
                if (!foundOne){
                    toReturn = chosenHand_[i];
                    foundOne = true;
                }
            }
            if (toReturn != nullptr){
                return toReturn;
            }
        }

        for (int i = cardsLeft_-1; i >= 0; --i){
            if (runningTotal + chosenHand_[i]->adder() <= 31){
                if (pairRoyalChance(i, runningTotal) ){
                    return chosenHand_[i];
                }
                if (!foundOne){
                    toReturn = chosenHand_[i];
                    foundOne = true;
                }
            }
            if (toReturn != nullptr){
                return toReturn;
            }
        }
        
        // Otherwise can't play anything
        return nullptr;
    }

    return card;
}

bool Pegger::pairRoyalChance(size_t cardIndex, size_t& runningTotal) const
{
    // Check for a pair in the hand and make sure that pair royal fits under 31
    return cardIndex != 0 && *(chosenHand_[cardIndex]) == *(chosenHand_[cardIndex-1]) && runningTotal + 3*(chosenHand_[cardIndex])->adder() <= 31;
}

bool Pegger::startWithSub5(Card*& cardToPlay, size_t& runningTotal) const
{
    if (runningTotal != 0){ // Can't start with a card below 5 if not the first card
        return false;
    }
    return cardToPlay->adder() < 5;
}

bool Pegger::sub5StartNo10(Card*& cardToPlay, vector<Card*>& board, size_t& runningTotal) const
{
    return board.size() == 1 && runningTotal < 5 && cardToPlay->adder() == 10;
}

bool Pegger::makesRunOpportunity(Card*& cardToPlay, vector<Card*>& board, size_t& runningTotal) const
{
    if (board.size() == 0){ // If no cards on board, can't make a run opportunity
        return false;
    }
    size_t lastOnBoard = board[board.size()-1]->value();
    size_t cardValue = cardToPlay->value();
    if ( abs( long(cardValue - lastOnBoard)) == 1){
        if (cardValue > lastOnBoard){
            if (runningTotal + cardValue + 1 <= 31 || runningTotal + lastOnBoard - 1 <= 31){
                return true;
            }
            return false;
        }
        else{
            if (runningTotal + lastOnBoard + 1 <= 31 || runningTotal + cardValue - 1 <= 31){
                return true;
            }
            return false;
        }
    }
    return false;
}

bool Pegger::makesTotal(Card*& card, size_t& runningTotal, size_t total) const
{
    if (runningTotal + card->adder() == total){
        return true;
    }
    return false;
}

bool Pegger::hasCards() const
{
    return cardsLeft_ != 0;
}

bool Pegger::canPlayUnder31(size_t& runningTotal_) const
{
    for (size_t i = 0; i < cardsLeft_; ++i){
        if (runningTotal_ + chosenHand_[i]->adder() <= 31){
            return true;
        }
    }
    return false;
}



/**********************************
*-------------SCORING-------------*
**********************************/   

size_t Pegger::scorePeg(Card*& card, vector<Card*>& board, size_t& runningTotal, bool print) const
{
    return pegPair(card, board, print) +
           peg15(card, runningTotal, print) +
           peg31(card, runningTotal, print) +
           pegRun(card, board, print);
}

size_t Pegger::pegPair(Card*& card, vector<Card*>& board, bool print) const
{
    size_t numDuplicates = 1;

    // Iterate backwards to get most recent cards first
    auto iter = board.rbegin();
    while(iter != board.rend() && *(card) == **iter){
        ++numDuplicates;
        ++iter;
    }

    // Calculate score based on pair, 3 or 4 of a kind
    if (numDuplicates > 1){
        if (numDuplicates == 4){
            if (print){
                cout << "Double pair royal." << endl;
            }
            return 12;
        }
        else if (numDuplicates == 3){
            if (print){
                cout << "Pair royal." << endl;
            }
            return 6;
        }
        else{ // If numDuplicates == 2
            if (print){
                cout << "Pair." << endl;
            }
            return 2;
        }
    }
    return 0;
}

// NEED TO CHANGE VALUE OF ROYALS TO 10
size_t Pegger::peg15(Card*& card, size_t& runningTotal, bool print) const
{
    if (runningTotal + card->adder() == 15){
        if (print){
            cout << "Made 15." << endl;
        }
        return 2;
    }
    else{
        return 0;
    }
}

size_t Pegger::peg31(Card*& card, size_t& runningTotal, bool print) const
{
    if (runningTotal + card->adder() == 31){
        if (print){
            cout << "Made 31." << endl;
        }
        return 2;
    }
    else{
        return 0;
    }
}

size_t Pegger::pegRun(Card*& card, vector<Card*>& board, bool print) const
{
    // Set up variables
    bool used[13]{false};
    size_t initValue = card->value();
    used[initValue-1] = true;
    size_t total = initValue;
    size_t max = initValue;
    size_t min = initValue;
    size_t length = 1;
    size_t bestRun = 1;

    for (auto iter = board.rbegin(); iter != board.rend() && length < 8; ++iter){
        size_t value = (*iter)->value();
        // If there is a duplicate number, no chance of a run
        if (used[value-1]){
            break;
        }
        // Update variables
        total += value;
        ++length;
        if (value > max){
            max = value;
        }
        else if (value < min){
            min = value;
        }
        // Check if there is a valid run
        if ( (max + min) * double(length) / 2 == total &&
              max - min + 1 == length){
            bestRun = length;
        }
        // Mark the value as used
        used[value-1] = true;
    }
    
    if (bestRun < 3){
        return 0;
    }
    if (print){
        cout << "Run of " << bestRun << "." << endl;
    }
    return bestRun;
}

// int main()
// {
//     Card** phand = new Card*[4];
//     // Card** qhand = new Card*[4];

//     phand[0] = new Card(5, CLUBS);
//     phand[1] = new Card(4, HEARTS);
//     phand[2] = new Card(11, DIAMONDS);
//     phand[3] = new Card(1, SPADES);    
    
//     sort(phand, phand+4, [](Card* a, Card* b){return *a < *b;});
//     //sort(qhand, qhand+4, [](Card* a, Card* b){return *a < *b;});

//     Pegger p{phand};
//     //Pegger q{qhand};
//     for (size_t i = 0; i < 4; ++i){
//         Card* pptr = p.optimalPegCard();
//         cout << *pptr << endl;
//         p.addToBoard(pptr);
//         printCardArray(p.chosenHand_, p.cardsLeft_);
//         cout << "running total: " << p.runningTotal_ << "\n" << endl;
//     }

//     // Not gonna work
//     // for (size_t i = 0; i < 4; ++i){
//     //     cout << "Player P" << endl;
//     //     Card* pptr = p.optimalPegCard();
//     //     cout << *pptr << endl;
//     //     p.addToBoard(pptr);
//     //     printCardArray(p.chosenHand_, p.cardsLeft_);
//     //     cout << "running total: " << p.runningTotal_ << "\n" << endl;

//     //     cout << "Player Q" << endl;
//     //     Card* qptr = q.optimalPegCard();
//     //     cout << *qptr << endl;
//     //     q.addToBoard(qptr);
//     //     printCardArray(q.chosenHand_, q.cardsLeft_);
//     //     cout << "running total: " << q.runningTotal_ << "\n" << endl;
//     // }
    
//     //destroyCardArray(qhand, 4);
//     return 0;
// }