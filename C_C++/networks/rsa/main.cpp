#include "user.hpp"
#include "server.hpp"

#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <random>
#include <time.h>

using namespace std;

void send_messages(User user, Server& server, size_t usernum)
{ 
    this_thread::sleep_for(chrono::milliseconds(rand() % 3000));
    for (int i = 1; i < 5; ++i) {
        ostringstream str_stream;
        str_stream << "This is secure message #" << i << " from user #" << usernum;
        user.send_msg(str_stream.str().c_str(), server);
    }
}

int main(int argc, char** argv)
{
    srand(time(NULL));

    size_t numUsers = 5;

    Server server;
    const cpp_int& e = server.get_public_key();
    const cpp_int& n = server.get_n();

    vector<std::thread> threads;
    for (size_t i = 0; i < numUsers; ++i) {
        User user{e, n};
        // We move the user to avoid a copy of its data members.
        //   We cannot pass by reference because the local user will go out of scope.
        // We pass server by reference to prevent thread from calling the deleted copy
        //   constructor. We cannot move it because we still need the server for future calls.
        threads.emplace_back(thread{send_messages, move(user), ref(server), i});
    }

    for (auto& thr : threads) {
        thr.join();
    }
    server.stop();
}
