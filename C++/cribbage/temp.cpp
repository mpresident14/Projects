#include "cribbage.hpp"

using namespace std;

Cribbage::Cribbage()
    :runningTotal_{0}, player1Score_{0}, player2Score_{0}
{
    // Seed the random number generator
    srand(size_t(time(NULL)));

    usedCards_.reserve(12);
    board_.reserve(8);
    deck_.reserve(52);
    
    // Populate the deck
    for (size_t suit = 0; suit < 4; ++suit){
        for (size_t value = 1; value < 14; ++value){ // 1 to 13 to match Enum           
            deck_.push_back(new Card{value, (Suit) suit} );
        }
    }
}

Cribbage::~Cribbage()
{
    // FIGURE OUT REST AFTER

    for (size_t i = 0; i < deck_.size(); ++i){
        delete deck_[i];
    }
    for (size_t i = 0; i < usedCards_.size(); ++i){
        delete usedCards_[i];
    }
}

Card*** Cribbage::dealCards()
{
    Card*** hands = new Card**[2];
    cout << "check1: " << endl;
    for (size_t i = 0; i < 2; ++i){
        hands[i] = new Card*[6]; // Deal 6 cards to each player
        cout << "check2: " << endl;
        for (size_t j = 0; j < 6; ++j){
            // Get a random number corresponding to an index in the deck
            int n = rand() % deck_.size();
            cout << "Dealing: " << (*deck_[n]) << endl;
            // Move selected card into usedCards and the correct hand
            usedCards_.push_back(deck_[n]);
            hands[i][j] = deck_[n];

            // Remove the card from the deck
            auto iter = deck_.begin();
            advance(iter, n);
            deck_.erase(iter);          
        }
    }
    return hands;
}

Card* Cribbage::flipTurnCard()
{
    // Add card to usedCards
    int n = rand() % deck_.size();
    Card* card = deck_[n];
    usedCards_.push_back(card);

    // Remove the card from the deck
    auto iter = deck_.begin();
    advance(iter, n);
    deck_.erase(iter);

    return card;
}

void Cribbage::resetDeck()
{
    for (auto iter = usedCards_.begin(); iter != usedCards_.end(); ++iter){
        cout << "deck: " << deck_.size() << endl;
        cout << "used: " << usedCards_.size() << endl;
        cout << "Transferring: " << **iter << "\n" << endl;
        
        // Move cards from usedCards back into the deck
        deck_.push_back(*iter);
        // iter = usedCards_.erase(iter);
        // push back all, then clear
    }
    cout << "ran" << endl;
    usedCards_.clear();
    cout << "deck: " << deck_.size() << endl;
    cout << "used: " << usedCards_.size() << endl;
}

void Cribbage::printBoard()
{
    int size = board_.size();
    cout << "\n" << endl;
    cout << "--------------------";
    for (int i = 0; i < size-1; ++i){
        cout << "----------";
    }
    cout << endl;
    cout << "Running Total: " << runningTotal_ << endl;
    cout << "BOARD: [";
    for (int i = 0; i < size; ++i){
        cout << *(board_[i]);        
        if (i != size - 1){
            cout << ", ";
        }
    }
    cout << "]" << endl;
    cout << "--------------------";
    for (int i = 0; i < size-1; ++i){
        cout << "----------";
    }
    cout << endl;
}

void Cribbage::printScore()
{   
    cout << "\n" << "            SCORE" << endl;
    cout << "--------------------------------" << endl;
    cout << "    You: " << player1Score_ << "   |   Opponent: " << player2Score_ << endl;
}

bool Cribbage::checkForWin()
{
    if (player1Score_ >= 30 || player2Score_ >= 30){
        return true;
    }
    return false;
}

bool Cribbage::decideFirstCrib()
{
    if (rand() % 2 == 0){
        return true;
    }
    return false;
}

Card** Cribbage::makeCrib(Card**& discard1, Card**& discard2)
{
    Card** crib = new Card*[5]; // Leave 1 spot open for turn-up card
    size_t index1 = 0;
    size_t index2 = 0;
    for (size_t i = 0; i < 4; ++i){
        // If at end of discard1, insert from discard2
        if (index1 == 2){
            crib[i] = discard2[index2];
            ++index2;
        }
        else if (index2 == 2){
            crib[i] = discard1[index1];
            ++index1;
        }
        else if ( *(discard1[index1]) < *(discard2[index2]) ){
            crib[i] = discard1[index1];
            ++index1;
        }
        else{
            crib[i] = discard2[index2];
            ++index2;
        }
    }
    return crib;
}

