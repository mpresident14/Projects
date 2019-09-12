#include <cstddef>
#include <cstring>
#include <iostream>
#include <queue>
#include <unordered_set>
#include "graph.hpp"

using namespace std;

void trade(int* x, int* y) {
  *x = *x ^ *y;
  *y = *x ^ *y;
  *x = *x ^ *y;
}

template <size_t width, size_t height>
SlidePuzzle<width, height>::SlidePuzzle(const int* grid) {
  bool isEmptyPositionSet = false;
  for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
    int num = grid[i];
    grid_[i] = num;
    if (num == -1) {
      if (isEmptyPositionSet) {
        throw std::invalid_argument(
            "Exactly one -1 must be present in parameter \"grid.\"");
      }
      emptyPosition_ = i;
      isEmptyPositionSet = true;
    }
  }

  if (!isEmptyPositionSet) {
    throw std::invalid_argument(
        "Exactly one -1 must be present in parameter \"grid.\"");
  }
}

template <size_t width, size_t height>
bool SlidePuzzle<width, height>::operator==(
    const SlidePuzzle<width, height>& other) const {
  return memcmp(grid_, other.grid_, NUM_ARRAY_BYTES) == 0;
}

template <size_t width, size_t height>
bool SlidePuzzle<width, height>::operator!=(
    const SlidePuzzle<width, height>& other) const {
  return !(*this == other);
}

template <size_t width, size_t height>
SlidePuzzle<width, height>::SlidePuzzle(const SlidePuzzle<width, height>& other)
    : emptyPosition_{other.emptyPosition_} {
  memcpy(grid_, other.grid_, NUM_ARRAY_BYTES);
}

template <size_t width, size_t height>
SlidePuzzle<width, height>::SlidePuzzle(SlidePuzzle<width, height>&& other) {
  swap(other);
}

template <size_t width, size_t height>
SlidePuzzle<width, height>& SlidePuzzle<width, height>::operator=(
    SlidePuzzle<width, height> other) {
  swap(other);
  return *this;
}

template <size_t width, size_t height>
void SlidePuzzle<width, height>::swap(SlidePuzzle<width, height>& other) {
  std::swap(grid_, other.grid_);
  std::swap(emptyPosition_, other.emptyPosition_);
}

template <size_t width, size_t height>
size_t SlidePuzzle<width, height>::hashFunction() const {
  hash<int> hasher;
  size_t result = 0;

  for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
    result = result * 31 + hasher(grid_[i]);
  }

  return result;
}

template <size_t width, size_t height>
std::forward_list<SlidePuzzle<width, height>>
SlidePuzzle<width, height>::solvePuzzle(
    const SlidePuzzle<width, height>& completed) const {
  Graph<SlidePuzzle<width, height>> graph;
  graph.addVertex(*this);
  queue<SlidePuzzle<width, height>> q({*this});

  while (!q.empty()) {
    SlidePuzzle<width, height>& puzzle = q.front();
    if (puzzle == completed) {
      break;
    }

    puzzle.addAllSwaps(graph, q);
    q.pop();
  }

  return graph.getShortestPath(*this, completed);
}

/* Adds all of the possible SlidePuzzles that could be created by performing one
 * slide (4 is max possible) */
template <size_t width, size_t height>
void SlidePuzzle<width, height>::addAllSwaps(
    Graph<SlidePuzzle<width, height>>& graph,
    queue<SlidePuzzle<width, height>>& q) const {
  size_t row = emptyPosition_ / width;
  size_t col = emptyPosition_ % width;

  // Check above
  if (row != 0) {
    SlidePuzzle<width, height> copy{*this};
    trade(copy.grid_ + emptyPosition_, copy.grid_ + emptyPosition_ - width);
    copy.emptyPosition_ -= width;
    if (graph.addVertex(copy)) {
      ;
      graph.addEdge(*this, copy);
      q.push(std::move(copy));
    }
  }

  // Check below
  if (row != height - 1) {
    SlidePuzzle<width, height> copy{*this};
    trade(copy.grid_ + emptyPosition_, copy.grid_ + emptyPosition_ + width);
    copy.emptyPosition_ += width;
    if (graph.addVertex(copy)) {
      graph.addEdge(*this, copy);
      q.push(std::move(copy));
    }
  }

  // Check to the left
  if (col != 0) {
    SlidePuzzle<width, height> copy{*this};
    trade(copy.grid_ + emptyPosition_, copy.grid_ + emptyPosition_ - 1);
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
