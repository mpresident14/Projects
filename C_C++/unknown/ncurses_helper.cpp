/**
 * Shamelessly copied (mostly) from HMC CS70
 * */

#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <csignal>
#include <cstdlib>

#include "ncurses_helper.hpp"

using namespace std;

void cleanup()
{
    // Restore the saved shell terminal before exiting.
    // Put the cursor back to beginning of the last row of the terminal, where
    // we expect it to be when the program exits.
    mvcur(0, COLS - 1, LINES - 1, 0);
    endwin();
}


void resizeHandler( int )
{
    endwin();
    refresh();
}


void cleanUpScreenHandler( int signum )
{
    cleanup();

    string err_name = "Unknown error";

    switch (signum) {
    case SIGSEGV:
        err_name = "Segmentation fault";
        break;
    case SIGFPE:
        err_name = "Floating point error";
        break;
    case SIGABRT:
        err_name = "Abort request";
        break;
    }
    cerr << "Interupt signal (" << signum << ") (" << err_name <<
      ") received.\n";

    exit(signum);
}


void init_ncurses()
{
    // Make sure the screen gets cleaned up if we exit abormally
    signal(SIGSEGV, cleanUpScreenHandler);
    signal(SIGFPE, cleanUpScreenHandler);
    signal(SIGABRT, cleanUpScreenHandler);
    signal(SIGWINCH, resizeHandler);

    // Initialize the screen
    initscr();

    // Don't echo characters entered by the user
    noecho();

    // Make the cursor invisible
    curs_set(0);

    // Scroll if update goes past bottom of screen.
    scrollok(stdscr, TRUE);
}

/* *
 * Grab line and put it in the buffer WITHOUT terminating null char.
 * If line has < n chars, fill end with spaces.
 * */
void extractLine(std::ifstream& in, char *buf, size_t n)
{
    char c;
    size_t i = 0;
    while ((c = in.get()) != '\n' && i != n) {
        buf[i++] = c;
    }

    for (; i < n; ++i) {
        buf[i] = ' ';
    }
}
