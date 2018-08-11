#include "rsa_decrypter.hpp"

using namespace std;

RSADecrypter::RSADecrypter() 
    : d_{0}, e_{0}, f_{0}, msgArrLen_{0}
{
    findDAndE();
    encrypter_ = {N, e_};
}

RSAEncrypter& RSADecrypter::getEncrypter()
{
    return encrypter_;
}

char* RSADecrypter::decryptMessage(ushort* encryptedMsg)
{
    msgArrLen_ = encrypter_.getMsgArrLen();
    ushort* origMsgArr = decodeMsgArr(encryptedMsg);
    return arrayToMsg(origMsgArr);
}

ushort* RSADecrypter::decodeMsgArr(ushort* codedMsgArr)
{
    ushort* msgArr = new ushort[msgArrLen_];
    for (size_t i = 0; i < msgArrLen_; ++i){
        msgArr[i] = computePowerModM(codedMsgArr[i], d_, N);
    }
    return msgArr;
}

/* Convert M to string */
char* RSADecrypter::arrayToMsg(ushort* msgArr)
{
    size_t typeSize = sizeof(ushort);
    size_t msgLen = msgArrLen_ * typeSize + 1;
    char* msg = new char[msgLen];
    msg[msgLen-1] = '\0';

    // Copy each long into 4 chars, 00 bytes will be null chars
    size_t j = 0;
    for (size_t i = 0; i < msgArrLen_; ++i){
        memcpy(msg+j, msgArr+i, typeSize);
        j += typeSize;
    }

    return msg;
}

/* Checks if (D, phi(N)) == 1 and if so, finds E,F s.t. DE-phi(N)F = 1 using Euclidean Algorithm */
/* a & b correspond to D and PHI(N), in any order & a >= b */
int* RSADecrypter::findEAndF(size_t a, size_t b)
{
    // Euclidean alg, store quotients in stack
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

void RSADecrypter::findDAndE()
{
    ushort d = 0;
    int e = 0;
    int f = 0;
    int* xAndY = nullptr;

    while (xAndY == nullptr){
        d = rand() % USHRT_MAX;
        if (d % 2 != 0){ // d can't be even b/c phi(N) is even
            // Find E and F s.t. DE - phi(N)F = 1
            if (d > PHI){
                xAndY = findEAndF(d, PHI);
                e = xAndY[0];
                f = xAndY[1];
            }
            else{
                xAndY = findEAndF(PHI, d);
                e = xAndY[1];
                f = xAndY[0];
            }            
        }        
    }

    // If e is negative, add D*phi(N) - phi(N)*D = 0 (adds e += PHI, F -= D)
    if (e < 0){
        e += PHI; // TODO: change this, could overflow e
        f -= d;   // TODO: same here
    }

    d_ = d;
    e_ = ushort(e);
    f_ = f;
}
