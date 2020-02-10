/**
 * \file ncurses_helper.hpp
 * \author CS 70 Provided Code
 *
 * \brief Makes ncurses a little more user-friendly for asciimation.
 *
 * \details
 *
 * \remarks
 *
 */

#include <fstream>

void cleanUpScreenHandler( int );

void cleanup();

void init_ncurses();

void extractLine(std::ifstream& in, char *buf, size_t n);
