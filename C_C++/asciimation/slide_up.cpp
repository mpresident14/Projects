#include "slide_up.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <unistd.h>
#include <ncurses.h>

using namespace std;

SlideUp::SlideUp(const char *filename)
    : currentRow_(0)
{
    ifstream inputFile(filename);
    if (!inputFile.good()) {
        throw invalid_argument( "Couldn't open file for reading" );
    }
    inputFile >> picHeight_;
    inputFile >> picWidth_;
    height_ = picHeight_ * 2;
    width_ = picWidth_;

    // Read past the newline characters after the height.
    inputFile.ignore();
    inputFile.ignore();

    // Fill top portion with spaces
    contents_ = new char *[height_];
    for (size_t i = 0; i < picHeight_; ++i) {
        contents_[i] = new char[width_];
        memset(contents_[i], ' ', width_);
    }

    // Fill bottom portion with picture
    for (size_t i = picHeight_; i < height_; ++i) {
        contents_[i] = new char[width_];
        extractLine(inputFile, contents_[i], width_);
    }
}

void SlideUp::update()
{
    for(size_t i = 0; i < picHeight_; ++i){
        for(size_t j = 0; j < picWidth_; ++j){
            mvaddch(i, j, contents_[i + currentRow_][j]);
        }
    }

    ++currentRow_;
}

char SlideUp::animate(const string& charChoices)
{
    char c;

    for (size_t i = 0; i < picHeight_ + 1; ++i) {
        update();
        refresh();
        usleep(80000);
    }

    do {
        c = getchar();
    } while (!charChoices.empty() && charChoices.find(c) == string::npos);

    // Return the screen to its normal settings before exiting, since we don't
    // want to leave it in ncurses's preferred setup.
    clearScreen();

    return c;
}
