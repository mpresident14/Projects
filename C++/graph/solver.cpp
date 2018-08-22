#include "slide_puzzle.hpp"
#include "graph.hpp"
#include "../testing_program/testing-program.hpp"
#include <iostream>
#include <cstddef>
#include <random>
#include <time.h>
#include <unordered_set>
#include <queue>

using namespace std;

typedef SlidePuzzle<3, 3> slide3_t;

int a[9] = {1,2,3,4,5,6,7,8,-1};
int b[9] = {1,2,3,4,5,6,7,-1,8};
int c[9] = {2,4,5,6,-1,4,3,2,1};
int d[9] = {1,2,4,3,5,-1,7,8,6};

typedef SlidePuzzle<2, 2> slide2_t;
int e1[9] = {1,2,3,-1};

void test_adjacentEmptyPosition()
{
  TestingProgram tester{"Adjacent empty position"};

  slide3_t s{a};

  affirm(s.getAdjacentEmptyPosition(slide3_t{b}) == 7);
  affirm(s.getAdjacentEmptyPosition(slide3_t{c}) == -1);

  TestingProgram::printResults();
}

void test_adjacent()
{
  TestingProgram tester{"Adjacent"};

  slide3_t s{a};

  affirm(slide3_t::adjacent(s, slide3_t{b}));
  affirm(!slide3_t::adjacent(s, slide3_t{c}));
  affirm(!slide3_t::adjacent(s, slide3_t{d}));

  TestingProgram::printResults();
}

void test_addAllSwaps()
{
  TestingProgram tester{"Add all swaps"};

  slide3_t s{a};
  Graph<slide3_t, slide3_t::adjacent> graph;
  queue<slide3_t> q;
  s.addAllSwaps(graph, q);

  for (auto& pair : graph) {
    affirm(slide3_t::adjacent(s, pair.first));
  }

  affirm(graph.size() == 2);
  affirm(q.size() == 2);

  s.addAllSwaps(graph, q);
  affirm(graph.size() == 2);
  affirm(q.size() == 2);

  // -----------------------------------------

  Graph<slide3_t, slide3_t::adjacent> graph2;
  queue<slide3_t> q2;
  slide3_t s2{c};
  s2.addAllSwaps(graph2, q2);

  for (auto& pair : graph2) {
    affirm(slide3_t::adjacent(s2, pair.first));
  }

  affirm(graph2.size() == 4);
  affirm(q2.size() == 4);

  TestingProgram::printResults();
}

void test_getAllTransformations()
{
  TestingProgram tester{"Get All Transformations"};

  slide3_t s{a}; /* slide2_t s{e1}; */

  // 4 empty spots, 3 ways to organize 1,2,3 in the same order ([123, 231, 321]). 4 * 3 = 12.
  affirm(s.getAllTransformations().size() == 12);

  TestingProgram::printResults();
}

int main() 
{
  test_adjacentEmptyPosition();
  test_adjacent();
  test_addAllSwaps();
  test_getAllTransformations();

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