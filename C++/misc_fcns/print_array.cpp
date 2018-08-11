#include <cstddef>
#include <iostream>

using namespace std;

/*******************
 * Printing Arrays *
 * *****************/
void printArray(int nums[], int length)
{
    cout << "[";
    
    for (size_t i = 0; i < length; ++i){
        cout << nums[i];        
        if (i != length - 1){
            cout << ",";
        }
    }
    cout << "]" << endl;
}

void printArray(size_t nums[], size_t length)
{
    cout << "[";
    
    for (size_t i = 0; i < length; ++i){
        cout << nums[i];        
        if (i != length - 1){
            cout << ",";
        }
    }
    cout << "]" << endl;
}