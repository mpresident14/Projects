#include "server.hpp"
#include "rsa_utils.hpp"
#include "keygen.hpp"

#include <thread>

#define P "35201546659608842026088328007565866231962578784643756647773109869245232364730066609837018108561065242031153677"
#define Q "499490918065850301921197603564081112780623690273420984342968690594064612108591217229304461006005170865294466527166368851"

using namespace std;

Server::Server()
    : is_stopped_{false}, thr_{&Server::process_msgs, this}
{
   cpp_int p{P};
   cpp_int q{Q};
   n_ = p * q;
   array<cpp_int, 2> keys = generate_keys((p - 1) * (q - 1));
   d_ = move(keys[0]);
   e_ = move(keys[1]);
}

void Server::recv_msg(const vector<cpp_int>& encrypted_msg)
{
    buffer_.push(encrypted_msg);
}

void Server::process_msgs()
{
    vector<cpp_int> msg;
    while (!is_stopped_.load()) {
        if (buffer_.try_pull(msg) == boost::concurrent::queue_op_status::success) {
            cout << "Server received: \"" << decrypt_msg(msg, d_, n_) << "\"" << endl;
        }
    }

    // Flush the remaining messages
    while (buffer_.try_pull(msg) == boost::concurrent::queue_op_status::success) {
        cout << "Server received: \"" << decrypt_msg(msg, d_, n_) << "\"" << endl;
    }
}

const cpp_int& Server::get_public_key()
{
    return e_;
}

const cpp_int& Server::get_n()
{
    return n_;
}

void Server::stop()
{
    is_stopped_.store(true);
    thr_.join();
}
