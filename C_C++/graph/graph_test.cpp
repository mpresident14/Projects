#include "graph.hpp"
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "../testing_program/testing-program.hpp"

using namespace std;

bool oneLetterDifferent(const string& s, const string& t) {
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

bool hasNoneOf(const string& s, char c) {
  for (auto& letter : s) {
    if (letter == c) {
      return false;
    }
  }
  return true;
}

// Because template is lazily instantiated, but we want to explicitly test
// everything
template class Graph<string, oneLetterDifferent>;

void test_oneLetterDifferent() {
  TestingProgram tester{"One Letter Different"};

  affirm(oneLetterDifferent("make", "mate"));
  affirm(!oneLetterDifferent("mates", "mate"));
  affirm(!oneLetterDifferent("meat", "mate"));
}

void test_defaultConstructor() {
  TestingProgram tester{"Default Constructor"};

  Graph<string, oneLetterDifferent> g;
  affirm(g.size() == 0);

  TestingProgram::printResults();
}

void test_copyConstructor() {
  TestingProgram tester{"Copy Constructor"};

  Graph<string, oneLetterDifferent> g;
  g.addVertex("day");
  Graph<string, oneLetterDifferent> h{g};
  h.addVertex("bay");

  affirm(g.getRelatives("bay") == nullptr);
  affirm(*(h.getRelatives("bay")) == unordered_set<string>({"day"}));

  TestingProgram::printResults();
}

void test_moveConstructor() {
  TestingProgram tester{"Move Constructor"};

  Graph<string, oneLetterDifferent> g;
  g.addVertex("day");
  Graph<string, oneLetterDifferent> h{std::move(g)};
  h.addVertex("bay");

  affirm(*(h.getRelatives("bay")) == unordered_set<string>({"day"}));

  TestingProgram::printResults();
}

void test_assignmentOperator() {
  TestingProgram tester{"Assignment Operator"};

  Graph<string, oneLetterDifferent> g;
  g.addVertex("day");
  Graph<string, oneLetterDifferent> h;
  h = g;
  h.addVertex("bay");

  affirm(g.getRelatives("bay") == nullptr);
  affirm(*(h.getRelatives("bay")) == unordered_set<string>({"day"}));

  TestingProgram::printResults();
}

void test_addVertex_lvalue() {
  TestingProgram tester{"Add Vertex lValue"};

  string str1 = "make";
  string str2 = "mate";
  string str3 = "mite";
  string str4 = "cool";
  Graph<string, oneLetterDifferent> g;

  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);
  g.addVertex(str4);

  affirm(!g.addVertex(str1));
  affirm(g.size() == 4);
  affirm(*(g.getRelatives(str1)) == unordered_set<string>({str2}));
  affirm(*(g.getRelatives(str2)) == unordered_set<string>({str1, str3}));
  affirm(*(g.getRelatives(str3)) == unordered_set<string>({str2}));
  affirm(*(g.getRelatives(str4)) == unordered_set<string>());

  TestingProgram::printResults();
}

void test_addVertex_lvalue_noAdjFcn() {
  TestingProgram tester{"Add Vertex lValue no adj fcn"};

  string str1 = "make";
  string str2 = "mate";
  string str3 = "mite";
  string str4 = "cool";
  Graph<string> g;

  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);
  g.addVertex(str4);

  affirm(!g.addVertex(str1));
  affirm(g.size() == 4);
  affirm(*(g.getRelatives(str1)) == unordered_set<string>());
  affirm(*(g.getRelatives(str2)) == unordered_set<string>());
  affirm(*(g.getRelatives(str3)) == unordered_set<string>());
  affirm(*(g.getRelatives(str4)) == unordered_set<string>());

  TestingProgram::printResults();
}

