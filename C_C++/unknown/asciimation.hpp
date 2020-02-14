#ifndef ASCIIMATION_HPP
#define ASCIIMATION_HPP

#include <cstddef>
#include <fstream>

class Asciimation {
public:
    Asciimation(size_t delayMicros);
    ~Asciimation();
    virtual void animate() = 0;
    Asciimation& setDelay(size_t micros);

protected:
    static void extractLine(std::ifstream& in, char *buf, size_t n);

    void clearScreen() const;

    char** contents_;
    size_t picHeight_;
    size_t picWidth_;
    size_t height_;
    size_t width_;
    size_t updateDelayMicros_;
};


#endif