void Cribbage::theGo(Card**& userChoice, Card**& compChoice, bool userHasCrib)
{
    cout << "****************************" << endl;
    cout << "* ---------THE GO--------- *" << endl;
    cout << "****************************" << endl;

    printBoard();
    Pegger pegger1{userChoice}; 
    Pegger pegger2{compChoice};
    bool userSaidGo = false;
    bool compSaidGo = false;
    this_thread::sleep_for(chrono::seconds(1));

    while (pegger1.hasCards() || pegger2.hasCards()){
        
        if (userHasCrib){
            // User has crib, so player2 (comp) starts The Go
            Card* compPlay = pegger2.optimalPegCard(board_, runningTotal_);         // Choose a card to play
            if (compPlay != nullptr){
                
                cout << "\n" << "Opponent plays " << *compPlay << "." << endl;
                size_t score = pegger2.scorePeg(compPlay, board_, runningTotal_, true); // Get the score from playing it
                player2Score_ += score;
                if (score != 0){
                    cout << "Opponent scores " << score << "." << endl;
                    if (checkForWin()){
                        break;
                    }
                }
                this_thread::sleep_for(chrono::seconds(1));

                pegger2.addToBoard(compPlay, board_, runningTotal_);                // Add it to the board
                if (runningTotal_ == 31){
                    runningTotal_ = 0;
                    board_.clear();
                    userSaidGo = false;
                }
                printBoard();
                this_thread::sleep_for(chrono::seconds(1));
            }
            else{
                if (userSaidGo){
                    ++player2Score_; // One for Go
                    cout << "\n" << "Opponent scores 1 for Go." << endl;
                    if (checkForWin()){
                        break;
                    }
                    // Reset running total and clear the board
                    runningTotal_ = 0;
                    board_.clear();
                    userSaidGo = false;
                    printBoard();
                }
                else{
                    cout << "\n" << "Opponent said \"Go.\" " << endl;
                    compSaidGo = true;
                    this_thread::sleep_for(chrono::seconds(1));
                }
            }
        }

        // User's play
        if (pegger1.canPlayUnder31(runningTotal_)){
            cout << "\n" << "YOUR CARDS: ";
            printCardArray(pegger1.chosenHand_, pegger1.cardsLeft_);                                        // Show user their cards

            Card* userPlay = userSelectCardToPlay(pegger1.chosenHand_, pegger1.cardsLeft_, runningTotal_);  // Select a card
            size_t score = pegger1.scorePeg(userPlay, board_, runningTotal_, true);                         // Get the score from playing it
            player1Score_ += score;
            if (score != 0){
                cout << "You score " << score << "." << endl;
                this_thread::sleep_for(chrono::seconds(1));
                if (checkForWin()){
                    break;
                }
            }
            pegger1.addToBoard(userPlay, board_, runningTotal_);  // Add it to the board
            if (runningTotal_ == 31){
                runningTotal_ = 0;
                board_.clear();
                compSaidGo = false;
            }
            printBoard();
            this_thread::sleep_for(chrono::seconds(1));
        }
        else{
            if (compSaidGo){
                ++player1Score_; // One for Go
                cout << "\n" << "You score 1 for Go." << endl;
                if (checkForWin()){
                    break;
                }
                // Reset running total and clear the board
                runningTotal_ = 0;
                board_.clear();
                compSaidGo = false;
                printBoard();
            }
            else{
                userSaidGo = true;
                cout << "\n" << "You said \"Go.\" " << endl;
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
        userHasCrib = true; // So that comp can play next
    }
    // Point for last card
    if (!checkForWin()){
        if (userSaidGo){
            cout << "Opponent scores 1 for Go." << endl;
            ++player2Score_;
            checkForWin();
        }
        else{
            if (runningTotal_ != 0){ // If == 0, this means we ended with a 31 and reset
                cout << "\n" << "Opponent said \"Go\" " << endl;
                cout << "You score 1." << "\n" << endl;
                ++player1Score_;
                checkForWin();
            }            
        }
        printScore();
        this_thread::sleep_for(chrono::seconds(1));
    }
    board_.clear(); // Clear the board for the next Go
}

/***********************************************
 *----------USER INTERFACE FUNCTIONS*----------*
 ***********************************************/
Card*** Cribbage::userSelectDiscards(Card**& hand)
{
    cout << "\n" << "Select cards to discard by index." << "\n" << endl;
    int firstIndex = -1;
    int secondIndex = -1;

    // Make sure the user inputs a number between 0 and 5
    cout << "First Card: ";
    cin >> firstIndex;
    while(!cin || firstIndex < 0 || firstIndex > 5){
        cin.clear(); // reset failbit
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please select two distinct numbers between 0 and 5." << "\n" << endl;
        cout << "First Card: ";
        cin >> firstIndex;
    }
    cout << "Second Card: ";
    cin >> secondIndex;
    while(!cin || secondIndex < 0 || secondIndex > 5 || firstIndex == secondIndex){
        cin.clear(); // reset failbit
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please select two distinct numbers between 0 and 5." << "\n" << endl;
        cout << "Second Card: ";
        cin >> secondIndex;
    }

    // Index 0 is chosen array, Index 1 is discarded array
    Card*** split = new Card**[2];
    split[0] = new Card*[5]; // Leave 1 spot open for turn-up card
    split[1] = new Card*[2];

    size_t choiceIndex = 0;
    size_t discardIndex = 0;

    for (int i = 0; i < 6; ++i){ // Hand will always have 6 cards
        if (i == firstIndex || i == secondIndex){
            split[1][discardIndex] = hand[i];
            ++discardIndex;
        }
        else{
            split[0][choiceIndex] = hand[i];
            ++choiceIndex;
        }
    }
    return split;
}

Card* Cribbage::userSelectCardToPlay(Card**& hand, size_t& cardsLeft, size_t& runningTotal)
{
    cout << "\n" << "Select card to play by index." << endl;
    int index = -1;

    // Make sure the user inputs a valid index and that his choice doesn't go above 31
    cin >> index;
    while(!cin || index < 0 || index >= int(cardsLeft) || runningTotal + hand[index]->adder() > 31){
        if (!cin || index < 0 || index >= int(cardsLeft) ){
            cout << "\n" << "Invalid input. Please select an index." << endl;
        }
        else{
            cout << "\n" << "Card chosen makes total greater than 31. Please select a different card." << endl;
        }
        cin.clear(); // reset failbit
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin >> index;
    }
    return hand[index];
}


/*****************************
 *----------PLAY!!!----------*
 *****************************/
// Player1: User
// Player2: Computer
void Cribbage::play()
{
    bool userGoesFirst = decideFirstCrib(); // If true, user has crib first. If false, computer has crib first
    while(true){
        cout << "check: " << endl;
        if (userGoesFirst){
            
            /***********************************************
             *-------PART 1a: USER CRIB, DISCARDING--------*
            ***********************************************/
            Card*** hands = dealCards();
            Player player1{hands[0]}; // Arrays sorted by player constructors
            Player player2{hands[1]};

            // Show user their cards
            cout << "\n" << "-----------YOU HAVE THE CRIB.-----------" << endl;
            cout << "\n" << "YOUR CARDS: ";
            printCardArray(player1.hand_, 6);

            // Print statements for debugging
            // cout << "Comp's Cards: ";
            // printCardArray(player2.hand_, 6);

            // Each player makes discards 2 cards
            Card*** userSplit = userSelectDiscards(player1.hand_);
            Card*** compSplit = player2.optimalDiscard(false);
            
            // Grab user's cards
            Card** userChoice = userSplit[0];

            // Grab comp's cards
            Card** compChoice = compSplit[0];

            // Make Crib
            Card** crib = makeCrib(userSplit[1], compSplit[1]);

            cout << "\n";
            

            // Flip the turn-up card and put it in each hand
            Card* turnUp = flipTurnCard();
            
            cout << "Turn-up card: " << *turnUp << "\n" << endl;
            if (turnUp->value() == 11){
                player1Score_ += 2;
                cout << "Turn-up Card is a Jack. You score 2." << endl;
                if (checkForWin()){
                    break;
                }
            }
            this_thread::sleep_for(chrono::seconds(1));

            // Make copies of these arrays so that we can score & print them later
            Card** userCopy = copyCardArray(userChoice, 4);
            Card** compCopy = copyCardArray(compChoice, 4);
            Card** cribCopy = copyCardArray(crib, 4);

            /***********************************************
             *---------PART 1b: USER CRIB, THE GO-----------*
            ***********************************************/
            // userChoice and compChoice are size 5 arrays, index 4 is nullptr so we're ok
            // b/c everything in Pegger class is hardcoded for an array size 4        
            theGo(userChoice, compChoice, true);
            if (checkForWin()){
                break;
            }

            /***********************************************
             *---------PART 1c: USER CRIB, SCORING-----------*
            ***********************************************/
            cout << "\n";
            cout << "Turn up card was: " << *turnUp << endl;

            cout << "\n" << "OPPONENT'S CARDS: ";
            printCardArray(compCopy, 4); // Print array before inserting turn up card
            size_t compInsert = Player::insertCardIntoSortedArray(compCopy, turnUp); // Insert the turn up card
            size_t compScore = player2.scoreOf(compCopy, 5, compInsert, false, true);
            cout << "Score: " << compScore << endl;
            player2Score_ += compScore;
            if (checkForWin()){
                break;
            }

            cout << "\n" << "YOUR CARDS: ";
            printCardArray(userCopy, 4);
            size_t userInsert = Player::insertCardIntoSortedArray(userCopy, turnUp);  
            size_t userScore = player1.scoreOf(userCopy, 5, userInsert, false, true);
            cout << "Score: " << userScore << endl;
            player1Score_ += userScore;
            if (checkForWin()){
                break;
            }

            cout << "\n" << "YOUR CRIB: ";
            printCardArray(cribCopy, 4);
            Player::insertCardIntoSortedArray(cribCopy, turnUp);
            size_t cribScore = player1.scoreOf(cribCopy, 5, 0, true, true);
            cout << "Score: " << cribScore << endl;
            player1Score_ += cribScore; 
            if (checkForWin()){
                break;
            }
            
            printScore();
            
            cout << "\n" << "Press <ENTER> to continue...";
            cin.get();
            cin.get();
            resetDeck();
            // delete[] hands;
        }
        cout << "check: " << endl;

        /***********************************************
         *-------PART 2a: COMP CRIB, DISCARDING--------*
        ***********************************************/
        Card*** hands = dealCards();
        Player player1{hands[0]}; // Arrays sorted by player constructors
        Player player2{hands[1]};

        // Show user their cards
        cout << "\n" << "-----------OPPONENT HAS THE CRIB.-----------" << endl; // **DIFFERENT
        cout << "\n" << "YOUR CARDS: ";
        printCardArray(player1.hand_, 6);

        // Print statements for debugging
        // cout << "Comp's Cards: ";
        // printCardArray(player2.hand_, 6);

        // Each player makes discards 2 cards
        Card*** userSplit = userSelectDiscards(player1.hand_);
        Card*** compSplit = player2.optimalDiscard(true);                       // **DIFFERENT
        
        // Grab user's cards
        Card** userChoice = userSplit[0];

        // Grab comp's cards
        Card** compChoice = compSplit[0];

        // Make Crib
        Card** crib = makeCrib(userSplit[1], compSplit[1]);

        cout << "\n";
        // Flip the turn-up card and put it in each hand
        Card* turnUp = flipTurnCard();
        
        cout << "Turn-up card: " << *turnUp << "\n" << endl;
        if (turnUp->value() == 11){
            player2Score_ += 2;
            cout << "Turn-up Card is a Jack. Opponent scores 2." << endl;       // **DIFFERENT
            if (checkForWin()){
                break;
            }        
        }
        this_thread::sleep_for(chrono::seconds(1));

        // Make copies of these arrays so that we can score & print them later
        Card** userCopy = copyCardArray(userChoice, 4);
        Card** compCopy = copyCardArray(compChoice, 4);
        Card** cribCopy = copyCardArray(crib, 4);

        /***********************************************
         *---------PART 2b: COMP CRIB, THE GO-----------*
        ***********************************************/
        // userChoice and compChoice are size 5 arrays, index 4 is nullptr so we're ok
        // b/c everything in Pegger class is hardcoded for an array size 4        
        theGo(userChoice, compChoice, false);  
        if (checkForWin()){
            break;
        } 

        /***********************************************
         *---------PART 2c: USER CRIB, SCORING-----------*
        ***********************************************/
        cout << "\n";
        cout << "Turn up card was: " << *turnUp << endl;

        cout << "\n" << "YOUR CARDS: ";
        printCardArray(userCopy, 4);
        size_t userInsert = Player::insertCardIntoSortedArray(userCopy, turnUp); // Insert the turn up card
        size_t userScore = player1.scoreOf(userCopy, 5, userInsert, false, true);
        cout << "Score: " << userScore << endl;
        player1Score_ += userScore;
        if (checkForWin()){
            break;
        }

        cout << "\n" << "OPPONENT'S CARDS: ";
        printCardArray(compCopy, 4);
        size_t compInsert = Player::insertCardIntoSortedArray(compCopy, turnUp); // Insert the turn up card
        size_t compScore = player2.scoreOf(compCopy, 5, compInsert, false, true);
        cout << "Score: " << compScore << endl;
        player2Score_ += compScore;
        if (checkForWin()){
            break;
        }

        cout << "\n" << "OPPONENT'S CRIB: ";
        printCardArray(cribCopy, 4);
        Player::insertCardIntoSortedArray(cribCopy, turnUp); // Insert the turn up card
        size_t cribScore = player2.scoreOf(cribCopy, 5, 0, true, true);
        cout << "Score: " << cribScore << endl;
        player2Score_ += cribScore; 
        if (checkForWin()){
            break;
        }

        printScore();
        
        cout << "\n" << "Press <ENTER> to continue...";
        cin.get();
        cin.get();
        
        resetDeck();

        // Allow user to play now if they didn't go first
        userGoesFirst = true;
    }

    // Print the results if someone has won
    if (player1Score_ > player2Score_){
        cout << "\n" << "CONGRATULATIONS! YOU WON!" << endl;
    }
    else{
        cout << "\n" << "You have been defeated." << endl;
    }
    printScore();
}

int main()
{
    Cribbage c;
    c.play();

    return 0;
}