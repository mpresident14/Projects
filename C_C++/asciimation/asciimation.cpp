#include "asciimation.hpp"

#include <unistd.h>
#include <ncurses.h>
#include <iostream>

using namespace std;

Asciimation::~Asciimation()
{
    for(size_t i = 0; i < height_; ++i){
        delete[] contents_[i];
    }
    delete[] contents_;
}

void Asciimation::extractLine(std::ifstream& in, char *buf, size_t n)
{
    char c;
    size_t i = 0;
    while ((c = in.get()) != '\n' && i != n) {
        buf[i++] = c;
    }

    for (; i < n; ++i) {
        buf[i] = ' ';
    }
}

void Asciimation::clearScreen()
{
    for(size_t i = 0; i < picHeight_; ++i){
        for(size_t j = 0; j < picWidth_; ++j){
            mvaddch(i, j, ' ');
        }
    }
}
