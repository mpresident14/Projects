#ifndef RSA_UTILS_HPP_INCLUDED
#define RSA_UTILS_HPP_INCLUDED 1

#include <boost/multiprecision/cpp_int.hpp> 

using boost::multiprecision::uint512_t;
using boost::multiprecision::uint1024_t;

uint512_t computePowerModM(const uint512_t& base, const uint512_t& exp, const uint512_t& mod);
uint512_t greatestPowerOf2(uint512_t n);

#endif