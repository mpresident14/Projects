#include "rsa_utils.hpp"

using namespace std;

uint512_t computePowerModM(uint512_t base, uint512_t exp, uint512_t mod)
{
    uint1024_t result = 1;
    // Grab greatest power of 2
    uint512_t currentPowerOf2 = greatestPowerOf2(exp);
    while (currentPowerOf2 != 0) {
        // Check if this power of 2 is in exponent's binary representation
        // and multiply accordingly
        if (exp & currentPowerOf2) {
            // Multiply and mod separately to prevent overflow
            result = (result * result) % mod;
            result = (result * base) % mod;
        }
        else{
            result = (result * result) % mod;
        }
        // Shift to the next power of 2
        currentPowerOf2 >>= 1;
    }
    return uint512_t(result);
}

// Returns 0 if n == 0
uint512_t  greatestPowerOf2(uint512_t n)
{
    if (n == 0) {
        return 0;
    }

    ushort shifts = 0;
    while (n != 0){
        n >>= 1;
        ++shifts;
    }
    return uint512_t(1) << (shifts - 1);
}

// int main()
// {
//     cout << greatestPowerOf2(127) << endl;
//     cout << greatestPowerOf2(128) << endl;
//     cout << hex << greatestPowerOf2(0xFFFFFFFF) << dec << endl;

//     cout << computePowerModM(3, 105, 23) << endl;
//     cout << computePowerModM(2, 77, 5) << endl;
// }
