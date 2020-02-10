#ifndef SLIDE_UP_HPP
#define SLIDE_UP_HPP

#include "asciimation.hpp"

#include <cstring>
#include <string>

class SlideUp : public Asciimation {
public:
    SlideUp(const char *filename);
    char animate(const std::string& charChoices);

protected:
    size_t currentRow_;

private:
    void update();
};

#endif
