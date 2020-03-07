SHELL = /bin/bash
PHEADERS = parser.hpp parser_impl.hpp
CXX = clang++
CXXV=17
CXXFLAGS = -std=c++$(CXXV) -Wall -Wextra -pedantic -g

all: widget expr regex main

clean:
	find . -type f -executable -delete

widget: widget.cpp widget.hpp $(PHEADERS)
	$(CXX) $(CXXFLAGS) widget.cpp -o widget

expr: expr.cpp expr.hpp $(PHEADERS)
	$(CXX) $(CXXFLAGS) expr.cpp -o expr

regex: regex.cpp regex.hpp $(PHEADERS)
	$(CXX) $(CXXFLAGS) regex.cpp -o regex

main: main.cpp $(PHEADERS)
	$(CXX) $(CXXFLAGS) main.cpp -o main
