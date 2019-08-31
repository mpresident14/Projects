#ifndef USER_HPP
#define USER_HPP 1

#include "server.hpp"

#include <boost/multiprecision/cpp_int.hpp>

using boost::multiprecision::cpp_int;

class User {
    public:
        User(const cpp_int& public_key, const cpp_int& n);
        // User(cpp_int&& public_key);
        ~User() = default;
        // User(const User& other) = default;
        void send_msg(const char* msg, Server& server);

    private:
        // RSAEncrypter& requestEncrypterFromServer();
        // void sendToServer(ushort* encryptedMsg, size_t msgArrLen);

        // Server& server_;
    	cpp_int e_; // public key
    	cpp_int n_; // P * Q


};

#endif
