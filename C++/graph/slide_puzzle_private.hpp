#include <cstddef>
#include <iostream>
#include <unordered_set>

using namespace std;

template<size_t width, size_t height>
SlidePuzzle<width, height>::SlidePuzzle(const int* grid)
  : grid_{new int[NUM_ELEMENTS]}
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
  delete[] grid_;
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
    if (other.emptyPosition_ == emptyPosition_ - 3) {
      return emptyPosition_ - 3;
    }
  }

  // Check below
  if (row != height - 1) {
    if (other.emptyPosition_ == emptyPosition_ + 3) {
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