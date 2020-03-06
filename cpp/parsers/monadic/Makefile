SHELL=/bin/bash

CXX = clang++
CXXV=17
CXXFLAGS = -std=c++$(CXXV) -Wall -Wextra -pedantic -g

all: widget expr regex main

clean:
	find . -type f -executable -delete

widget: widget.cpp widget.hpp parser.hpp
	$(CXX) $(CXXFLAGS) widget.cpp -o widget

expr: expr.cpp expr.hpp parser.hpp
	$(CXX) $(CXXFLAGS) expr.cpp -o expr

regex: regex.cpp regex.hpp parser.hpp
	$(CXX) $(CXXFLAGS) regex.cpp -o regex

main: main.cpp parser.hpp
	$(CXX) $(CXXFLAGS) main.cpp -o main
