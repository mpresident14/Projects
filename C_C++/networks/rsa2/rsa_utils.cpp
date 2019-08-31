#include "rsa_utils.hpp"
#include "keygen.hpp"

#include <string>
#include <cstring>
#include <cstddef>
#include <cassert>
#include <random>

#include <time.h>

#define P "35201546659608842026088328007565866231962578784643756647773109869245232364730066609837018108561065242031153677"
#define Q "499490918065850301921197603564081112780623690273420984342968690594064612108591217229304461006005170865294466527166368851"

using namespace std;
using boost::multiprecision::cpp_int;
using boost::multiprecision::limb_type;

void printByteVec(const vector<uint8_t>& vec);

string decrypt_msg(const vector<cpp_int>& chunks, const cpp_int& d, const cpp_int& n)
{
    vector<uint8_t> vec;

    // Decrypt each chunk and add it to the byte vector
    for (const cpp_int& c : chunks) {
        cpp_int decryption{power_mod(c, d, n)};
        export_bits(decryption, back_inserter(vec), 8);
    }
    
    return string{vec.begin(), vec.end()};
}

// Will fail if the byte value of msg is greater than n, 
// so we encrypt the message in chunks. If the length of message > CHUNKSIZE,
// characters past index CHUNKSIZE will be dropped.
cpp_int encrypt_chunk(const char* msg, size_t chunk_size, const cpp_int& e, const cpp_int& n)
{
    vector<uint8_t> byte_vec;

    // Stop at the end of the message or chunk_size, whichever comes first
    for (size_t i = 0; i < chunk_size && msg[i] != '\0'; ++i) {
        byte_vec.push_back(msg[i]);
    }

    cpp_int encrypted_msg;
    import_bits(encrypted_msg, byte_vec.begin(), byte_vec.end());

    return power_mod(encrypted_msg, e, n);
}

vector<cpp_int> encrypt_msg(const char* msg, const cpp_int& e, const cpp_int& n)
{
    size_t msg_len = strlen(msg);
    size_t chunk_size;
    size_t i = 0;
    vector<cpp_int> chunks;
    srand(time(NULL));
    
    // Encrypt chunks of random sizes
    while(i < msg_len) {
        chunk_size = rand() % MAXCHUNKSIZE + 1;
        chunks.push_back(encrypt_chunk(&msg[i], chunk_size, e, n));
        i += chunk_size;
    }
    return chunks;
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

// int main(int argc, char** argv)
// {
//     cpp_int p{P};
//     cpp_int q{Q};
//     cpp_int n = p * q;
//     cpp_int phi = (p - 1) * (q - 1);

//     array<cpp_int, 2> keys = generate_keys(phi);

//     vector<cpp_int> chunks{encrypt_msg(argv[1], keys[1], n)};

//     cout << decrypt_msg(chunks, keys[0], n) << endl;
// }
