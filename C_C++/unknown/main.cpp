#include "scroll_left.hpp"
#include "slide_up.hpp"

#include <iostream>
#include <atomic>
#include <ncurses.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <ctime>
#include <cstring>
#include <optional>
#include <cstdlib>
#include <csignal>

using namespace std;
using namespace std::string_literals;

atomic_bool timesUp(false);
const char alphanum[]
    = "            0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";


/*************************************************
 * ncurses utils shamelessly taken from HMC CS70 *
 *************************************************/
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


/************************************
 * Other (not stolen) ncurses utils *
 ************************************/

void moveCursorNextLine()
{
    int cy = getcury(stdscr);
    // If we get to the bottom of the screen, we can't move the cursor down
    // anymore, so we have to scroll up.
    if (cy == LINES - 1) {
        scroll(stdscr);
    } else {
        wmove(stdscr, cy + 1, 0);
        refresh();
    }
}

/*
 * Like getstr(), but allows a timeout to interrupt after any keystroke
 * (See timer() below)
 */
optional<string> getLineWithTimeout()
{
    optional<string> strOpt{string()};
    strOpt->reserve(5);

    char c;
    while ((c = getch()) != '\n' && c != '\r') {
        if (timesUp.load()) {
            return optional<string>();
        }

        if (c == '\b' || c == 127 /* DEL */) {
            int curX = getcurx(stdscr);
            // Hacky way to backspace the desired char, as well as the ^? that
            // is output when you hit Backspace. Really should turn on noecho()
            // and print characters myself b/c this may be machine-dependent.

            // If at the beginning of a line
            if (curX == 2) {
                mvwdelch(stdscr, getcury(stdscr), curX - 2);
                delch();
            } else {
                mvwdelch(stdscr, getcury(stdscr), curX - 3);
                delch();
                delch();
                strOpt->pop_back();
            }
        } else {
            strOpt->append(1, c);
        }
    }

    moveCursorNextLine();
    return strOpt;
}

/**************
 * Game utils *
 **************/

void timer(size_t seconds)
{
    for (size_t i = 0; i < seconds; ++i) {
        this_thread::sleep_for(1s);
    }
    timesUp.store(true);
}

void printAndSleep(const char *str)
{
    printw(str);
    refresh();
    this_thread::sleep_for(500ms);
}

void readySetGo()
{
    printAndSleep("READY");
    printAndSleep(".");
    printAndSleep(".");
    printAndSleep(".");
    printAndSleep("SET");
    printAndSleep("." );
    printAndSleep("." );
    printAndSleep("." );
    printw("GO!!!\n\n");
}

void setUpGame()
{
    wmove(stdscr, 0, 0);
    refresh();
    readySetGo();
    echo();
    curs_set(1);
}

void cleanUpGame()
{
    noecho();
    curs_set(0);
    erase();
}

string randomString(size_t len)
{
    string s;
    s.resize(len);

    for (size_t i = 0; i < len; ++i) {
        // No spaces at beginning or end.
        char c;
        do {
            c = alphanum[rand() % (sizeof(alphanum) - 1)];
        } while ((i == 0 || i == len - 1) && c == ' ');

        s[i] = c;
    }

    return s;
}

/*********
 * Games *
 *********/

bool passwordGame()
{
    ScrollLeft("txt/computer.txt").animate();
    SlideUp("txt/password.txt").animate();
    SlideUp("txt/password_challenge.txt").animate();

    setUpGame();

    string password = "1234"s;
    random_shuffle(password.begin(), password.end());

    timesUp = false;
    thread thr(timer, 30);
    thr.detach();

    while (true) {
        optional<string> guess = getLineWithTimeout();

        // Time expired
        if (!guess) {
            cleanUpGame();
            SlideUp("txt/mission_failed.txt").animate();
            return false;
        }

        if (*guess == password) {
            cleanUpGame();
            SlideUp("txt/access_granted.txt").animate();
            return true;
        } else {
            printw("ACCESS DENIED!\n\n");
        }
    }
}


bool eavesdropGame()
{
    ScrollLeft("txt/phone.txt").animate();
    SlideUp("txt/eavesdrop.txt").animate();
    SlideUp("txt/eavesdrop_challenge.txt").animate();

    setUpGame();

    timesUp = false;
    thread thr(timer, 45);
    thr.detach();

    for (size_t i = 1; i < 6; ++i) {
        size_t len = i * 5;
        string randText = randomString(len);
        printw("%s\n", randText.c_str());

        while (true) {
            optional<string> userInput = getLineWithTimeout();

            // Time expired
            if (!userInput) {
                cleanUpGame();
                SlideUp("txt/mission_failed.txt").animate();
                return false;
            }

            if (*userInput == randText) {
                printw("CORRECT!\n\n");
                break;
            } else {
                printw("INCORRECT! TRY AGAIN.\n\n");
                printw("%s\n", randText.c_str());
            }
        }
    }

    cleanUpGame();
    SlideUp("txt/success.txt").animate();
    return true;
}


int main()
{
    srand(time(0));

    // Set up ncurses so that it's ready to display our asciimation.
    init_ncurses();

    ScrollLeft("txt/vday_presents.txt").setDelay(30000).animate();
    SlideUp("txt/ralph_vs_pikachu.txt").animate();
    SlideUp("txt/intro.txt").animate();

    bool challengeComplete = false;
    while(!challengeComplete) {
        if (SlideUp("txt/hack_or_spy.txt").animateWithChoices("12") == '1') {
            challengeComplete = passwordGame();
        } else {
            challengeComplete = eavesdropGame();
        }
    }

    SlideUp("txt/fantastic_work.txt").animate();
    SlideUp("txt/happy_vday.txt").animate();

    // Reset ncurses settings
    cleanup();

    return 0;
}
