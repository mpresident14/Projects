#ifndef USER_HPP
#define USER_HPP 1

#include "server.hpp"

class User {
    public:
        User(Server& server);
        ~User() = default;
        User(const User& other) = default;
        void sendMessage(char* msg);

    private:
        RSAEncrypter& requestEncrypterFromServer();
        void sendToServer(ushort* encryptedMsg);

        Server& server_;
};

#endif
