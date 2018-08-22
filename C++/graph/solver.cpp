#include "slide_puzzle.hpp"
#include <iostream>
#include <cstddef>
#include <random>
#include <time.h>
#include <unordered_set>
#include "../testing_program/testing-program.hpp"

using namespace std;

typedef SlidePuzzle<3, 3> slide_t;

int a[9] = {1,2,3,4,5,6,7,8,-1};
int b[9] = {1,2,3,4,5,6,7,-1,8};
int c[9] = {2,4,5,6,-1,4,3,2,1};
int d[9] = {1,2,4,3,5,-1,7,8,6};

void test_adjacentEmptyPosition()
{
  TestingProgram tester{"Adjacent empty position"};

  slide_t s{a};

  affirm(s.getAdjacentEmptyPosition(slide_t{b}) == 7);
  affirm(s.getAdjacentEmptyPosition(slide_t{c}) == -1);

  TestingProgram::printResults();
}

void test_adjacent()
{
  TestingProgram tester{"Adjacent"};

  slide_t s{a};

  affirm(slide_t::adjacent(s, slide_t{b}));
  affirm(!slide_t::adjacent(s, slide_t{c}));
  affirm(!slide_t::adjacent(s, slide_t{d}));

  TestingProgram::printResults();
}

int main() 
{
  test_adjacentEmptyPosition();
  test_adjacent();

  TestingProgram::summarize();
  // srand(time(NULL));
  
  // const size_t width = 3;
  // const size_t height = 3;
  
  // unordered_set<SlidePuzzle<width, height>> set;

  // for (size_t i = 0; i < 10; ++i) {

  //   int grid[NUM_ELEMENTS];
  //   for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
  //     grid[i] = rand() % 10;
  //   }

  //   set.emplace(grid);
  // }

  // size_t nbuckets = set.bucket_count();
  // for (size_t i = 0; i < nbuckets; ++i) {
  //   std::cout << "bucket #" << i << " has " << set.bucket_size(i) << " elements.\n";
  // }
  // return 0;
}