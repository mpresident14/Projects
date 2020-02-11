#ifndef SCROLL_LEFT_HPP
#define SCROLL_LEFT_HPP

#include "asciimation.hpp"

class ScrollLeft : public Asciimation {
public:
    ScrollLeft(const char *filename);
    void animate();

private:
    void update();

    size_t currentCol_;
};

#endif
