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

void readySetGo()
{
    const auto sleepTime = 500ms;
    cout << "READY" << flush;
    this_thread::sleep_for(sleepTime);
    cout << "." << flush;
    this_thread::sleep_for(sleepTime);
    cout << "." << flush;
    this_thread::sleep_for(sleepTime);
    cout << "." << flush;
    this_thread::sleep_for(sleepTime);
    cout << "SET" << flush;
    this_thread::sleep_for(sleepTime);
    cout << "."  << flush;
    this_thread::sleep_for(sleepTime);
    cout << "."  << flush;
    this_thread::sleep_for(sleepTime);
    cout << "."  << flush;
    this_thread::sleep_for(sleepTime);
    cout << "GO!!!" << endl;
}

bool passwordGame()
{
    ScrollLeft("pics/computer.txt").animate();
    SlideUp("story/password.txt").animate();
    SlideUp("story/password_challenge.txt").animate();

    cleanup();
    system("clear");
    readySetGo();

    string password = "1234";
    random_shuffle(password.begin(), password.end());
    auto future = async(launch::async, timer, 30);

    string guess;
    while (future.wait_for(0ms) != future_status::ready) {
        getline(cin, guess);
        if (guess == password) {
            init_ncurses();
            SlideUp("story/access_granted.txt").animate();
            return true;
        } else {
            cout << "Access Denied!"  << '\n' << endl;
        }
    }

    init_ncurses();
    SlideUp("story/mission_failed.txt").animate();
    return false;
}

const char alphanum[] = "            0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
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

bool eavesdropGame()
{
    ScrollLeft("pics/phone.txt").animate();
    SlideUp("story/eavesdrop.txt").animate();
    SlideUp("story/eavesdrop_challenge.txt").animate();

    cleanup();
    system("clear");
    readySetGo();

    auto future = async(launch::async, timer, 45);
    string userInput;

    for (size_t i = 1; i < 6; ++i) {
        string randText = randomString(i * 5);
        cout << randText << endl;
        while (true) {
            if (future.wait_for(0ms) == future_status::ready) {
                init_ncurses();
                SlideUp("story/mission_failed.txt").animate();
                return false;
            }
            getline(cin, userInput);
            if (userInput == randText) {
                cout << "CORRECT!" << '\n' << endl;
                break;
            } else {
                cout << "INCORRECT! TRY AGAIN." << '\n' << endl;
                cout << randText << endl;
            }
        }
    }

    init_ncurses();
    SlideUp("story/success.txt").animate();
    return true;
}

int main()
{
    srand(time(0));
    // Set up ncurses so that it's ready to display our asciimation.
    init_ncurses();

    ScrollLeft("pics/vday_presents.txt").setDelay(30000).animate();

    SlideUp("pics/ralph_vs_pikachu.txt").animate();
    SlideUp("story/intro.txt").animate();

    hack_or_spy:
        if (SlideUp("story/hack_or_spy.txt").animateWithChoices("12") == '1') {
            if (!passwordGame()) {
                goto hack_or_spy;
            }
        } else {
            if (!eavesdropGame()) {
                goto hack_or_spy;
            }
        }

    SlideUp("story/fantastic_work.txt").animate();
    SlideUp("pics/happy_vday.txt").animate();

    // Reset ncurses settings
    cleanup();

    return 0;
}
