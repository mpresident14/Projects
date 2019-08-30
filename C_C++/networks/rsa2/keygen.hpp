#ifndef KEYGEN_HPP_INCLUDED
#define KEYGEN_HPP_INCLUDED 1

#include <boost/multiprecision/cpp_int.hpp>

using boost::multiprecision::uint512_t;

// Private key d first, public key e second
std::array<uint512_t, 2> generate_keys(const uint512_t& phi);

#endif