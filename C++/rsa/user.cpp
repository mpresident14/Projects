#include "user.hpp"

using namespace std;

User::User(Server& server)
    : server_{server}
{
    // Nothing to do
}

void User::sendMessage(char* msg)
{
    RSAEncrypter& encrypter = requestEncrypterFromServer();
    ushort* encryptedMsg = encrypter.encryptMessage(msg);
    sendToServer(encryptedMsg);
}

RSAEncrypter& User::requestEncrypterFromServer()
{
    return server_.sendEncrypter();
}

void User::sendToServer(ushort* encryptedMsg)
{
    server_.receiveMessage(encryptedMsg);
}
