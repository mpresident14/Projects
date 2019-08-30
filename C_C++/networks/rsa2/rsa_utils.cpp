#include "rsa_utils.hpp"
#include "keygen.hpp"

#include <string>
#include <cstring>
#include <cstddef>

#define P "35201546659608842026088328007565866231962578784643756647773109869245232364730066609837018108561065242031153677"
#define Q "499490918065850301921197603564081112780623690273420984342968690594064612108591217229304461006005170865294466527166368851"

using namespace std;
using boost::multiprecision::cpp_int;
using boost::multiprecision::limb_type;

void printByteVec(const vector<uint8_t>& vec);

char* decrypt_msg(const cpp_int& c, const cpp_int& d, const cpp_int& n)
{
    cpp_int decryption{power_mod(c, d, n)};
    cout << "decryption=" << hex << decryption << dec << endl;


    size_t cpp_size = decryption.backend().size();
    size_t msg_size = cpp_size * sizeof(limb_type);

    char* msg = new char[msg_size + 1];
    msg[msg_size] = '\0';
    memcpy(msg, decryption.backend().limbs(), msg_size);

    return msg;
}

cpp_int encrypt_msg(const char* msg, const cpp_int& e, const cpp_int& n)
{
    vector<uint8_t> byte_vec{};
    size_t msg_len = strlen(msg);

    // cpp_int_backend stores the bytes in reverse order, so we reverse the 
    // bytes before we deliver them to the cpp_int
    for (size_t i = msg_len - 1; i != size_t(-1); i--) {
        byte_vec.push_back(msg[i]);
    }

    cout << "byte_vec: ";
    printByteVec(byte_vec);

    cpp_int test{byte_vec};
    size_t size = test.backend().size();
    limb_type* p = test.backend().limbs();

    cout << "limbs: " << hex;
    for (size_t i = 0; i < size; ++i) {
        cout << *p << endl;
        ++p;
    }
    cout << endl;

    cout << "plaintext=" << hex << test << dec << endl;


    return power_mod(cpp_int{byte_vec}, e, n);
}



// vector<cpp_int> encrypt_msg(const string& msg, const cpp_int& e, const cpp_int& n)
// {  


//     vector<cpp_int> encrypted_msg{};
//     size_t msg_len = msg.size();

//     cout << hex;
//     for (size_t i = 0; i < msg_len; i++) {
//         cout << unsigned(msg[i]);
//     }
//     cout << endl;

//     // cpp_int holds 64 bytes
//     for (size_t i = 0; i < msg_len; i += 64){
//         encrypted_msg.emplace_back(msg.substr(i, i + 64));
//     }

//     size_t encrypted_len = encrypted_msg.size();
//     // for (size_t i = 0; i < encrypted_len; ++i){
//     //     cout << encrypted_msg[i];
//     //     encrypted_msg[i] = power_mod(encrypted_msg[i], e, n);
//     // }
//     cout << dec << endl;

//     return encrypted_msg;
// }

cpp_int power_mod(const cpp_int& base, const cpp_int& exp, const cpp_int& mod)
{
    cpp_int result = 1;
    // Grab greatest power of 2
    cpp_int currentPowerOf2 = greatest_power_of_two(exp);
    while (currentPowerOf2 != 0) {
        // Check if this power of 2 is in exponent's binary representation
        // and multiply accordingly
        if (exp & currentPowerOf2) {
            // Multiply and mod separately to prevent overflow
            result = (result * result * base) % mod;
            // result = (result * base) % mod;
        }
        else{
            result = (result * result) % mod;
        }
        // Shift to the next power of 2
        currentPowerOf2 >>= 1;
    }
    return cpp_int(result);
}

// Returns 0 if n == 0
cpp_int greatest_power_of_two(cpp_int n)
{
    if (n == 0) {
        return 0;
    }

    ushort shifts = 0;
    while (n != 0){
        n >>= 1;
        ++shifts;
    }
    return cpp_int(1) << (shifts - 1);
}

void printByteVec(const vector<uint8_t>& vec)
{
    cout << hex;
    for (auto& c : vec) {
        cout << unsigned(c);
    }
    cout << dec << endl;
}

int main(int argc, char** argv)
{
    cpp_int p{P};
    cpp_int q{Q};
    cpp_int n = p * q;
    cpp_int phi = (p - 1) * (q - 1);
    cout << "n=" << hex << n << dec << endl;
    cout << "phi=" << phi << endl;

    array<cpp_int, 2> keys = generate_keys(phi);

    cpp_int c{encrypt_msg(argv[1], keys[1], n)};
    cout << "ciphertext: " << hex << c << dec << endl;

    char* msg = decrypt_msg(c, keys[0], n);
    cout << msg << endl;
}
