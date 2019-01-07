#include <iostream>
#include <cstddef>
#include <cstring>
#include <cmath>

using namespace std;

int a_to_i(char* arg){
    int result = 0;
    int digit;
    int length = strlen(arg);
    int is_neg = 0;
    // Parse negative number
    if (arg[0] == '-'){
        is_neg = 1;
    }

    for (int i = is_neg; i < length; ++i){
        // Make sure char is in "0123456789"
        if (arg[i] < 48 || arg[i] > 57){
            return 0;
        }
        // Get char as an int
        digit = int(arg[i] - 48);

        // Multiply by appropriate power of 10
        result += pow(10.0, length-1-i) * digit;
    }
    if (is_neg){
        return -result;
    }
    return result;
}

int reverse_digits(int num){
    // One digit number reversed is itself
    if (num <= 9 && num >= -9){
        return num;
    }
    // Negative?
    int is_neg = 0;
    if (num < 0){
        is_neg = 1;
        num = -num;
    }
    int result = 0;
    int digit;
    int length = 0;
    int cpy = num;
    // Find number of digits
    while (cpy != 0){
        ++length;
        cpy /= 10;
    }
    // Multiply each digit by appropriate power of 10
    for (int i = 0; i < length; ++i){
        digit = num % 10;
        result += pow(10.0, length-1-i) * digit;
        num /= 10;
    }
    // Restore negative
    if (is_neg){
        return -result;
    }
    return result;
}

int main(int argc, char** argv){
    if (argc != 2){
        cerr << "Usage: atoi <arg>" << endl;
        exit(0);
    }

    cout << reverse_digits(a_to_i(argv[1])) << endl;
}
