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
  g.addVertex(str1);
  g.addVertex(str2, {str1});
  g.addVertex(str3, {str1, str2});
  affirm(g.numVertices() == 3);

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
  affirm(g.numVertices() == 3);

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
  test_contains();

  TestingProgram::summarize();
  return 0;
}