#ifndef RSA_ENCRYPTER_HPP_INCLUDED
#define RSA_ENCRYPTER_HPP_INCLUDED 1

#include "rsa_utils.hpp"
#include <string.h>
#include <cstring>

class RSAEncrypter {
    public:
        RSAEncrypter() = default;
        RSAEncrypter(ushort n, ushort e);
        ~RSAEncrypter() = default;
        RSAEncrypter(const RSAEncrypter& other) = default;
        ushort* encryptMessage(const char* msg, size_t msgArrLen);
        size_t calculateMsgArrLen(const char* msg);

    // private:
        ushort* encodeMsgArr(ushort* msgArr, size_t msgArrLen);
        ushort* msgToArray(const char* msg, size_t msgArrLen);

        ushort n_;
        ushort e_;
};

#endif