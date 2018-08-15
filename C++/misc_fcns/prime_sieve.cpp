#include <cstddef>
#include <iostream>

using namespace std;

bool isPrime(size_t n, bool print)
{
    size_t length = n+1;
    bool* sieve = new bool[length] {false};
    size_t j = 0;
    size_t p = 2;

    while (p*p < length){
        // Mark all multiples of prime p as composite (start at p^2 b/c
        // smaller multiples of p have already been marked)
        for (size_t i = p*p; i < length; i += p){
            sieve[i] = true;
        }
    
        // Find next prime and set p to it
        j = p+1;
        while (j < length && sieve[j]){
            ++j;
        }
        p = j;
    }
    
    //Print all primes less than or equal to n
    if (print){
        for (size_t i = 2; i < length; ++i){
            if (!sieve[i]){
                cout << i << endl;
            }
        }
    }

    return !sieve[n];
}

bool isPrime2(size_t n, bool print)
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

int main(int argc, char** argv){
    bool print = false;
    if (argc == 3){
        if (!strcmp(argv[2], "-v")){
            print = true;
        }
    }
    
    cout << "\n" << isPrime2(atoi(argv[1]), print) << endl;
}