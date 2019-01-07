
#include <iostream>
#include <cstddef>
#include <algorithm>
#include <unordered_map>
#include <chrono>

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

/******************
 * Getting Combos *
 * ****************/

void getCombosHelper(int* array, int* combo, int length, int comboSize, int distanceToEnd, int arrayIndex, int comboIndex){
    if (comboIndex == comboSize){
        printArray(combo, comboSize);
    }

    else{
        for (size_t i = arrayIndex; i < 2*length - comboSize - distanceToEnd + 1; ++i){
            combo[comboIndex] = array[i];
            getCombosHelper(array, combo, length, comboSize, distanceToEnd-1, i+1, comboIndex+1);
        }
    }
}

// Prints all combinations of comboSize elements in array
void getCombos(int* array, int length, int comboSize){
    int* combo = new int[comboSize];
    getCombosHelper(array, combo, length, comboSize, length, 0,0);
}

/***************************************
 * Find Combos That Sum to a Target *
 * *************************************/

void findSumCombosHelper(int* array, int* combo, int length, int comboSize, int target, int distanceToEnd, int arrayIndex, int comboIndex, int currentSum){
    if (comboIndex == comboSize){
        if (currentSum == target){
            printArray(combo, comboSize);
        }
    }

    else{
        for (size_t i = arrayIndex; i < 2*length - comboSize - distanceToEnd + 1; ++i){
            combo[comboIndex] = array[i];
            // Take advantage of sorted array, stop if we've overshot target
            if (currentSum + array[i] > target){
                break;
            }
            findSumCombosHelper(array, combo, length, comboSize, target, distanceToEnd-1, i+1, comboIndex+1, currentSum+array[i]);
        }
    }
}

// Prints all combinations of comboSize elements in array that sum to target
void findSumCombosSizeN(int* array, int length, int comboSize, int target){
    sort(array, array+length);
    int* combo = new int[comboSize];
    findSumCombosHelper(array, combo, length, comboSize, target, length, 0,0,0);
}

// Prints all combinations of elements in array that sum to target
void findSumCombos(int* array, int length, int target){
    for (int n = 1; n < length+1; ++n){
        findSumCombosSizeN(array, length, n, target);
    }
}





int main()
{
    // int length = 50;
    // int* array = new int[length];
    // for (int i = 0; i < length; ++i){
    //     array[i] = i+1;
    // }    
   
    // auto start = chrono::steady_clock::now();
    
    // findSumCombos(array, length, 50);    

    // auto stop = chrono::steady_clock::now();
    // cout << chrono::duration_cast<chrono::nanoseconds>(stop - start).count() / 1000000000.0
    //     << " seconds" << endl;    

    cout << isPowerOfFour(2) << endl;
    return 0;
}