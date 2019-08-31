#include "user.hpp"
#include "server.hpp"

#include <vector>
#include <string>
#include <thread>

using namespace std;

void send_messages(const User& user, Server& server, size_t usernum)
{
    for (int i = 1; i < 5; ++i) {
        ostringstream str_stream;
        str_stream << "This is secure message #" << i << " from user #" << usernum;
        user.send_msg(str_stream.str().c_str(), server);
    }
}

// TODO: mutex for encryption printing?
// TODO: the user threads should be joined in Server::stop
int main(int argc, char** argv)
{
    // srand(unsigned(time(NULL)));
    size_t numUsers = 5;
    // std::vector<std::thread> threads;
    // Server server;

    // for (size_t i = 0; i < numUsers; ++i) {
    //     threads.push_back(thread(sendMessages, &server, i));
    // }   

    // server.stop(threads);
    Server server;
    const cpp_int& e = server.get_public_key();
    const cpp_int& n = server.get_n();

    vector<std::thread> threads;
    for (size_t i = 0; i < numUsers; ++i) {
        threads.emplace_back(send_messages, User{e, n}, ref(server), i);
    }

    // User user{e, n};
    // user.send_msg("Hello", server);

    for (auto& thr : threads) {
        thr.join();
    }
    server.stop();
}

// int main()
// {
//     srand(unsigned(time(NULL)));
//     // srand(1);

//     string msg;
//     Server server;
//     User user{server};

//     cout << "Enter message: ";
//     getline(cin, msg);

//     while (msg.compare("-exit") != 0) {
//         user.sendMessage(msg.c_str());
//         cout << "Enter message: ";
//         getline(cin, msg);
//     }
  
//     user.endSession();

//     return 0;
// }
