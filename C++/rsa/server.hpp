#ifndef SERVER_HPP
#define SERVER_HPP 1

#include "rsa_decrypter.hpp"

class Server {
    public:
        Server() = default;
        ~Server() = default;
        Server(const Server& other) = default;
        void receiveMessage(ushort* encryptedMsg);
        RSAEncrypter& sendEncrypter();

    private:    
        RSADecrypter decrypter_;
};

#endif