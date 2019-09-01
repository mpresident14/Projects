#include "server.hpp"
#include "keygen.hpp"
#include "rsa_utils.hpp"

#include <thread>

#define P                                                                      \
  "35201546659608842026088328007565866231962578784643756647773109869245232364" \
  "730066609837018108561065242031153677"
#define Q                                                                      \
  "49949091806585030192119760356408111278062369027342098434296869059406461210" \
  "8591217229304461006005170865294466527166368851"

using namespace std;

Server::Server() : is_stopped_{false}, thr_{&Server::process_msgs, this} {
  cpp_int p{P};
  cpp_int q{Q};
  n_ = p * q;
  array<cpp_int, 2> keys{generate_keys((p - 1) * (q - 1))};
  d_ = move(keys[0]);
  e_ = move(keys[1]);
}

void Server::recv_msg(const vector<cpp_int>& encrypted_msg) {
  buffer_.push(encrypted_msg);
}

void Server::recv_msg(vector<cpp_int>&& encrypted_msg) {
  buffer_.push(move(encrypted_msg));
}

void Server::process_msgs() {
  vector<cpp_int> msg;
  while (!is_stopped_) {
    if (buffer_.try_pull(msg) == boost::concurrent::queue_op_status::success) {
      cout << "Server received: \"" << decrypt_msg(msg, d_, n_) << "\"" << endl;
    }
  }

  // Flush the remaining messages
  while (buffer_.try_pull(msg) == boost::concurrent::queue_op_status::success) {
    cout << "Server received: \"" << decrypt_msg(msg, d_, n_) << "\"" << endl;
  }
}

const cpp_int& Server::get_public_key() { return e_; }

const cpp_int& Server::get_n() { return n_; }

void Server::stop() {
  is_stopped_ = true;
  thr_.join();
}
