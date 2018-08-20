#include "graph.hpp"
#include "../testing_program/testing-program.hpp"
#include <string>

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
  Graph<string> h{g};

  TestingProgram::printResults();
}

void test_moveConstructor()
{
  TestingProgram tester{"Move Constructor"};

  Graph<string> g;
  Graph<string> h{std::move(g)};

  TestingProgram::printResults();
}

void test_assignmentOperator()
{
  TestingProgram tester{"Assignment Operator"};

  Graph<string> g;
  Graph<string> h = g;

  TestingProgram::printResults();
}

void test_addVertex_lValue_works()
{
  TestingProgram tester{"Add Vertex LValue works"};

  Graph<string> g;

  string str1 = "hello";
  string str2 = "bye";
  string str3 = "I'm back";
  g.addVertex(str1, {});
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
  g.addVertex(str1, {"Not in graph"});

  affirm(g.numVertices() == 0);

  TestingProgram::printResults();
}

void test_addVertex_rValue_works()
{
  TestingProgram tester{"Add Vertex RValue works"};

  Graph<string> g;

  g.addVertex("hello", {});
  g.addVertex("bye", {"hello"});
  g.addVertex("I'm back", {"hello", "bye"});

  affirm(g.numVertices() == 3);

  TestingProgram::printResults();
}

void test_addVertex_rValue_withIllegalArgs()
{
  TestingProgram tester{"Add Vertex RValue with illegal args"};

  Graph<string> g;
  
  g.addVertex("hello", {"hi"});

  affirm(g.numVertices() == 0);

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

  TestingProgram::summarize();
  return 0;
}