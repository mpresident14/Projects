#ifndef ASCIIMATION_HPP
#define ASCIIMATION_HPP

#include <cstddef>

#include "ncurses_helper.hpp"

class Asciimation {
public:
    Asciimation(size_t delayMicros);
    ~Asciimation();
    virtual void update() = 0;
    virtual void animate() = 0;
    Asciimation& setDelay(size_t micros);

protected:
    void clearScreen();

    char** contents_;
    size_t picHeight_;
    size_t picWidth_;
    size_t height_;
    size_t width_;
    size_t updateDelayMicros_;
};

#endif
