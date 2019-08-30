#include "user.hpp"

using namespace std;

User::User(Server& server)
    : server_{server}
{
    // Nothing to do
}

void User::sendMessage(const char* msg)
{
    RSAEncrypter& encrypter = requestEncrypterFromServer();
    size_t msgArrLen = encrypter.calculateMsgArrLen(msg);
    ushort* encryptedMsg = encrypter.encryptMessage(msg, msgArrLen);
    sendToServer(encryptedMsg, msgArrLen);
}

RSAEncrypter& User::requestEncrypterFromServer()
{
    return server_.sendEncrypter();
}

void User::sendToServer(ushort* encryptedMsg, size_t msgArrLen)
{
    server_.receiveMessage(encryptedMsg, msgArrLen);
}