void test_addVertex_rvalue() {
  TestingProgram tester{"Add Vertex rValue"};

  string str1 = "make";
  string str2 = "mate";
  string str3 = "mite";
  string str4 = "cool";
  Graph<string, oneLetterDifferent> g;

  g.addVertex("make");
  g.addVertex("mate");
  g.addVertex("mite");
  g.addVertex("cool");

  affirm(!g.addVertex("make"));
  affirm(g.size() == 4);
  affirm(*(g.getRelatives(str1)) == unordered_set<string>({str2}));
  affirm(*(g.getRelatives(str2)) == unordered_set<string>({str1, str3}));
  affirm(*(g.getRelatives(str3)) == unordered_set<string>({str2}));
  affirm(*(g.getRelatives(str4)) == unordered_set<string>());

  TestingProgram::printResults();
}

void test_addVertex_rvalue_noAdjFcn() {
  TestingProgram tester{"Add Vertex rValue no adj fcn"};

  Graph<string> g;

  g.addVertex("a");
  g.addVertex("b");
  g.addVertex("c");
  g.addVertex("d");

  affirm(!g.addVertex("a"));
  affirm(g.size() == 4);
  affirm(*(g.getRelatives("a")) == unordered_set<string>());
  affirm(*(g.getRelatives("b")) == unordered_set<string>());
  affirm(*(g.getRelatives("c")) == unordered_set<string>());
  affirm(*(g.getRelatives("d")) == unordered_set<string>());

  TestingProgram::printResults();
}

void test_addEdge() {
  TestingProgram tester{"Add Edge"};

  Graph<string> g;

  string str1 = "make";
  string str2 = "mate";
  string str3 = "late";
  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);

  affirm(g.addEdge(str1, str2));
  affirm(g.addEdge(str2, str3));
  affirm(!g.addEdge(str2, str1));
  affirm(!g.addEdge(str2, str3));
  affirm(!g.addEdge("absent", str1));

  affirm(*(g.getRelatives(str1)) == unordered_set<string>({str2}));
  affirm(*(g.getRelatives(str2)) == unordered_set<string>({str1, str3}));
  affirm(*(g.getRelatives(str3)) == unordered_set<string>({str2}));

  TestingProgram::printResults();
}

void test_removeVertex() {
  TestingProgram tester{"Remove Vertex"};

  Graph<string, oneLetterDifferent> g;

  string str1 = "make";
  string str2 = "mate";
  string str3 = "mane";
  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);

  affirm(g.removeVertex(str1));
  affirm(!g.removeVertex(str1));

  affirm(g.size() == 2);
  affirm(*(g.getRelatives(str2)) == unordered_set<string>({str3}));
  affirm(*(g.getRelatives(str3)) == unordered_set<string>({str2}));

  TestingProgram::printResults();
}

void test_removeEdge() {
  TestingProgram tester{"Remove Edge"};

  Graph<string, oneLetterDifferent> g;

  string str1 = "make";
  string str2 = "mate";
  string str3 = "late";
  string str4 = "lake";
  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);
  g.addVertex(str4);

  affirm(g.removeEdge(str2, str3));
  affirm(g.removeEdge(str1, str4));
  affirm(!g.removeEdge(str1, str4));
  affirm(!g.removeEdge("absent", str1));

  affirm(*(g.getRelatives(str1)) == unordered_set<string>({str2}));
  affirm(*(g.getRelatives(str2)) == unordered_set<string>({str1}));
  affirm(*(g.getRelatives(str3)) == unordered_set<string>({str4}));
  affirm(*(g.getRelatives(str4)) == unordered_set<string>({str3}));

  TestingProgram::printResults();
}

void test_getRelatives() {
  TestingProgram tester{"Get Relatives"};

  Graph<string, oneLetterDifferent> g;

  string str1 = "white";
  string str2 = "while";
  string str3 = "whale";
  string str4 = "whine";
  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);
  g.addVertex(str4);

  affirm(g.size() == 4);
  affirm(*(g.getRelatives(str1)) == unordered_set<string>({str2, str4}));
  affirm(*(g.getRelatives(str2)) == unordered_set<string>({str1, str3, str4}));
  affirm(*(g.getRelatives(str3)) == unordered_set<string>({str2}));
  affirm(*(g.getRelatives(str4)) == unordered_set<string>({str1, str2}));
  affirm(g.getRelatives("absent") == nullptr);

  TestingProgram::printResults();
}

