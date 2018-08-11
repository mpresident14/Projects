#include "server.hpp"

using namespace std;

void Server::receiveMessage(ushort* encryptedMsg)
{
    char* msg = decrypter_.decryptMessage(encryptedMsg);
    cout << "MESSAGE RECEIVED: " << msg << endl;
}

RSAEncrypter& Server::sendEncrypter()
{
    return decrypter_.getEncrypter();
}
