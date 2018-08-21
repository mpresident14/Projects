#include "graph.hpp"
#include "../testing_program/testing-program.hpp"
#include <string>

bool oneLetterDifferent(const string& s, const string& t)
{
  size_t size = s.size();
  if (size != t.size()) {
    return false;
  }

  bool foundDifferent = false;
  for (size_t i = 0; i < size; ++i) {
    if (s.at(i) != t.at(i)) {
      if (foundDifferent) {
        return false;
      }
      foundDifferent = true;
    }
  }

  return foundDifferent;
}

int main()
{
  Graph<string, oneLetterDifferent> g;
  string arr[9] = {"rank", "mink", "mine", "mice", "tank", "nice", "nine", "mint", "rink"};
  for (string& str: arr) {
    g.addVertex(str);
  }

  for (auto& x : g.getShortestPath("rank", "nine")) {
    cout << x << " -> ";
  }
  cout << endl;
  return 0;
}