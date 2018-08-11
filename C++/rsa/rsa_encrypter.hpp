#ifndef RSA_ENCRYPTER_HPP_INCLUDED
#define RSA_ENCRYPTER_HPP_INCLUDED 1

#include "rsa_utils.hpp"

class RSAEncrypter {
    public:
        RSAEncrypter() = default;
        RSAEncrypter(ushort n, ushort e);
        ~RSAEncrypter() = default;
        RSAEncrypter(const RSAEncrypter& other) = default;
        ushort* encryptMessage(char* msg);
        size_t getMsgArrLen();

    // private:
        ushort* encodeMsgArr(ushort* msgArr);
        ushort* msgToArray(char* msg);

        ushort n_;
        ushort e_;
        size_t msgArrLen_;
};

#endif