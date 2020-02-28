#ifndef USER_HPP
#define USER_HPP 1

#include "server.hpp"

#include <boost/multiprecision/cpp_int.hpp>

using boost::multiprecision::cpp_int;

class User {
 public:
  User(const cpp_int& public_key, const cpp_int& n);
  // Compiler generated special member fcns are ok
  void send_msg(const char* msg, Server& server) const;

 private:
  cpp_int e_;  // public key
  cpp_int n_;  // P * Q
};

#endif
