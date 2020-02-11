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

using namespace std;

void timer(size_t seconds, atomic_bool& b)
{
    for (size_t i = 0; i < seconds; ++i) {
        this_thread::sleep_for(1s);
    }
    b.store(true);
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

    cleanup();
    system("clear");
    readySetGo();

    string password = "1234";
    random_shuffle(password.begin(), password.end());

    atomic_bool timesUp(false);
    thread thr(timer, 30, ref(timesUp));
    thr.detach();

    string guess;
    // This will allow an extra guess after timer stops, but that's ok
    while (!timesUp.load()) {
        getline(cin, guess);
        if (guess == password) {
            init_ncurses();
            SlideUp("txt/access_granted.txt").animate();
            return true;
        } else {
            cout << "ACCESS DENIED!"  << '\n' << endl;
        }
    }

    init_ncurses();
    SlideUp("txt/mission_failed.txt").animate();
    return false;
}

bool eavesdropGame()
{
    ScrollLeft("txt/phone.txt").animate();
    SlideUp("txt/eavesdrop.txt").animate();
    SlideUp("txt/eavesdrop_challenge.txt").animate();

    cleanup();
    system("clear");
    readySetGo();

    atomic_bool timesUp(false);
    thread thr(timer, 45, ref(timesUp));
    thr.detach();

    string userInput;
    for (size_t i = 1; i < 6; ++i) {
        string randText = randomString(i * 5);
        cout << randText << endl;
        while (true) {
            // This will allow an extra input after timer stops, but that's ok
            if (timesUp.load()) {
                init_ncurses();
                SlideUp("txt/mission_failed.txt").animate();
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
