#include "scroll_left.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <ncurses.h>
#include <unistd.h>

using namespace std;

ScrollLeft::ScrollLeft(const char *filename)
    : currentCol_(0)
{
    updateDelayMicros_ = 50000;

    ifstream inputFile(filename);
    if (!inputFile.good()) {
        throw invalid_argument( "Couldn't open file for reading" );
    }
    inputFile >> picHeight_;
    inputFile >> picWidth_;
    height_ = picHeight_;
    width_ = picWidth_ * 3;

    // Read past the newline characters after the height.
    inputFile.ignore();
    inputFile.ignore();

    contents_ = new char *[height_];
    for (size_t i = 0; i < height_; ++i) {
        contents_[i] = new char[width_];

        // Fill in left and right portions with spaces
        memset(&contents_[i][0], ' ', picWidth_);
        memset(&contents_[i][picWidth_ * 2], ' ', picWidth_);

        // Fill in middle portion with content
        extractLine(inputFile, &contents_[i][picWidth_], picWidth_);
    }
}

void ScrollLeft::update()
{
    for(size_t i = 0; i < picHeight_; ++i){
        for(size_t j = 0; j < picWidth_; ++j){
            mvaddch(i, j, contents_[i][j + currentCol_]);
        }
    }

    ++currentCol_;
}

void ScrollLeft::animate()
{
    for (size_t i = 0; i < picWidth_ * 2 + 1; ++i) {
        update();
        refresh();
        usleep(updateDelayMicros_);
    }

    clearScreen();
}
