CXX = g++
CXXFLAGS = -g -std=c++1z -Wall -Wextra -pedantic -O2

all: graph_test letter_change solver

test: graph_test
	./graph_test

letter: letter_change

solve: solver
	./solver

clean: 
	rm -rf *.o
	rm -rf *.exe

testing-program.o: ../testing_program/testing-program.hpp ../testing_program/testing-program.cpp
	$(CXX) $(CXXFLAGS) -c ../testing_program/testing-program.cpp 

graph_test.o: graph_test.cpp graph.hpp graph_private.hpp
	$(CXX) $(CXXFLAGS) -c graph_test.cpp

letter_change.o: letter_change.cpp graph.hpp graph_private.hpp
	$(CXX) $(CXXFLAGS) -c letter_change.cpp

solver.o: solver.cpp slide_puzzle.hpp slide_puzzle_private.hpp graph.hpp graph_private.hpp
	$(CXX) $(CXXFLAGS) -c solver.cpp

graph_test: graph_test.o testing-program.o
	$(CXX) $(CXXFLAGS) -o graph_test $^

letter_change: letter_change.o testing-program.o
	$(CXX) $(CXXFLAGS) -o letter_change $^

solver: solver.o slide_puzzle.o
	$(CXX) $(CXXFLAGS) -o solver $^	