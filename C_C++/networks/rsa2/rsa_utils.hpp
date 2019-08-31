#ifndef RSA_UTILS_HPP_INCLUDED
#define RSA_UTILS_HPP_INCLUDED 1

#define CHUNKSIZE 12

#include <boost/multiprecision/cpp_int.hpp> 

#include <vector>

using boost::multiprecision::cpp_int;

// cpp_int encrypt_msg(const char* msg, const cpp_int& e, const cpp_int& n);
cpp_int power_mod(const cpp_int& base, const cpp_int& exp, const cpp_int& mod);
cpp_int greatest_power_of_two(cpp_int n);

#endif