#include <iostream>
#include <cstddef>
#include <string>
#include <algorithm>

using namespace std;
// TODO: Try with dynamic programming!

string lcsubstr(string r, string s){

    if (r == "" || s == ""){
        return "";
    }

    string substring;
    size_t i = 0;
    size_t min_length = r.size() < s.size() ? r.size() : s.size();

    // Find length of common substring at start
    while( i < min_length && r.at(i) == s.at(i)  ){
        substring += r.at(i);
        ++i;
    }
    
    // Recurse (drop first letter on one, compare to other full)
    string rest_r = r.substr(1, string::npos);
    string rest_s = s.substr(1, string::npos);

    string string1 = lcsubstr(r, rest_s);
    string string2 = lcsubstr(rest_r, s);

    // Get largest of common substring at start and the 2 recursive calls
    string greater = string1.size() > string2.size() ? string1 : string2;
    string longest = substring > greater ? substring : greater;
    return longest;
}

string lcsubseq(string r, string s){

    if (r == "" || s == ""){
        return "";
    }
    else if(r.at(0) == s.at(0)){
        string rest_r = r.substr(1, string::npos);
        string rest_s = s.substr(1, string::npos);
        return r.at(0) + lcsubseq(rest_r, rest_s);
    }
    else{
        string rest_r = r.substr(1, string::npos);
        string rest_s = s.substr(1, string::npos);

        string string1 = lcsubseq(r, rest_s);
        string string2 = lcsubseq(rest_r, s);

        string longest = string1.size() > string2.size() ? string1 : string2;

        return longest;
    }
}

string dp_lcsubseq(string s, string t){

    // Create main grid
    size_t nrows = s.size()+1;
    size_t ncols = t.size()+1;
    size_t** grid = new size_t*[nrows];
    
    for (size_t i = 0; i < nrows; ++i){
        grid[i] = new size_t[ncols];
    }
    
    /* Populate grid
     * Compare chars
     * If a match, get upper left + 1
     * Otherwise get max(left, above)
     */

    // Dynamic Allocate the rest of the grid
    for (size_t row = 0; row < nrows; ++row){
        for (size_t col = 0; col < ncols; ++col){
            if (row == 0 || col == 0){
                grid[row][col] = 0;
            }
            else if (s.at(row-1) == t.at(col-1)){
                grid[row][col] = 1 + grid[row-1][col-1];
            }
            else{
                grid[row][col] = max(grid[row-1][col], grid[row][col-1]);
            }
        }
    }
    
    // // Print grid
    // cout << endl;
    // for (size_t i = 0; i < nrows; ++i){
    //     for (size_t j = 0; j < ncols; ++j){
    //         cout << grid[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // cout << endl;


    // Set up lcs string
    string lcs;    
    size_t cur_cell = 0;

    /* Traverse backwards
     * If left same and above same: go left
     * If left same and above diff: go left (doesn't matter)
     * If left diff and above same: go above
     * If left diff and above diff: add char and go left (doesn't matter)
     */

    size_t row = nrows-1;
    size_t col = ncols-1;
    while (grid[row][col] != 0){
        // cout << "row=" << row << ", col=" << col << endl;

        cur_cell = grid[row][col];
        if (row != 0 && col != 0){
            if (grid[row][col-1] == cur_cell){
                --col;
            }
            else{
                if (grid[row-1][col] == cur_cell){
                    --row;
                }
                else{
                    lcs.insert(0, 1, s.at(row-1));
                    --col;
                }
            }
        }
    }
    return lcs;
}

string dp_lcsubstr(string s, string t){

    // Create grid
    size_t nrows = s.size() + 1;
    size_t ncols = t.size() + 1;
    size_t** grid = new size_t*[nrows];
    for (size_t i = 0; i < nrows; ++i){
        grid[i] = new size_t[ncols];
    }

    /* Populate grid and keep track of max
     * If a match get above left and add 1
     * Otherwise, it is zero (have to start the length count over) 
     */

    size_t maxLen = 0;
    size_t maxRow = 0;
    size_t maxCol = 0;

    for (size_t row = 0; row < nrows; ++row){
        for (size_t col = 0; col < ncols; ++col){
            if (row == 0 || col == 0){
                grid[row][col] = 0;
            }
            else if (s.at(row-1) == t.at(col-1)){
                grid[row][col] = 1 + grid[row-1][col-1];
                if (grid[row][col] > maxLen){
                    maxLen = grid[row][col];
                    maxRow = row;
                    maxCol = col;
                }
                
            }
            else{
                grid[row][col] = 0;
            }
        }
    }

    // // Print grid
    // cout << endl;
    // for (size_t i = 0; i < nrows; ++i){
    //     for (size_t j = 0; j < ncols; ++j){
    //         cout << grid[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // cout << endl;

    // Use the maxRow and maxCol and go up left until you reach 0
    char* lcs = new char[maxLen+1];
    lcs[maxLen] = '\0';
    size_t row = maxRow;
    size_t col = maxCol;
    int index = maxLen - 1;
    while(grid[row][col] != 0){
        lcs[index] = s.at(row-1);
        --index;
        --row;
        --col;
    }

    return lcs;
}

int main(int argc, char** argv){
    if (argc != 3){
        cerr << "ERROR: Input 2 string arguments" << endl;
    }
    else{
        //cout << lcsubseq(argv[1], argv[2]) << endl;
        cout << dp_lcsubstr(argv[1], argv[2]) << endl;
    }
}