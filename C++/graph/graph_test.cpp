#include "graph.hpp"
#include "../testing_program/testing-program.hpp"
#include <string>
#include <unordered_set>
#include <stdexcept>

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

// Because template is lazily instantiated, but we want to explicitly test everything
template class Graph<string, oneLetterDifferent>;

void test_oneLetterDifferent()
{
  TestingProgram tester{"One Letter Different"};

  affirm(oneLetterDifferent("make", "mate"));
  affirm(!oneLetterDifferent("mates", "mate"));
  affirm(!oneLetterDifferent("meat", "mate"));
}

void test_defaultConstructor()
{
  TestingProgram tester{"Default Constructor"};

  Graph<string, oneLetterDifferent> g;
  affirm(g.numVertices() == 0);

  TestingProgram::printResults();
}

void test_copyConstructor()
{
  TestingProgram tester{"Copy Constructor"};

  Graph<string, oneLetterDifferent> g;
  g.addVertex("day");
  Graph<string, oneLetterDifferent> h{g};
  h.addVertex("bay");

  affirm(g.getRelatives("bay") == nullptr);
  affirm(*(h.getRelatives("bay")) == unordered_set<string>( {"day"} ));

  TestingProgram::printResults();
}

void test_moveConstructor()
{
  TestingProgram tester{"Move Constructor"};

  Graph<string, oneLetterDifferent> g;
  g.addVertex("day");
  Graph<string, oneLetterDifferent> h{std::move(g)};
  h.addVertex("bay");

  affirm(*(h.getRelatives("bay")) == unordered_set<string>( {"day"} ));

  TestingProgram::printResults();
}

void test_assignmentOperator()
{
  TestingProgram tester{"Assignment Operator"};

  Graph<string, oneLetterDifferent> g;
  g.addVertex("day");
  Graph<string, oneLetterDifferent> h;
  h = g;
  h.addVertex("bay");

  affirm(g.getRelatives("bay") == nullptr);
  affirm(*(h.getRelatives("bay")) == unordered_set<string>( {"day"} ));

  TestingProgram::printResults();
}

void test_addVertex_lvalue()
{
  TestingProgram tester{"Add Vertex LValue"};

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
  affirm(g.numVertices() == 4);
  affirm(*(g.getRelatives(str1)) == unordered_set<string>({str2}));
  affirm(*(g.getRelatives(str2)) == unordered_set<string>({str1, str3}));
  affirm(*(g.getRelatives(str3)) == unordered_set<string>({str2}));
  affirm(*(g.getRelatives(str4)) == unordered_set<string>());

  TestingProgram::printResults();
}

void test_addVertex_rvalue()
{
  TestingProgram tester{"Add Vertex LValue"};

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
  affirm(g.numVertices() == 4);
  affirm(*(g.getRelatives(str1)) == unordered_set<string>({str2}));
  affirm(*(g.getRelatives(str2)) == unordered_set<string>({str1, str3}));
  affirm(*(g.getRelatives(str3)) == unordered_set<string>({str2}));
  affirm(*(g.getRelatives(str4)) == unordered_set<string>());

  TestingProgram::printResults();
}

void test_addVertex_initList_lValue_works()
{
  TestingProgram tester{"Add Vertex init list LValue works"};

  Graph<string, oneLetterDifferent> g;

  string str1 = "hello";
  string str2 = "bye";
  string str3 = "I'm back";
  string str4 = "cool";
  g.addVertex(str1, {});
  g.addVertex(str2, {str1});
  g.addVertex(str3, {str1, str2});
  g.addVertex(str4, {str1});


  affirm(g.numVertices() == 4);
  affirm(*(g.getRelatives(str1)) == unordered_set<string>({str2, str3, str4}));
  affirm(*(g.getRelatives(str2)) == unordered_set<string>({str1, str3}));
  affirm(*(g.getRelatives(str3)) == unordered_set<string>({str1, str2}));
  affirm(*(g.getRelatives(str4)) == unordered_set<string>({str1}));

  TestingProgram::printResults();
}

