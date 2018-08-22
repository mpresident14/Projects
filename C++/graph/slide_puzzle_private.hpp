#include <cstddef>
#include <iostream>
#include <unordered_set>

using namespace std;

template<size_t width, size_t height>
SlidePuzzle<width, height>::SlidePuzzle()
  : grid_{nullptr}, width_{0}, height_{0}
{

}

template<size_t width, size_t height>
SlidePuzzle<width, height>::SlidePuzzle(const int* grid)
  : grid_{new int[width*height]}, width_{width}, height_{height}
{
  size_t numElements = width * height;
  for (size_t i = 0; i < numElements; ++i) {
    grid_[i] = grid[i];
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
  if (width_ != other.width_ || height_ != other.height_) {
    return false;
  }

  size_t numElements = width_ * height_;
  for (size_t i = 0; i < numElements; ++i) {
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
  
  size_t numElements = width_ * height_;
  for (size_t i = 0; i < numElements; ++i) {
    result = result * 31 + hasher(grid_[i]);
  }

  return result;
}

template<size_t width, size_t height>
bool SlidePuzzle<width, height>::adjacent(const SlidePuzzle<width, height>& first, const SlidePuzzle<width, height>& seccond)
{
  return true;
}