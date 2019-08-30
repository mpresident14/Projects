#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp> 

#include <stack>
#include <memory>
#include <chrono>

using namespace std;

using boost::multiprecision::uint512_t;
using boost::multiprecision::int1024_t;
using generator512 = boost::random::independent_bits_engine<mt19937, 512, uint512_t>;

#define FAIL uint512_t(-1)

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

/* 
 * Checks if (D, phi(N)) == 1 and if so, finds E s.t. DE - phi(N)F = 1 
 * Returns uint512_t(-1) if (D, phi(N)) != 1 
 * Undefined behavior if d <= 1 or phi <= 1
 */
uint512_t findE(uint512_t d, uint512_t phi)
{
    // Going to lose phi's value while doing Euclidean Alg, and might need it later.
    uint512_t phi_copy{phi};

    // Let a be the greater of d and phi, b be the lesser
    bool d_greater;
    uint512_t* a;
    uint512_t* b;
    if (d > phi) {
        a = &d;
        b = &phi;
        d_greater = true;
    } else {
        a = &phi;
        b = &d;
        d_greater = false;
    }
    uint512_t mod;
    uint512_t* modptr = &mod;

    /* Euclidean alg: (a, b) = (b, a mod b). Store quotients in stack */
    stack<uint512_t> qStack;
    while (*b != 0){
        qStack.emplace(*a / *b);
        *modptr = *a % *b;

        // Reuse these 3 
        uint512_t* tmp = a;
        a = b;
        b = modptr;
        modptr = tmp;
    }

    // a now equals gcd(d, phi(N)). If a != 1, we need to try again.
    if (*a != 1){
        return FAIL;
    }

    /* Reverse Euclidean alg to find x,y s.t. ax + by = gcd(a,b) = 1 */

    // Pop last quotient, we don't need it
    qStack.pop(); 

    // To figure out which # goes with a and which goes with b (see below)
    bool even_size = qStack.size() % 2 == 0 ? true : false; 

    // Accumulate x and y by adding -quotient on top of stack times x or y
    int1024_t x{1};
    int1024_t y{-1};
    x *= qStack.top();
    qStack.pop();

    while(!qStack.empty()){
        x += -y * qStack.top();
        qStack.pop();
        if (!qStack.empty()){
            y += -x * qStack.top();
            qStack.pop();
        }
    }

    // We start the reverse alg with 1 = 1(a) - 1(b), so x is positive and y is negative.
    // If even, we finish with ax + by = 1
    // If odd, we finish with ay + bx = 1
    // If d > phi, then a = d & b = phi
    // If d < phi, then a = phi & b = d

    // Therefore:
    // 1) even & dGreater -> dx + phi(y)
    // 2) even & phiGreater -> phi(x) + dy
    // 3) odd & dGreater -> dy + phi(x)
    // 4) odd & phiGreater -> phi(y) + dx
    // Since 1) == 4) & 2) == 3), we can just xor even and dGreater.

    bool e_positive = even_size ^ d_greater;
    if (e_positive) {
        return uint512_t(x);
    }

    return uint512_t(y + phi_copy);
}

// Private key d first, public key e second
array<uint512_t, 2> generate_keys(const uint512_t& phi)
{
    generator512 gen{seed};
    auto keys = array<uint512_t, 2> {{gen()}};

    keys[0] = gen();

    while ((keys[1] = findE(keys[0], phi)) == FAIL) {
        keys[0] = gen();
    }

    return keys;
}

int main(int argc, char** argv)
{
    auto keys = generate_keys(atoi(argv[1]));

    cout << "d=" << keys[0] << endl;
    cout << "e=" << keys[1] << endl;
}
