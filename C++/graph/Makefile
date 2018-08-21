CXX = g++
CXXFLAGS = -g -std=c++1z -Wall -Wextra -pedantic -O2

all: main

clean: 
	rm -rf *.o
	rm -rf *.exe

testing-program.o: ../testing_program/testing-program.hpp ../testing_program/testing-program.cpp
	$(CXX) $(CXXFLAGS) -c ../testing_program/testing-program.cpp 

main.o: main.cpp graph.hpp graph_private.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

main: main.o testing-program.o
	$(CXX) $(CXXFLAGS) -o main main.o testing-program.o