void test_getRelativeCount() {
  TestingProgram tester{"Get Relative Count"};

  Graph<string, oneLetterDifferent> g;

  string str1 = "white";
  string str2 = "while";
  string str3 = "whale";
  string str4 = "whine";
  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);
  g.addVertex(str4);

  affirm(g.getRelativeCount(str1) == 2);
  affirm(g.getRelativeCount(str2) == 3);
  affirm(g.getRelativeCount(str3) == 1);
  affirm(g.getRelativeCount(str4) == 2);
  affirm(g.getRelativeCount("Absent") == -1);

  TestingProgram::printResults();
}

void test_adajacent() {
  TestingProgram tester{"Adjacent"};

  Graph<int> g;

  int a = 5;
  int b = 50;
  int c = 77;

  g.addVertex(a);
  g.addVertex(b);
  g.addVertex(c);
  g.addEdge(a, b);

  affirm(g.adjacent(a, b));
  affirm(!g.adjacent(c, b));
  affirm(g.removeEdge(a, b));
  affirm(!g.adjacent(a, b));

  TestingProgram::printResults();
}

void test_contains() {
  TestingProgram tester{"Contains"};

  Graph<string, oneLetterDifferent> g;

  string str1 = "hello";
  g.addVertex(str1);

  affirm(g.contains(str1));
  affirm(!g.contains("bye"));

  TestingProgram::printResults();
}

void test_getShortestPath() {
  TestingProgram tester{"Get Shortest Path"};

  Graph<string, oneLetterDifferent> g;

  string str1 = "bay";
  string str2 = "day";
  string str3 = "buy";
  string str4 = "cut";
  string str5 = "but";
  string str6 = "gut";

  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);
  g.addVertex(str4);
  g.addVertex(str5);
  g.addVertex(str6);

  affirm(g.getShortestPath("day", "cut") ==
         forward_list<string>({"day", "bay", "buy", "but", "cut"}));
  affirm(g.getShortestPath("cut", "gut") ==
         forward_list<string>({"cut", "gut"}));
  affirm(g.getShortestPath("but", "but") == forward_list<string>({"but"}));

  TestingProgram::printResults();
}

void test_getShortestPath_withCondition() {
  TestingProgram tester{"Get Shortest Path with condition"};

  Graph<string, oneLetterDifferent> g;

  string str1 = "bard";
  string str2 = "hard";
  string str3 = "hare";
  string str4 = "care";
  string str5 = "cane";
  string str6 = "cans";
  string str7 = "bars";
  string str8 = "bans";

  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);
  g.addVertex(str4);
  g.addVertex(str5);
  g.addVertex(str6);
  g.addVertex(str7);
  g.addVertex(str8);

  affirm(g.getShortestPath("hare", "cans") ==
         forward_list<string>({"hare", "care", "cane", "cans"}));
  affirm(
      g.getShortestPath("hare", "cans", hasNoneOf, 'e') ==
      forward_list<string>({"hare", "hard", "bard", "bars", "bans", "cans"}));

  TestingProgram::printResults();
}

void test_size() {
  TestingProgram tester{"Size"};

  Graph<string, oneLetterDifferent> g;

  string str1 = "bay";
  string str2 = "day";
  string str3 = "buy";
  string str4 = "cut";
  string str5 = "but";
  string str6 = "gut";

  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);
  g.addVertex(str4);
  g.addVertex(str5);
  g.addVertex(str6);

  TestingProgram::printResults();
}

