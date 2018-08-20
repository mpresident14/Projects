#include "graph.hpp"
#include "../testing_program/testing-program.hpp"
#include <string>
#include <unordered_set>
#include <stdexcept>

using namespace std;

// Because template is lazily instantiated, but we want to explicitly test everything
template class Graph<string>;

void test_defaultConstructor()
{
  TestingProgram tester{"Default Constructor"};

  Graph<string> g;
  affirm(g.numVertices() == 0);

  TestingProgram::printResults();
}

void test_copyConstructor()
{
  TestingProgram tester{"Copy Constructor"};

  Graph<string> g;
  g.addVertex("hello");
  Graph<string> h{g};
  h.addVertex("bye", {"hello"});

  affirm(g.getRelatives("bye") == nullptr);

  TestingProgram::printResults();
}

void test_moveConstructor()
{
  TestingProgram tester{"Move Constructor"};

  Graph<string> g;
  g.addVertex("hello");
  Graph<string> h{std::move(g)};
  affirm(*(h.getRelatives("hello")) == unordered_set<string>());

  TestingProgram::printResults();
}

void test_assignmentOperator()
{
  TestingProgram tester{"Assignment Operator"};

  Graph<string> g;
  g.addVertex("hello");
  Graph<string> h;
  h = g;
  h.addVertex("bye", {"hello"});

  affirm(g.getRelatives("bye") == nullptr);

  TestingProgram::printResults();
}

void test_addVertex_lValue_works()
{
  TestingProgram tester{"Add Vertex LValue works"};

  Graph<string> g;

  string str1 = "hello";
  string str2 = "bye";
  string str3 = "I'm back";
  string str4 = "cool";
  g.addVertex(str1);
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

void test_addVertex_lValue_withIllegalArgs()
{
  TestingProgram tester{"Add Vertex LValue with illegal args"};

  Graph<string> g;

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

void test_addVertex_rValue_works()
{
  TestingProgram tester{"Add Vertex RValue works"};

  Graph<string> g;

  g.addVertex("hello");
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

void test_addVertex_rValue_withIllegalArgs()
{
  TestingProgram tester{"Add Vertex RValue with illegal args"};

  Graph<string> g;

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

  Graph<string> g;

  string str1 = "hello";
  g.addVertex(str1);
  affirm(g.removeVertex(str1));
  affirm(!g.removeVertex(str1));

  TestingProgram::printResults();
}

void test_getRelatives_withPresent()
{
  TestingProgram tester{"Get Relatives with present"};

  Graph<string> g;

  string str1 = "hello";
  string str2 = "bye";
  string str3 = "I'm back";
  g.addVertex(str1);
  g.addVertex(str2, {str1});
  g.addVertex(str3, {str1, str2});

  affirm(*(g.getRelatives(str3)) == unordered_set<string>({str1, str2}));

  TestingProgram::printResults();
}

void test_getRelatives_withAbsent()
{
  TestingProgram tester{"Get Relatives with absent"};

  Graph<string> g;

  string str1 = "hello";
  string str2 = "bye";
  string str3 = "I'm back";
  g.addVertex(str1);
  g.addVertex(str2, {str1});
  g.addVertex(str3, {str1, str2});

  affirm(g.getRelatives("absent") == nullptr);

  TestingProgram::printResults();
}

void test_getRelativeCount()
{
  TestingProgram tester{"Get Relative Count"};

  Graph<string> g;

  string str1 = "hello";
  string str2 = "bye";
  string str3 = "I'm back";
  g.addVertex(str1);
  g.addVertex(str2, {str1});
  g.addVertex(str3, {str1, str2});

  affirm(g.getRelativeCount(str3) == 2);
  affirm(g.getRelativeCount("Absent") == -1);

  TestingProgram::printResults();
}

void test_contains()
{
  TestingProgram tester{"Contains"};

  Graph<string> g;

  string str1 = "hello";
  g.addVertex(str1);

  affirm(g.contains(str1));
  affirm(!g.contains("bye"));

  TestingProgram::printResults();
}

void test_getPath()
{
  TestingProgram tester{"Get Path"};

  Graph<string> g;

  string str1 = "1";
  string str2 = "2";
  string str3 = "3";
  string str4 = "4";
  string str5 = "5";
  string str6 = "6";
  
  g.addVertex(str1);
  g.addVertex(str2, {str1});
  g.addVertex(str3, {str1, str2});
  g.addVertex(str4, {str3});
  g.addVertex(str5, {str4});
  g.addVertex(str6, {str2});

  affirm(g.getPath(str1, str5) == 3);

  // --------------------------------------

  Graph<string> h;
  h.addVertex(str3);
  h.addVertex(str2, {str3});
  h.addVertex(str1, {str2});
  h.addVertex(str4, {str3, str1});

  affirm(h.getPath(str4, str1) == 1);

  TestingProgram::printResults();
}

int main()
{
  test_defaultConstructor();
  test_copyConstructor();
  test_moveConstructor();
  test_assignmentOperator();
  test_addVertex_lValue_works();
  test_addVertex_lValue_withIllegalArgs();
  test_addVertex_rValue_works();
  test_addVertex_rValue_withIllegalArgs();
  test_removeVertex();
  test_getRelatives_withPresent();
  test_getRelatives_withAbsent();
  test_getRelativeCount();
  test_contains();
  test_getPath();

  TestingProgram::summarize();
  return 0;
}