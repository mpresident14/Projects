#include "user.hpp"

using namespace std;

int main(int argc, char** argv)
{
    // srand(unsigned(time(NULL)));
    srand(1);
    if (argc != 2){
        cerr << "String argument required." << endl;
        exit(EXIT_FAILURE);
    }

    char* msg = argv[1];

    Server server;
    User user{server};

    user.sendMessage(msg);    

    return 0;
}

// printf("M: ");
// for (size_t i = 0; i < msgArrLen; ++i){
//     printf("%u ", encryptedMsg[i]);
// }
// cout << endl;