#include "graph.hpp"
#include <cstddef>
#include <iostream>
#include <unordered_set>
#include <queue>
#include <cstring>

using namespace std;

void trade(int *x, int *y)
{
    *x = *x ^ *y;
    *y = *x ^ *y;
    *x = *x ^ *y;
}

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
bool SlidePuzzle<width, height>::operator==(const SlidePuzzle<width, height>& other) const
{
  return memcmp(grid_, other.grid_, NUM_ARRAY_BYTES) == 0;
}

template<size_t width, size_t height>
SlidePuzzle<width, height>::SlidePuzzle(const SlidePuzzle<width, height>& other)
  : emptyPosition_{other.emptyPosition_}
{
  memcpy(grid_, other.grid_, NUM_ARRAY_BYTES);
}

template<size_t width, size_t height>
SlidePuzzle<width, height>::SlidePuzzle(SlidePuzzle<width, height>&& other)
{
  swap(other);
}

template<size_t width, size_t height>
SlidePuzzle<width, height>& SlidePuzzle<width, height>::operator=(SlidePuzzle<width, height> other)
{
  swap(other);
  return *this;
}

template<size_t width, size_t height>
void SlidePuzzle<width, height>::swap(SlidePuzzle<width, height>& other)
{
  std::swap(grid_, other.grid_);
  std::swap(emptyPosition_, other.emptyPosition_);
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

// TODO: stop searching when we find the desired end state, then use 
// Graph::getShortestPath to construct the path
template<size_t width, size_t height>
Graph<SlidePuzzle<width, height>> SlidePuzzle<width, height>::getAllTransformations() const
{
  Graph<SlidePuzzle<width, height>> graph;
  graph.addVertex(*this);
  queue<SlidePuzzle<width, height>> q( {*this} );

  while (!q.empty()) {
    q.front().addAllSwaps(graph, q);
    q.pop();
  }

  return graph;
}

template<size_t width, size_t height>
void SlidePuzzle<width, height>::addAllSwaps(
    Graph<SlidePuzzle<width, height>>& graph, queue<SlidePuzzle<width, height>>& q) const
{
  size_t row = emptyPosition_ / width;
  size_t col = emptyPosition_ % width; 

  // Check above
  if (row != 0) {
    SlidePuzzle<width, height> copy{*this};
    trade(copy.grid_ + emptyPosition_, copy.grid_ + emptyPosition_ - width);
    copy.emptyPosition_ -= width;
    if (graph.addVertex(copy)) {;
      graph.addEdge(*this, copy);
      q.push(std::move(copy));
    }
  }

  // Check below
  if (row != height - 1) {
    SlidePuzzle<width, height> copy{*this};
    int temp = copy.grid_[copy.emptyPosition_];
    copy.grid_[copy.emptyPosition_] = copy.grid_[copy.emptyPosition_ + width];
    copy.grid_[copy.emptyPosition_ + width] = temp;
    copy.emptyPosition_ += width;
    if (graph.addVertex(copy)) {
      graph.addEdge(*this, copy);
      q.push(std::move(copy));
    }
  }

  // Check to the left
  if (col != 0) {
    SlidePuzzle<width, height> copy{*this};
    trade(copy.grid_ + emptyPosition_, copy.grid_ + emptyPosition_ -1);
    --copy.emptyPosition_;
    if (graph.addVertex(copy)) {
      graph.addEdge(*this, copy);
      q.push(std::move(copy));
    }
  }

  // Check to the right
  if (col != width - 1) {
    SlidePuzzle<width, height> copy{*this};
    trade(copy.grid_ + emptyPosition_, copy.grid_ + emptyPosition_ + 1);
    ++copy.emptyPosition_;
    if (graph.addVertex(copy)) {
      graph.addEdge(*this, copy);
      q.push(std::move(copy));
    }
  }
}
