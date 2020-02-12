#include "asciimation.hpp"

#include <ncurses.h>

using namespace std;

/*
 * Grab line and put it in the buffer WITHOUT terminating null char.
 * If line has < n chars, fill end with spaces.
 */
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

Asciimation::Asciimation(size_t delayMicros)
    : updateDelayMicros_(delayMicros)
{}

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
