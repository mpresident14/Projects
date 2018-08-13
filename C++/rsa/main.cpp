#include "user.hpp"
#include <vector>
#include <string>

using namespace std;

void sendMessages(Server* server, size_t num)
{
    User user{*server};
    for (int i = 1; i < 51; ++i) {
        string msg = "This is secure message #" + to_string(i) + " from user #" + to_string(num);
        user.sendMessage(msg.c_str());
    }
}

// TODO: mutex for encryption printing?
// TODO: the user threads should be joined in Server::stop
int main()
{
    srand(unsigned(time(NULL)));
    size_t numUsers = 10;
    std::vector<std::thread> threads;
    Server server;

    for (size_t i = 0; i < numUsers; ++i) {
        threads.push_back(thread(sendMessages, &server, i));
    }   

    server.stop(threads);
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
