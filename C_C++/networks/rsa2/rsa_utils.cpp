#include "rsa_utils.hpp"
#include "keygen.hpp"

#include <string>
#include <cstring>
#include <cstddef>
#include <cassert>

#define P "35201546659608842026088328007565866231962578784643756647773109869245232364730066609837018108561065242031153677"
#define Q "499490918065850301921197603564081112780623690273420984342968690594064612108591217229304461006005170865294466527166368851"

using namespace std;
using boost::multiprecision::cpp_int;
using boost::multiprecision::limb_type;

void printByteVec(const vector<uint8_t>& vec);

// Returns a null-terminated pointer
string decrypt_msg(const cpp_int& c, const cpp_int& d, const cpp_int& n)
{
    cpp_int decryption{power_mod(c, d, n)};

    vector<uint8_t> vec;
    export_bits(decryption, back_inserter(vec), 8);

    return string{vec.begin(), vec.end()};
}

// msg need not be null-terminated
// Will fail if the byte value of msg is greater than n, 
cpp_int encrypt_msg(const char* msg, const cpp_int& e, const cpp_int& n)
{
    vector<uint8_t> byte_vec;
    size_t msg_len = strlen(msg);

    // cpp_int_backend stores the bytes in reverse order, so we reverse the 
    // bytes before we deliver them to the cpp_int
    for (size_t i = 0; i < msg_len; i++) {
        byte_vec.push_back(msg[i]);
    }

    cpp_int test;
    import_bits(test, byte_vec.begin(), byte_vec.end());

    return power_mod(test, e, n);
}

cpp_int power_mod(const cpp_int& base, const cpp_int& exp, const cpp_int& mod)
{
    cpp_int result{1};
    // Grab greatest power of 2
    cpp_int currentPowerOf2 = greatest_power_of_two(exp);
    while (currentPowerOf2 != 0) {
        // Check if this power of 2 is in exponent's binary representation
        // and multiply accordingly
        if (exp & currentPowerOf2) {
            result = (result * result * base) % mod;
        }
        else{
            result = (result * result) % mod;
        }
        // Shift to the next power of 2
        currentPowerOf2 >>= 1;
    }
    return result;
}

// Returns 0 if n == 0
cpp_int greatest_power_of_two(cpp_int n)
{
    if (n == 0) {
        return 0;
    }

    unsigned shifts = 0;
    while (n != 0){
        n >>= 1;
        ++shifts;
    }
    return cpp_int{1} << (shifts - 1);
}

void printByteVec(const vector<uint8_t>& vec)
{
    cout << hex;
    for (auto& c : vec) {
        cout << c;
    }
    cout << dec << endl;
}

int main(int argc, char** argv)
{
    cpp_int p{P};
    cpp_int q{Q};
    cpp_int n = p * q;
    cpp_int phi = (p - 1) * (q - 1);

    array<cpp_int, 2> keys = generate_keys(phi);

    cpp_int c{encrypt_msg(argv[1], keys[1], n)};

    cout << decrypt_msg(c, keys[0], n) << endl;
}
