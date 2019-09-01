#include "win_percentages.hpp"

using namespace std;

WinPercentages::WinPercentages(int* info)
    : players_{new Player[9]}, num_players_{0}, board_size_{0} {
  deck_.reserve(52);

  srand(unsigned(time(NULL)));

  uchar i;

  // Populate the deck
  // TODO: emplace_back ???
  for (i = 0; i < 52; ++i) {
    deck_.emplace_back(i);
  }

  // Add Players
  for (i = 0; i < info[0]; ++i) {
    addPlayer();
  }

  // Add board cards (slots 1 through 5)
  for (i = 1; i < 6; ++i) {
    if (info[i] != UNKNOWN) {
      addUserSelected(uchar(info[i]), BOARD);
    }
  }

  // Add cards to each player
  uchar upper_bound = 6 + num_players_ * 2;
  for (i = 6; i < upper_bound; ++i) {
    if (info[i] != UNKNOWN) {
      addUserSelected(uchar(info[i]),
                      (i - 6) / 2);  // Convert index to player position
    }
  }
}

WinPercentages::~WinPercentages() { delete[] players_; }

void WinPercentages::addPlayer() {
  players_[num_players_] = Player();
  ++num_players_;
}

void WinPercentages::addUserSelected(uchar cardNum, short pos) {
  Card c{cardNum};  // Make card
  // Add to all players (board)
  if (pos == BOARD) {
    ++board_size_;
    for (uchar i = 0; i < num_players_; ++i) {
      players_[i].addOriginalBoardCard(c);  // Give card to player
    }
  }
  // Add to single player
  else {
    players_[pos].addOriginalHoleCard(c);  // Give card to each player
  }

  // Remove card from deck
  removeFromVector(c, deck_);
}

double** WinPercentages::getWinAndTiePercentages() {
  size_t* winCounts = new size_t[num_players_];
  size_t* tieCounts = new size_t[num_players_];
  uchar* emptyTiedPositions = new uchar[num_players_];
  for (uchar i = 0; i < num_players_; ++i) {
    emptyTiedPositions[i] = 0;
  }
  uchar* tiedPositions = new uchar[num_players_];

  for (size_t iters = 0; iters < NUM_ITERS; ++iters) {
    // Finish the board
    for (uchar i = board_size_; i < 5; ++i) {
      dealRandomCard(BOARD);
    }

    // Init comparison vars
    Hand bestHand{HIGH_CARD, 0, 0};
    uchar bestPlayerPos = 0;
    uchar numPlayersTied = 1;
    memcpy(tiedPositions, emptyTiedPositions, num_players_);

    for (uchar pos = 0; pos < num_players_; ++pos) {
      Player& player = players_[pos];
      // Finish hole cards
      for (uchar i = player.num_cards_; i < 2; ++i) {
        dealRandomCard(pos);
      }

      Hand myHand = player.getBestHand();
      short comparison = myHand.compareTo(bestHand);

      if (comparison > 0) {
        bestHand = myHand;
        bestPlayerPos = pos;

        tiedPositions[0] = pos;
        numPlayersTied = 1;
      } else if (comparison == 0) {
        tiedPositions[numPlayersTied] = pos;
        ++numPlayersTied;
      }

      // Reset the player's values
      memcpy(player.value_arr_, player.orig_value_arr_, NUMVALUES);
      player.suit_counts_ = player.orig_suit_counts_;
    }

    if (numPlayersTied != 1) {
      for (uchar i = 0; i < numPlayersTied; ++i) {
        ++tieCounts[tiedPositions[i]];
      }
    } else {
      ++winCounts[bestPlayerPos];
    }

    // Reset deck
    for (Card& c : deck_) {
      c.isDealt_ = false;
    }
  }
  delete[] tiedPositions;
  delete[] emptyTiedPositions;
  return convertToPercentages(winCounts, tieCounts);
}

double** WinPercentages::convertToPercentages(size_t* winCounts,
                                              size_t* tieCounts) {
  double* winPercentages = new double[num_players_];
  double* tiePercentages = new double[num_players_];

  for (uchar i = 0; i < num_players_; ++i) {
    winPercentages[i] = winCounts[i] * 1.0 / NUM_ITERS;
    tiePercentages[i] = tieCounts[i] * 1.0 / NUM_ITERS;
  }

  delete[] winCounts;
  delete[] tieCounts;

  return avgEmptyHands(winPercentages, tiePercentages);
}

