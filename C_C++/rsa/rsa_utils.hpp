#ifndef RSA_UTILS_HPP_INCLUDED
#define RSA_UTILS_HPP_INCLUDED 1

#include <cstddef>
#include <iostream>
#include <math.h>

typedef unsigned short ushort;

ushort computePowerModM(ushort b, ushort e, ushort m);
size_t greatestPowerOf2(size_t n);
bool isPrime(size_t n, bool print);

#endif