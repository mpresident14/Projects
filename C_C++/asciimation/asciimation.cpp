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

void Asciimation::clearScreen()
{
    for(size_t i = 0; i < picHeight_; ++i){
        for(size_t j = 0; j < picWidth_; ++j){
            mvaddch(i, j, ' ');
        }
    }
}

Asciimation& Asciimation::setDelay(size_t micros)
{
    updateDelayMicros_ = micros;
    return *this;
}
