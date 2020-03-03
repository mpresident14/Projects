CXX = clang++
CXXV=17
CXXFLAGS = -std=c++$(CXXV) -Wall -Wextra -pedantic -g
HEADERS = \
	Parsers.hpp \
	Parser.hpp \
	ConditionalParser.hpp \
	MapParser.hpp \
	AltParser.hpp \
	CharParser.hpp \
	LazyParser.hpp \
	SequenceParser.hpp \
	ManyParser.hpp \
	IgnoreParser.hpp \
	StringParser.hpp

all: test

clean:
	rm main || true
	rm test

main: main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) main.cpp -o main

test: ParserTest.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) ParserTest.cpp -o test
