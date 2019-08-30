#include <boost/multiprecision/cpp_int.hpp> 

using boost::multiprecision::uint512_t;

/* Checks if (D, phi(N)) == 1 and if so, finds E,F s.t. DE-phi(N)F = 1 using Euclidean Algorithm */
/* a & b correspond to D and PHI(N), in any order & a >= b */
uint512_t findE(uint512_t d, uint512_t phi)
{
    // Euclidean alg: (b, a) = (a, b mod a). Store quotients in stack
    stack<size_t> qStack;
    while (b != 0){
        qStack.push(a/b);
        size_t temp = a % b;
        a = b;
        b = temp;
    }
    // a now equals gcd(a,b)
    // Return nullptr upon failure, we need gcd(d, phi(N)) == 1
    if (a != 1){
        return nullptr;
    }

    // Reverse Euclidean alg to find x,y s.t. ax + by = gcd(a,b)
    // x and y correspond to e and f in 
    qStack.pop(); // pop last quotient (unneeded)
    // To figure out which # goes with a and which goes with b
    bool evenSize = qStack.size() % 2 == 0 ? true : false; 
    // Accumulate x and y by adding -quotient on top of stack times x or y
    int x = 1;
    int y = -1 * qStack.top();
    qStack.pop();

    while(!qStack.empty()){
        x += -y * qStack.top();
        qStack.pop();
        if (!qStack.empty()){
            y += -x * qStack.top();
            qStack.pop();
        }
    }

    // Organize s.t. x(max{a,b}) + y(min{a,b}) = gcd(a,b)
    int* xAndY = new int[2];
    if (evenSize){
        xAndY[0] = y;
        xAndY[1] = x;
    }
    else{
        xAndY[0] = x;
        xAndY[1] = y;
    }

    return xAndY;
}