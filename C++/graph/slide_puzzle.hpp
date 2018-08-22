#ifndef SLIDE_PUZZLE_HPP
#define SLIDE_PUZZLE_HPP 1

#include <cstddef>
#include <iostream>
#include <unordered_set>

template<size_t width, size_t height>
class SlidePuzzle {
  public:
    SlidePuzzle() = delete;
    SlidePuzzle(const int* grid); ///< Must have one -1 to signal empty position
    ~SlidePuzzle();
    
    bool operator==(const SlidePuzzle& other) const;
    size_t hashFunction() const;
    
    std::unordered_set<SlidePuzzle> getAllTransformations() const;
    
    static bool adjacent(const SlidePuzzle& first, const SlidePuzzle& second);

    // private:
      int getAdjacentEmptyPosition(const SlidePuzzle& other) const;

      static const int NONE = -1;
      static const size_t NUM_ELEMENTS = width * height;

      int* grid_;
      size_t emptyPosition_;
};

namespace std
{
  template<size_t width, size_t height>
  class hash<SlidePuzzle<width, height>> {
    public:
      using argument_type = SlidePuzzle<width, height>;
      using result_type = size_t;

      result_type operator()(const argument_type& puzzle) const
      {
        return puzzle.hashFunction();
      }
  };
}


#endif // SLIDE_PUZZLE_HPP

#include "slide_puzzle_private.hpp"
