CXX = g++
CXXFLAGS = -g -std=c++1z -Wall -Wextra -pedantic -O2

all: main

clean: 
	rm -rf *.o
	rm -rf *.exe

main: main.cpp graph.hpp graph_private.hpp
	$(CXX) $(CXXFLAGS) -o main main.cpp ../testing_program/testing-program.cpp