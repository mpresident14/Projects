#ifndef SLIDE_UP_HPP
#define SLIDE_UP_HPP

#include "asciimation.hpp"

#include <string>

class SlideUp : public Asciimation {
public:
    SlideUp(const char *filename);
    void animate();
    char animateWithChoices(const std::string& charChoices);

private:
    void update();

    size_t currentRow_;
};

#endif
