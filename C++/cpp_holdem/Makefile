# Use "nmake"

CXX = g++
CXXFLAGS = -g -pedantic -O2 -Wall

all: win_percentages

clean: 
	rm -rf *.o
	rm -rf *.exe

card.o: card.cpp card.hpp
	$(CXX) $(CXXFLAGS) -c card.cpp

player.o: player.cpp player.hpp card.hpp
	$(CXX) $(CXXFLAGS) -c player.cpp

hand_percentages.o: hand_percentages.cpp hand_percentages.hpp player.hpp card.hpp
	$(CXX) $(CXXFLAGS) -c hand_percentages.cpp

win_percentages.o: win_percentages.cpp win_percentages.hpp player.hpp card.hpp
	$(CXX) $(CXXFLAGS) -c win_percentages.cpp

hand_percentages: hand_percentages.o player.o card.o
	$(CXX) hand_percentages.o player.o card.o -o hand_percentages

win_percentages: win_percentages.o player.o card.o
	$(CXX) win_percentages.o player.o card.o -o win_percentages
