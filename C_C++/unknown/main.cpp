#include "scroll_left.hpp"
#include "slide_up.hpp"
#include "ncurses_helper.hpp"

#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <thread>
#include <future>
#include <chrono>
#include <algorithm>
#include <ctime>
#include <cstring>

using namespace std;

void timer(size_t seconds, atomic_bool& b)
{
    for (size_t i = 0; i < seconds; ++i) {
        this_thread::sleep_for(1s);
    }
    b.store(true);
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
    printAndSleep("GO!!!\n\n");
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
    erase();
    noecho();
    curs_set(0);
}

const char alphanum[]
    = "            0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

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

bool passwordGame()
{
    ScrollLeft("txt/computer.txt").animate();
    SlideUp("txt/password.txt").animate();
    SlideUp("txt/password_challenge.txt").animate();

    setUpGame();

    char password[] = "1234";
    random_shuffle(password, password + 4);

    atomic_bool timesUp(false);
    thread thr(timer, 30, ref(timesUp));
    thr.detach();

    char guess[256];
    // This will allow an extra guess after timer stops, but that's ok
    while (!timesUp.load()) {
        getnstr(guess, 256);
        if (!strncmp(password, guess, 4)) {
            cleanUpGame();
            SlideUp("txt/access_granted.txt").animate();
            return true;
        } else {
            printw("ACCESS DENIED!\n\n");
            refresh();
        }
    }

    cleanUpGame();
    SlideUp("txt/mission_failed.txt").animate();
    return false;
}

bool eavesdropGame()
{
    ScrollLeft("txt/phone.txt").animate();
    SlideUp("txt/eavesdrop.txt").animate();
    SlideUp("txt/eavesdrop_challenge.txt").animate();

    setUpGame();

    atomic_bool timesUp(false);
    thread thr(timer, 45, ref(timesUp));
    thr.detach();

    char userInput[256];;
    for (size_t i = 1; i < 6; ++i) {
        size_t len = i * 5;
        string randText = randomString(len);
        printw("%s\n", randText.c_str());
        while (true) {
            // This will allow an extra input after timer stops, but that's ok
            if (timesUp.load()) {
                cleanUpGame();
                SlideUp("txt/mission_failed.txt").animate();
                return false;
            }
            getnstr(userInput, 256);
            if (!strncmp(userInput, randText.c_str(), len)) {
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
