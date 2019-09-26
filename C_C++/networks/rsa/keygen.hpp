#ifndef KEYGEN_HPP_INCLUDED
#define KEYGEN_HPP_INCLUDED 1

#include <boost/multiprecision/cpp_int.hpp>

using boost::multiprecision::cpp_int;

// Private key d first, public key e second
std::array<cpp_int, 2> generate_keys(const cpp_int& phi);

#endif