#include "user.hpp"
#include "rsa_utils.hpp"

using namespace std;

User::User(Server& server)
    : server_{server}
{
    // Nothing to do
}

cpp_int User::requestPublicKey(Server& server)
{
    // return server_.sendEncrypter();
}

void User::sendToServer(ushort* encryptedMsg, size_t msgArrLen)
{
    server_.receiveMessage(encryptedMsg, msgArrLen);
}
