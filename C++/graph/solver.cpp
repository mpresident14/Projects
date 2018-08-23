#include "slide_puzzle.hpp"
#include "graph.hpp"
#include "../testing_program/testing-program.hpp"
#include <iostream>
#include <cstddef>
#include <random>
#include <time.h>
#include <unordered_set>
#include <queue>
#include <chrono>

using namespace std;

typedef SlidePuzzle<3, 3> slide3_t;

int a[9] = {1,2,3,4,5,6,7,8,-1};
int b[9] = {1,2,3,4,5,6,7,-1,8};
int c[9] = {2,7,5,6,-1,4,3,8,1};
int d[9] = {1,2,4,3,5,-1,7,8,6};
int e[9] = {1,2,3,4,5,-1,7,8,6};

typedef SlidePuzzle<2, 2> slide2_t;
int f[9] = {1,2,3,-1};

typedef SlidePuzzle<2, 3> slide23_t;
int g[9] = {1,2,3,-1,4,5};

void test_addAllSwaps()
{
  TestingProgram tester{"Add all swaps"};

  slide3_t s{a};
  Graph<slide3_t> graph;
  queue<slide3_t> q;
  s.addAllSwaps(graph, q);

  affirm(graph.size() == 2);
  affirm(q.size() == 2);

  slide3_t{b}.addAllSwaps(graph, q);
  affirm(graph.size() == 5);
  affirm(q.size() == 5);

  // -----------------------------------------

  Graph<slide3_t> graph2;
  queue<slide3_t> q2;
  slide3_t s2{c};
  s2.addAllSwaps(graph2, q2);

  affirm(graph2.size() == 4);
  affirm(q2.size() == 4);

  TestingProgram::printResults();
}

void test_getAllTransformations()
{
  TestingProgram tester{"Get All Transformations"};

  slide2_t s2{f};
  Graph<slide2_t> graph{s2.getAllTransformations()};
  // 4 empty spots, 3 ways to organize 1,2,3 in the same order ([123, 231, 321]). 4 * 3 = 12.
  // *Also, the number of possible states of a m x n grid seems to be (m*n)! / 2
  affirm(graph.size() == 12);

  // -----------------------------------------------------------------------------------
  
  slide3_t s3{a};
  auto start = chrono::steady_clock::now();
  s3.getAllTransformations();
  auto stop = chrono::steady_clock::now();
  double elapsedTime = chrono::duration_cast<chrono::nanoseconds>(stop - start).count()/1000000000.0;
  cout << "3x3 grid took " << elapsedTime << " seconds." << endl;
  
  TestingProgram::printResults();
}

int main() 
{
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