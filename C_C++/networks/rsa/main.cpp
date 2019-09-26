#include "server.hpp"
#include "user.hpp"

#include <time.h>
#include <chrono>
#include <random>
#include <string>
#include <thread>
#include <vector>

#define USERS 8
#define MSGS 3

using namespace std;

void send_messages(User user, Server& server, size_t usernum) {
  this_thread::sleep_for(chrono::milliseconds(rand() % 3000));

  for (int i = 0; i < MSGS; ++i) {
    ostringstream str_stream;
    str_stream << "This is secure message #" << i << " from user #" << usernum;
    user.send_msg(str_stream.str().c_str(), server);
  }
}

int main() {
  srand(time(NULL));

  Server server;
  const cpp_int& e = server.get_public_key();
  const cpp_int& n = server.get_n();

  vector<std::thread> threads;
  for (size_t i = 0; i < USERS; ++i) {
    // Thread callback passes by value. We use std::ref to retain a reference to
    // the server. (Note that we must ensure all the threads join before the
    // server goes out of scope.)
    threads.emplace_back(thread{send_messages, User{e, n}, ref(server), i});
  }

  for (auto& thr : threads) {
    thr.join();
  }
  server.stop();
}