void test_addVertex_initList_lValue_withIllegalArgs()
{
  TestingProgram tester{"Add Vertex init list LValue with illegal args"};

  Graph<string, oneLetterDifferent> g;

  string str1 = "hello";

  bool threwError = false;
  try {
    g.addVertex(str1, {"Not in graph"});
  }
  catch (const invalid_argument&) {
    threwError = true;
  }
  
  affirm(threwError == true);

  TestingProgram::printResults();
}

void test_addVertex_initList_rValue_works()
{
  TestingProgram tester{"Add Vertex init list RValue works"};

  Graph<string, oneLetterDifferent> g;

  g.addVertex("hello", {});
  g.addVertex("bye", {"hello"});
  g.addVertex("I'm back", {"hello", "bye"});
  g.addVertex("cool", {"hello"});

  affirm(g.numVertices() == 4);
  affirm(*(g.getRelatives("hello")) == unordered_set<string>({"bye", "I'm back", "cool"}));
  affirm(*(g.getRelatives("bye")) == unordered_set<string>({"hello", "I'm back"}));
  affirm(*(g.getRelatives("I'm back")) == unordered_set<string>({"hello", "bye"}));
  affirm(*(g.getRelatives("cool")) == unordered_set<string>({"hello"}));

  TestingProgram::printResults();
}

void test_addVertex_initList_rValue_withIllegalArgs()
{
  TestingProgram tester{"Add Vertex init list RValue with illegal args"};

  Graph<string, oneLetterDifferent> g;

  bool threwError = false;
  try {
    g.addVertex("hello", {"Not in graph"});
  }
  catch (const invalid_argument&) {
    threwError = true;
  }
  
  affirm(threwError == true);

  TestingProgram::printResults();
}

void test_removeVertex()
{
  TestingProgram tester{"removeVertex"};

  Graph<string, oneLetterDifferent> g;

  string str1 = "make";
  string str2 = "mate";
  string str3 = "mane";
  g.addVertex(str1);
  g.addVertex(str2);
  g.addVertex(str3);

  affirm(g.removeVertex(str1));
  affirm(!g.removeVertex(str1));

  affirm(g.numVertices() == 2);
  affirm(*(g.getRelatives(str2)) == unordered_set<string>({str3}));
  affirm(*(g.getRelatives(str3)) == unordered_set<string>({str2}));

  TestingProgram::printResults();
}

void test_getRelatives()
{
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

  affirm(g.numVertices() == 4);
  affirm(*(g.getRelatives(str1)) == unordered_set<string>( {str2, str4} ));
  affirm(*(g.getRelatives(str2)) == unordered_set<string>( {str1, str3, str4} ));
  affirm(*(g.getRelatives(str3)) == unordered_set<string>( {str2} ));
  affirm(*(g.getRelatives(str4)) == unordered_set<string>( {str1, str2} ));
  affirm(g.getRelatives("absent") == nullptr);

  TestingProgram::printResults();
}

void test_getRelativeCount()
{
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

void test_contains()
{
  TestingProgram tester{"Contains"};

  Graph<string, oneLetterDifferent> g;

  string str1 = "hello";
  g.addVertex(str1);

  affirm(g.contains(str1));
  affirm(!g.contains("bye"));

  TestingProgram::printResults();
}

void test_getPath()
{
  TestingProgram tester{"Get Path"};

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

  affirm( g.getShortestPath("day", "cut") == forward_list<string>( {"day", "bay", "buy", "but", "cut"} ));
  affirm( g.getShortestPath("cut", "gut") == forward_list<string>( {"cut", "gut"} ));
  affirm( g.getShortestPath("but", "but") == forward_list<string>( {"but"} ));

  TestingProgram::printResults();
}

int main()
{
  test_oneLetterDifferent();
  test_defaultConstructor();
  test_copyConstructor();
  test_moveConstructor();
  test_assignmentOperator();
  test_addVertex_lvalue();
  test_addVertex_rvalue();
  test_addVertex_initList_lValue_works();
  test_addVertex_initList_lValue_withIllegalArgs();
  test_addVertex_initList_rValue_works();
  test_addVertex_initList_rValue_withIllegalArgs();
  test_removeVertex();
  test_getRelatives();
  test_getRelativeCount();
  test_contains();
  test_getPath();

  TestingProgram::summarize();

  return 0;
}