#include "rsa_utils.hpp"

#include <cstddef>
#include <cstring>
#include <random>
#include <string>

#include <time.h>

#define MAXCHUNKSIZE 128
#define P                                                                      \
  "35201546659608842026088328007565866231962578784643756647773109869245232364" \
  "730066609837018108561065242031153677"
#define Q                                                                      \
  "49949091806585030192119760356408111278062369027342098434296869059406461210" \
  "8591217229304461006005170865294466527166368851"

using namespace std;
using boost::multiprecision::cpp_int;

void printByteVec(const vector<uint8_t>& vec);

string decrypt_msg(const vector<cpp_int>& chunks, const cpp_int& d,
                   const cpp_int& n) {
  string msg;

  // Decrypt each chunk and append it to the string
  for (const cpp_int& c : chunks) {
    cpp_int decryption{power_mod(c, d, n)};
    export_bits(move(decryption), back_inserter(msg), 8);
  }

  return msg;
}

// Will fail if the byte value of msg is greater than n,
// so we encrypt the message in randomly sized chunks.
cpp_int encrypt_chunk(const char* msg, size_t chunk_size, const cpp_int& e,
                      const cpp_int& n) {
  vector<uint8_t> byte_vec;

  // Stop at the end of the message or chunk_size, whichever comes first
  for (size_t i = 0; i < chunk_size && msg[i] != '\0'; ++i) {
    byte_vec.push_back(msg[i]);
  }

  cpp_int encrypted_msg;
  import_bits(encrypted_msg, byte_vec.begin(), byte_vec.end());

  return power_mod(encrypted_msg, e, n);
}

vector<cpp_int> encrypt_msg(const char* msg, const cpp_int& e,
                            const cpp_int& n) {
  srand(time(NULL));
  size_t msg_len = strlen(msg);
  size_t chunk_size;
  size_t i = 0;
  vector<cpp_int> chunks;

  // Encrypt chunks of random sizes
  while (i < msg_len) {
    chunk_size = rand() % MAXCHUNKSIZE + 1;
    chunks.push_back(encrypt_chunk(&msg[i], chunk_size, e, n));
    i += chunk_size;
  }
  return chunks;
}

cpp_int power_mod(const cpp_int& base, const cpp_int& exp, const cpp_int& mod) {
  cpp_int result{1};
  // Grab greatest power of 2
  cpp_int current_power_of_2{greatest_power_of_two(exp)};
  while (current_power_of_2 != 0) {
    // Check if this power of 2 is in exponent's binary representation
    // and multiply accordingly
    if (exp & current_power_of_2) {
      result = (result * result * base) % mod;
    } else {
      result = (result * result) % mod;
    }
    // Shift to the next power of 2
    current_power_of_2 >>= 1;
  }
  return result;
}

// Returns -1 if n == 0
cpp_int greatest_power_of_two(const cpp_int& n) {
  if (n == 0) {
    return -1;
  }

  cpp_int result{1};

  while (result <= n) {
    result <<= 1;
  }
  return result >> 1;
}
