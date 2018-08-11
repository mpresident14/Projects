#include "rsa_encrypter.hpp"

using namespace std;

RSAEncrypter::RSAEncrypter(ushort n, ushort e)
    : n_{n}, e_{e}, msgArrLen_{0}
{
    // Nothing to do
}

ushort* RSAEncrypter::encryptMessage(char* msg)
{
    ushort* msgArr = msgToArray(msg);
    ushort* encryptedMsg = encodeMsgArr(msgArr);

    printf("M: ");
    for (size_t i = 0; i < msgArrLen_; ++i){
        printf("%u ", encryptedMsg[i]);
    }
    cout << endl;

    return encryptedMsg;
}

ushort* RSAEncrypter::encodeMsgArr(ushort* msgArr)
{  
    ushort* codedMsgArr = new ushort[msgArrLen_];
    for (size_t i = 0; i < msgArrLen_; ++i){
        codedMsgArr[i] = computePowerModM(msgArr[i], e_, n_);
    }
    return codedMsgArr;
}

ushort* RSAEncrypter::msgToArray(char* msg)
{
    size_t typeSize = sizeof(ushort);
    size_t msgLength = strlen(msg);
    if (msgLength == 0){
        cerr << "Message cannot have length 0." << endl;
        exit(EXIT_FAILURE);
    }

    msgArrLen_ = (msgLength - 1) / typeSize + 1; // Each int_type takes up typeSize chars
    ushort* msgArr = new ushort[msgArrLen_];

    // Copy typeSize chars into every int_type
    size_t i; // msgArr index
    size_t j = 0; // msg index
    for (i = 0; i < msgArrLen_ - 1; ++i){
        memcpy(msgArr + i, msg + j, typeSize);
        j += typeSize;
    }

    // i at last position in cnvMsg
    // j at corresponding position in msg
    // For lengths that aren't multiples of typeSize, finish the rest
    // Note that the chars are store backward so if the remaining chars are "ing",
    // then they need to be stored "gni_"
    size_t k = 0; // # of bits to shift the char to store in the last int_type
    ushort last = 0;
    for (; j < msgLength; ++j){
        last += msg[j] << k;
        k += 8; // # of bits in a byte
    }
    msgArr[i] = last;

    return msgArr;
}

size_t RSAEncrypter::getMsgArrLen()
{
    return msgArrLen_;
}