double** WinPercentages::avgEmptyHands(double* winPercentages,
                                       double* tiePercentages) {
  double emptyWins = 0;
  double emptyTies = 0;
  size_t numEmptyPositions = 0;
  size_t* emptyPositions = new size_t[num_players_];

  for (uchar i = 0; i < num_players_; ++i) {
    if (players_[i].num_cards_ == 0) {
      emptyWins += winPercentages[i];
      emptyTies += tiePercentages[i];
      emptyPositions[numEmptyPositions] = i;
      ++numEmptyPositions;
    }
  }

  double avgWins = emptyWins / numEmptyPositions;
  double avgTies = emptyTies / numEmptyPositions;

  for (uchar i = 0; i < numEmptyPositions; ++i) {
    uchar pos = emptyPositions[i];
    winPercentages[pos] = avgWins;
    tiePercentages[pos] = avgTies;
  }

  double** percentages = new double*[2];
  percentages[0] = winPercentages;
  percentages[1] = tiePercentages;

  delete[] emptyPositions;
  return percentages;
}

void WinPercentages::dealRandomCard(short pos) {
  // TODO: convert deck_ to an array after I erase the necessary cards ???

  // Get random card
  Card* cardPtr;
  bool isInDeck = false;
  while (!isInDeck) {
    int n = rand() % deck_.size();
    Card& deckCard = deck_[n];
    if (!deckCard.isDealt_) {
      cardPtr = &deckCard;
      deckCard.isDealt_ = true;  // Mark the card as used (but don't remove it)
      isInDeck = true;
    }
  }

  // Add to all players (board)
  if (pos == BOARD) {
    for (uchar i = 0; i < num_players_; ++i) {
      players_[i].addSingleCard(*cardPtr);  // Give card to each player
    }

  }
  // Add to single player
  else {
    players_[pos].addSingleCard(*cardPtr);  // Give card to player
  }
}

void WinPercentages::printDeck() {
  size_t length = deck_.size();
  cout << "Deck:";
  cout << "[";
  for (size_t i = 0; i < length; ++i) {
    cout << (deck_[i]);
    if (i != length - 1) {
      cout << ", ";
    }
  }
  cout << "]" << endl;
}

void WinPercentages::printWinAndTiePercentages(double** result) {
  double* winCounts = result[0];
  double* tieCounts = result[1];

  cout << "Win Counts:";
  cout << "[";
  for (size_t i = 0; i < num_players_; ++i) {
    cout << winCounts[i];
    if (i != size_t(num_players_ - 1)) {
      cout << ", ";
    }
  }
  cout << "]" << endl;

  cout << "Tie Counts:";
  cout << "[";
  for (size_t i = 0; i < num_players_; ++i) {
    cout << tieCounts[i];
    if (i != size_t(num_players_ - 1)) {
      cout << ", ";
    }
  }
  cout << "]" << endl;
}

int main() {
  int* info = new int[24]{9,  -1, -1, -1, -1, -1, 24, 25, -1, -1, -1, -1,
                          -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
  WinPercentages wp{info};

  auto start = chrono::steady_clock::now();
  double** result = wp.getWinAndTiePercentages();
  auto stop = chrono::steady_clock::now();

  double elapsedTime =
      chrono::duration_cast<chrono::nanoseconds>(stop - start).count() /
      1000000000.0;

  wp.printWinAndTiePercentages(result);
  cout << "Elapsed time: " << elapsedTime << endl;

  delete[] result[0];
  delete[] result[1];
  delete[] result;
  delete[] info;
}

// int main()
// {
//     int* info = new int[24]{1, 0,-1,-1,-1,-1, 51,-1, -1,-1, -1,-1, -1,-1,
//     -1,-1, -1,-1, -1,-1, -1,-1, -1,-1,};

//     WinPercentages wp{info};

//     auto start = chrono::steady_clock::now();

//     size_t arr[52] = {0};
//     for (size_t iters = 0; iters < 200000; ++iters){
//         for (size_t i = 0; i < 20; ++i){
//             size_t n = wp.dealRandomCard(BOARD);
//             ++arr[n];
//         }
//         wp.reset();
//     }

//     auto stop = chrono::steady_clock::now();
//     double elapsedTime = chrono::duration_cast<chrono::nanoseconds>(stop -
//     start).count()/1000000000.0; cout << "Elapsed time: " << elapsedTime <<
//     endl;

//     cout << "Counts:";
//     cout << "[";
//     for (size_t i = 0; i < 52; ++i){
//         cout << (arr[i]);
//         if (i != 51){
//             cout << ", ";
//         }
//     }
//     cout << "]" << endl;

//     //cout << wp << endl;

//     delete[] info;
//     return 0;

// }