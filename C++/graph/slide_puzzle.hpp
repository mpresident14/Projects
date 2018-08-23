#ifndef SLIDE_PUZZLE_HPP
#define SLIDE_PUZZLE_HPP 1

#include "graph.hpp"
#include <cstddef>
#include <iostream>
#include <iomanip>
#include <queue>
#include <forward_list>

template<size_t width, size_t height>
class SlidePuzzle {
  public:
    SlidePuzzle() = delete;
    SlidePuzzle(const int* grid); // Must have exactly one -1 to signal empty position
    SlidePuzzle(const SlidePuzzle& other);
    SlidePuzzle(SlidePuzzle&& other);
    ~SlidePuzzle() = default;
    SlidePuzzle& operator=(SlidePuzzle other);
    void swap(SlidePuzzle& other);
    
    bool operator==(const SlidePuzzle& other) const;
    bool operator!=(const SlidePuzzle& other) const;
    size_t hashFunction() const;

    std::forward_list<SlidePuzzle> solvePuzzle(const SlidePuzzle& completed) const;

    friend inline std::ostream& operator<<(std::ostream& out, const SlidePuzzle<width, height>& puzzle)
    {
      for (size_t row = 0; row < height; ++row) {
        for (size_t col = 0; col < width; ++col) {
          size_t i = row*width + col;
          if (i == puzzle.emptyPosition_) {
            out << std::setw(3) << " ";
          }
          else {
            out << std::setw(3) << puzzle.grid_[i];
          }  
        }  
        out << endl;
      }
      return out;
    }

  private:
    void addAllSwaps(Graph<SlidePuzzle>& graph, std::queue<SlidePuzzle>& q) const;

    static const size_t NUM_ELEMENTS = width * height;
    static const size_t NUM_ARRAY_BYTES = NUM_ELEMENTS * sizeof(int);

    int grid_[NUM_ELEMENTS];
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
