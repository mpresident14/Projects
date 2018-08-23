#include "slide_puzzle.hpp"
#include <iostream>
#include <cstddef>
#include <iomanip>
#include <forward_list>

using namespace std;

typedef SlidePuzzle<3, 3> slide3_t;

void printPath(forward_list<slide3_t> path)
{
  if (path.empty()) {
    cout << "NO PATHS" << endl;
    return;
  }

  cout << *path.begin();
  for (auto iter = ++path.begin(); iter != path.end(); ++iter) {
    
    cout << '\n' << setw(8) << "↓" << '\n' << endl;;
    cout << *iter;
  }
  cout << endl;
}

int main() 
{
  int start1[9] = {2,7,5,6,-1,4,3,8,1};
  int start2[9] = {2,1,3,4,5,6,7,8,-1};
  int finish[9] = {1,2,3,4,5,6,7,8,-1};

  slide3_t puzzle1{start1};
  slide3_t puzzle2{start2};
  slide3_t completed{finish};

  printPath(puzzle1.solvePuzzle(completed));
  printPath(puzzle2.solvePuzzle(completed));
}