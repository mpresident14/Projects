#include "graph.hpp"
#include "../testing_program/testing-program.hpp"
#include <string>
#include <fstream>
#include <vector>
#include <locale>

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

void printPath(forward_list<string> path)
{
  if (path.begin() == path.end()) {
    cout << "NO PATHS" << endl;
    return;
  }

  cout << *path.begin();
  for (auto iter = ++path.begin(); iter != path.end(); ++iter) {
    cout << " -> " << *iter;
  }
  cout << endl;
}

int main(int argc, char** args)
{
  if (argc != 3) {
    printf("Enter 2 words.");
    return 1;
  }

  string first{args[1]};
  string second{args[2]};

  if (first.size() != second.size()) {
    printf("The 2 words must be the same length.");
    return 1;
  }

  // Convert to uppercase (b/c dictionary has uppercase letters)
  locale loc;
  for (size_t i = 0; i < first.size(); ++i) {
    first[i] = toupper(first[i], loc);
    second[i] = toupper(second[i], loc);
  }

  // Add words to dictionary
  Graph<string, oneLetterDifferent> g;

  ifstream infile("dictionary.txt");
  string word;

  while(getline(infile, word)) {
    // word has an extra '\r' character (carriage return)
    if (word.size() == first.size() + 1) {
      g.addVertex(word.substr(0, word.size() - 1));
    }  
  }

  // Print the shortest path
  try {
    printPath(g.getShortestPath(first, second));
  }
  catch (invalid_argument ex) {
    cerr << ex.what() << endl;
  }
  
  return 0;
}