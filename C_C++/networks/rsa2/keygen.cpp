#include <boost/multiprecision/cpp_int.hpp> 

#include <stack>

using namespace std;
using boost::multiprecision::uint512_t;
using boost::multiprecision::int1024_t;

/* Checks if (D, phi(N)) == 1 and if so, finds E,F s.t. DE-phi(N)F = 1 using Euclidean Algorithm */
/* a & b correspond to D and PHI(N), in any order & a >= b */
uint512_t findE(uint512_t d, uint512_t phi)
{
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

    // Euclidean alg: (a, b) = (b, a mod b). Store quotients in stack
    stack<uint512_t> qStack;
    while (*b != 0){
        qStack.emplace(*a / *b);
        mod = *a % *b;

        // Reuse these 3 
        uint512_t* tmp = a;
        a = b;
        b = modptr;
        modptr = tmp;
        cout << "a=" << *a << endl;
        cout << "b=" << *b << endl;
    }

    // a now equals gcd(a,b)
    // Return uint521_t(-1) upon failure. We need gcd(d, phi(N)) == 1
    if (*a != 1){
        return -1;
    }

    // Reverse Euclidean alg to find x,y s.t. ax + by = gcd(a,b)
    // x and y correspond to e and f in 
    qStack.pop(); // Pop last quotient, we don't need it

    // To figure out which # goes with a and which goes with b
    // If even, ax + by = g
    // If odd, ay + bx = g
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

    cout << "x=" << x << endl;
    cout << "y=" << y << endl;

    // We want de - phi(f) = 1 -> de + phi(-f) = 1.
    // If d > phi
    //   If e > 0: e = e
    //   If e < 0: e += phi
    // If d < phi:
    //   If 

    bool e_positive = even_size ^ d_greater;
    if (e_positive) {
        return x > 0 ? uint512_t(x) : uint512_t(y);
    }

    return x < 0 ? uint512_t(x + phi) : uint512_t(y + phi);


    // Organize s.t. x(max{a,b}) + y(min{a,b}) = gcd(a,b)
    // int* xAndY = new int[2];
    // if (evenSize){
    //     xAndY[0] = y;
    //     xAndY[1] = x;
    // }
    // else{
    //     xAndY[0] = x;
    //     xAndY[1] = y;
    // }

    // return xAndY;
}

int main()
{
    cout << findE(8, 5) << endl;
}