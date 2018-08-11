#include <cstddef>
#include <iostream>

using namespace std;

// Tests whether num is a power of 4 (bit operators)
bool isPowerOfFour(int num) {
    if (num <= 0){
        return false;
    }
    // Get least significant one
    int ls_one = num & (~num + 1);
    
    // Only passes if it has only 1 one
    if (num != ls_one){
        return false;
    }
    // num & 0101...0101 (needs to have a 1 in power of four position)
    if ( (num & 5) == 0){
        return false;
    }
    return true;
}