CXX = clang++
CXX_ARGS = -g -std=c++17 -pedantic -Wall -Wextra

all: main

clean:
	rm -rf *.o main

asciimation.o: asciimation.cpp asciimation.hpp
	$(CXX) -c -o asciimation.o $(CXX_ARGS) asciimation.cpp

scroll_left.o: scroll_left.cpp scroll_left.hpp asciimation.hpp
	$(CXX) -c -o scroll_left.o $(CXX_ARGS) scroll_left.cpp

slide_up.o: slide_up.cpp slide_up.hpp asciimation.hpp
	$(CXX) -c -o slide_up.o $(CXX_ARGS) slide_up.cpp

main.o: main.cpp slide_up.hpp scroll_left.hpp
	$(CXX) -c -o main.o $(CXX_ARGS) main.cpp

main: main.o slide_up.o scroll_left.o asciimation.o
	$(CXX) -o main main.o slide_up.o scroll_left.o asciimation.o -lncurses -lpthread
