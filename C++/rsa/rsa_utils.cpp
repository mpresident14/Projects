#include "rsa_utils.hpp"

using namespace std;

ushort computePowerModM(ushort b, ushort e, ushort m)
{
    unsigned long long result = 1;
    // Grab greatest power of 2
    size_t currentPowerOf2 = greatestPowerOf2(e);
    while (currentPowerOf2 != 0){
        
        // Check if this power of 2 is in exponent's binary representation
        // and multiply accordingly
        if (e & currentPowerOf2){
            // cout << "hit" << endl;
            result = (result * result * b) % m;
        }
        else{
            // cout << "miss" << endl;
            result = (result * result) % m;
        }
        // Shift to new power of 2
        currentPowerOf2 >>= 1;
        // cout << "result=" << result << endl;
    }
    // cout << endl;
    return ushort(result);
}

size_t greatestPowerOf2(size_t n)
{
    size_t i = 0;
    while (n != 0){
        n >>= 1;
        ++i;
    }
    return size_t(pow(2.0, i-1));
}

bool isPrime(size_t n, bool print)
{
    // Odd sieve doesn't catch n == 2
    if (n == 2){
        if (print){
            cout << 2 << endl;
        }
        return true;
    }
    
    size_t length = (n+1)/2;
    // Sieve represents odds (1,3,5,7...n (if n is odd) ) b/c even #s are prime
    bool* sieve = new bool[length] {false}; 
    size_t j = 0;
    size_t p = 3;

    while (p*p < length){
        // Mark all multiples of prime p as composite (start at p^2 b/c
        // smaller multiples of p have already been marked)
        for (size_t i = p*p; i < n; i += 2*p){
            sieve[(i-1)/2] = true;
        }
    
        // Find next prime and set p to it
        j = p+2;
        while (j < length && sieve[(j-1)/2]){
            j += 2;
        }
        p = j;
    }

    //Print all primes less than or equal to n
    if (print){
        for (size_t i = 1; i < length; ++i){
            if (!sieve[i]){
                cout << 2*i+1 << endl;
            }
        }
    }

    return !sieve[length - 1];
}
