#ifndef SCROLL_LEFT_HPP
#define SCROLL_LEFT_HPP

#include "asciimation.hpp"

#include <cstring>

class ScrollLeft : public Asciimation {
public:
    ScrollLeft(const char *filename);
    void animate();

protected:
    size_t currentCol_;

private:
    void update();
};

#endif
