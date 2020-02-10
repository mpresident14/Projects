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
#include <iomanip>


using namespace std;


// TODO: Add 10, 30s warning
void timer(size_t seconds)
{
    for (size_t i = 0; i < seconds; ++i) {
        this_thread::sleep_for(1s);
    }
}

bool passwordGame()
{
    SlideUp("story/challenge.txt").animate("");
    cleanup();

    system("clear");
    cout << "START GUESSING!!!" << endl;

    string password = "1234";
    random_shuffle(password.begin(), password.end());

    auto future = async(launch::async, timer, 30);
    string guess;

    while (future.wait_for(0ms) != future_status::ready) {
        getline(cin, guess);

        if (guess == password) {
            init_ncurses();
            SlideUp("story/access_granted.txt").animate("");
            return true;
        }
    }

    init_ncurses();
    SlideUp("story/access_denied.txt").animate("");

    return false;
}

int main()
{
    srand(time(0));
    // Set up ncurses so that it's ready to display our asciimation.
    init_ncurses();

    SlideUp("pics/ralph_vs_pikachu.txt").animate("");
    SlideUp("story/intro.txt").animate("");

    hack_or_spy:
        if (SlideUp("story/hack_or_spy.txt").animate("12") == '1') {
            ScrollLeft("pics/computer.txt").animate();
            SlideUp("story/password.txt").animate("");
            if (!passwordGame()) {
                goto hack_or_spy;
            }
        } else {

        }


    // Reset ncurses settings
    cleanup();

    return 0;
}
