#ifndef ASCIIMATION_HPP
#define ASCIIMATION_HPP

#include <cstddef>
#include <fstream>

#include "ncurses_helper.hpp"

class Asciimation {
public:
    ~Asciimation();
    virtual void update() = 0;

protected:
    virtual void extractLine(std::ifstream& in, char *buf, size_t n);
    virtual void clearScreen();

    char** contents_;
    size_t picHeight_;
    size_t picWidth_;
    size_t height_;
    size_t width_;
};

#endif
