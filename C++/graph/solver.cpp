#include "slide_puzzle.hpp"
#include <iostream>
#include <cstddef>
#include <random>
#include <time.h>
#include <unordered_set>

using namespace std;

#define ROWS 3
#define COLS 3
#define NUM_ELEMENTS ROWS * COLS

int main() 
{
  srand(time(NULL));
  
  const size_t width = 3;
  const size_t height = 3;
  
  unordered_set<SlidePuzzle<width, height>> set;

  for (size_t i = 0; i < 2000; ++i) {

    int grid[NUM_ELEMENTS];
    for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
      grid[i] = rand() % 10;
    }

    set.emplace(grid);
  }

  size_t nbuckets = set.bucket_count();
  for (size_t i = 0; i < nbuckets; ++i) {
    std::cout << "bucket #" << i << " has " << set.bucket_size(i) << " elements.\n";
  }
  return 0;
}