#ifndef RSA_DECRYPTER_HPP_INCLUDED
#define RSA_DECRYPTER_HPP_INCLUDED 1

#define P 229 // 997
#define Q 277 // 751

#include "rsa_encrypter.hpp"

#include <stack>
#include <random>
#include <time.h>

class RSADecrypter {    

    public:
        RSADecrypter();
        ~RSADecrypter() = default;
        RSADecrypter(const RSADecrypter& other) = default;
        RSADecrypter& operator=(const RSADecrypter& other) = default;
        char* decryptMessage(ushort* encryptedMsg); 
        RSAEncrypter& getEncrypter();

    // private:
        ushort* decodeMsgArr(ushort* codedMsgArr);
        char* arrayToMsg(ushort* msgArr);
        int* findEAndF(size_t a, size_t b);
        void findDAndE();
        
        const ushort PHI = (P-1) * (Q-1);
        const ushort N = P * Q; /// public key

        ushort d_; /// private key
        ushort e_; /// public key      
        int f_; // for debugging
        size_t msgArrLen_;
        RSAEncrypter encrypter_;
};

#endif