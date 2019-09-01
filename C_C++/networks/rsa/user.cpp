#include "user.hpp"
#include "rsa_utils.hpp"

using namespace std;

User::User(const cpp_int& public_key, const cpp_int& n)
    : e_{public_key}, n_{n}
{
    // Nothing to do
}

void User::send_msg(const char* msg, Server& server) const
{
    server.recv_msg(encrypt_msg(msg, e_, n_));
}
