CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

all: example

clean:
	rm example

example: example.cpp unit_test.hpp
	$(CXX) $(CXXFLAGS) example.cpp -o example