void test_numEdges() {
  TestingProgram tester{"Num Edges"};

  Graph<int> g;

  int a = 5;
  int b = 50;
  int c = 500;
  int d = 5000;

  g.addVertex(a);
  g.addVertex(b);
  g.addVertex(c);
  g.addVertex(d);

  affirm(g.numEdges() == 0);
  g.addEdge(a, b);
  affirm(g.numEdges() == 1);
  g.addEdge(b, c);
  g.addEdge(b, c);  // repeat
  affirm(g.numEdges() == 2);
  g.addEdge(c, d);
  affirm(g.numEdges() == 3);
  g.addEdge(d, a);
  affirm(g.numEdges() == 4);

  g.removeEdge(a, b);
  affirm(g.numEdges() == 3);

  TestingProgram::printResults();
}

void test_begin() {
  TestingProgram tester{"Begin"};

  Graph<string> g;
  affirm(g.begin() == g.end());

  g.addVertex("hi");
  affirm(g.begin()->first == "hi");

  TestingProgram::printResults();
}

void test_end() {
  TestingProgram tester{"End"};

  Graph<string> g;
  affirm(g.begin() == g.end());

  g.addVertex("hi");
  affirm(++g.begin() == g.end());

  TestingProgram::printResults();
}

/******************
 * Iterator Tests *
 ******************/
void test_IterIncrement() {
  TestingProgram tester{"Iterator ++"};

  Graph<string, oneLetterDifferent> g;

  string str1 = "bay";
  string str2 = "day";
  string str3 = "buy";
  string str4 = "cut";

  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);
  g.addVertex(str4);

  size_t increments = 0;
  for (auto iter = g.begin(); iter != g.end(); ++iter) {
    ++increments;
  }

  affirm(increments == g.size());

  TestingProgram::printResults();
}

void test_IterDereference() {
  TestingProgram tester{"Iterator *"};

  Graph<string, oneLetterDifferent> g;

  string str1 = "bay";
  string str2 = "day";
  string str3 = "buy";
  string str4 = "cut";
  string str5 = "but";
  string str6 = "gut";

  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);
  g.addVertex(str4);
  g.addVertex(str5);
  g.addVertex(str6);

  unordered_set<string> traversed;
  for (auto iter = g.begin(); iter != g.end(); ++iter) {
    traversed.insert(iter->first);
    affirm(iter->second == *g.getRelatives(iter->first));
  }
  affirm(traversed ==
         unordered_set<string>({"bay", "day", "buy", "cut", "but", "gut"}));

  TestingProgram::printResults();
}

void test_IterEquals() {
  TestingProgram tester{"Iterator =="};

  Graph<string, oneLetterDifferent> g;

  string str1 = "bay";
  string str2 = "day";
  string str3 = "buy";

  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);

  auto iter1 = g.begin();
  auto iter2 = g.begin();

  affirm(iter1 == iter2);
  affirm(++iter1 == ++iter2);
  affirm(g.end() == g.end());

  TestingProgram::printResults();
}

void test_IterNotEquals() {
  TestingProgram tester{"Iterator !="};

  Graph<string, oneLetterDifferent> g;

  string str1 = "bay";
  string str2 = "day";
  string str3 = "buy";

  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);

  auto iter1 = g.begin();
  auto iter2 = ++g.begin();

  affirm(iter1 != iter2);
  affirm(++iter1 != ++iter2);
  affirm(iter1 != g.end());

  TestingProgram::printResults();
}

int main() {
  test_oneLetterDifferent();
  test_defaultConstructor();
  test_copyConstructor();
  test_moveConstructor();
  test_assignmentOperator();
  test_addVertex_lvalue();
  test_addVertex_lvalue_noAdjFcn();
  test_addVertex_rvalue();
  test_addVertex_rvalue_noAdjFcn();
  test_addEdge();
  test_removeEdge();
  test_removeVertex();
  test_getRelatives();
  test_getRelativeCount();
  test_adajacent();
  test_contains();
  test_getShortestPath();
  test_getShortestPath_withCondition();
  test_size();
  test_numEdges();
  test_begin();
  test_end();
  test_IterIncrement();
  test_IterDereference();
  test_IterEquals();
  test_IterNotEquals();

  TestingProgram::summarize();

  return 0;
}