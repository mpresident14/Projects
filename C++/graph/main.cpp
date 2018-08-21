#include "graph.hpp"
#include "../testing_program/testing-program.hpp"
#include <string>
#include <fstream>
#include <vector>

using namespace std;

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
  ifstream infile("4_letter_dictionary.txt");

  Graph<string, oneLetterDifferent> g;

  string word;
  while(getline(infile, word)) {
    string wordWithoutCarriageReturn = word.substr(0, word.size() - 1);
    g.addVertex(wordWithoutCarriageReturn);
  }

  cout << g.numVertices() << endl;
  forward_list<string> path = g.getShortestPath("RANK", "NINE");

  for (const string& x : path) {
    cout << x << " -> ";
  }
  cout << endl;
  return 0;
}