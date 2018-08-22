#include "graph.hpp"
#include <cstddef>
#include <iostream>
#include <unordered_set>
#include <queue>
#include <cstring>

using namespace std;

template<size_t width, size_t height>
SlidePuzzle<width, height>::SlidePuzzle(const int* grid)
{
  for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
    int num = grid[i];
    grid_[i] = num;
    if (num == -1) {
      emptyPosition_ = i;
    }
  }
}  

template<size_t width, size_t height>
SlidePuzzle<width, height>::~SlidePuzzle()
{
  // delete[] grid_;
}

template<size_t width, size_t height>
bool SlidePuzzle<width, height>::operator==(const SlidePuzzle<width, height>& other) const
{
  for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
    if (grid_[i] != other.grid_[i]) {
      return false;
    }
  }

  return true;
}

template<size_t width, size_t height>
size_t SlidePuzzle<width, height>::hashFunction() const
{
  hash<int> hasher;
  size_t result = 0;
  
  for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
    result = result * 31 + hasher(grid_[i]);
  }

  return result;
}

template<size_t width, size_t height>
Graph<SlidePuzzle<width, height>, SlidePuzzle<width, height>::adjacent> 
    SlidePuzzle<width, height>::getAllTransformations() const
{
  Graph<SlidePuzzle<width, height>, SlidePuzzle<width, height>::adjacent> graph;
  graph.addVertex(*this);
  queue<SlidePuzzle<width, height>> q( {*this} );

  while (!q.empty()) {
    SlidePuzzle& puzzle = q.front();
    puzzle.addAllSwaps(graph, q);
    q.pop();
  }

  return graph;
}

template<size_t width, size_t height>
void SlidePuzzle<width, height>::addAllSwaps(
    Graph<SlidePuzzle<width, height>, SlidePuzzle<width, height>::adjacent>& graph, 
    queue<SlidePuzzle<width, height>>& q) const
{
  // TODO: Store these ???
  size_t row = emptyPosition_ / height;
  size_t col = emptyPosition_ % width; 

  // Check above
  if (row != 0) {
    int copy[NUM_ELEMENTS];
    memcpy(copy, grid_, sizeof(int) * NUM_ELEMENTS);
    std::swap(copy[emptyPosition_], copy[emptyPosition_ - width]);

    SlidePuzzle<width, height> swap{copy};
    if (graph.addVertex(swap)) {
      q.push(std::move(swap));
    }
  }

  // Check below
  if (row != height - 1) {
    int copy[NUM_ELEMENTS];
    memcpy(copy, grid_, sizeof(int) * NUM_ELEMENTS);
    std::swap(copy[emptyPosition_], copy[emptyPosition_ + width]);
    
    SlidePuzzle<width, height> swap{copy};
    if (graph.addVertex(swap)) {
      q.push(std::move(swap));
    }
  }

  // Check to the left
  if (col != 0) {
    int copy[NUM_ELEMENTS];
    memcpy(copy, grid_, sizeof(int) * NUM_ELEMENTS);
    std::swap(copy[emptyPosition_], copy[emptyPosition_ - 1]);
    
    SlidePuzzle<width, height> swap{copy};
    if (graph.addVertex(swap)) {
      q.push(std::move(swap));
    }
  }

  // Check to the right
  if (col != width - 1) {
    int copy[NUM_ELEMENTS];
    memcpy(copy, grid_, sizeof(int) * NUM_ELEMENTS);
    std::swap(copy[emptyPosition_], copy[emptyPosition_ + 1]);
    
    SlidePuzzle<width, height> swap{copy};
    if (graph.addVertex(swap)) {
      q.push(std::move(swap));
    }
  }
}

template<size_t width, size_t height>
bool SlidePuzzle<width, height>::adjacent(const SlidePuzzle<width, height>& first, const SlidePuzzle<width, height>& second)
{
  // Check for adjacent empty position
  int adjacentEmptyPosition = first.getAdjacentEmptyPosition(second);
  if (adjacentEmptyPosition == NONE) {
    return false;
  }

  // Check that the same value was swapped from the empty position
  if (first.grid_[second.emptyPosition_] != second.grid_[first.emptyPosition_]) {
    return false;
  }

  // Check to make sure all other values are in the same position
  for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
    if (i != first.emptyPosition_ && i != second.emptyPosition_) {
      if (first.grid_[i] != second.grid_[i]) {
        return false;
      }
    }
  }

  return true;
}

template<size_t width, size_t height>
int SlidePuzzle<width, height>::getAdjacentEmptyPosition(const SlidePuzzle<width, height>& other) const
{
  // TODO: Store these ???
  size_t row = emptyPosition_ / height;
  size_t col = emptyPosition_ % width; 

  // Check above
  if (row != 0) {
    if (other.emptyPosition_ == emptyPosition_ - width) {
      return emptyPosition_ - 3;
    }
  }

  // Check below
  if (row != height - 1) {
    if (other.emptyPosition_ == emptyPosition_ + width) {
      return emptyPosition_ + 3;
    }
  }

  // Check to the left
  if (col != 0) {
    if (other.emptyPosition_ == emptyPosition_ - 1) {
      return emptyPosition_ - 1;
    }
  }

  // Check to the right
  if (col != width - 1) {
    if (other.emptyPosition_ == emptyPosition_ + 1) {
      return emptyPosition_ + 1;
    }
  }

  return NONE;
}