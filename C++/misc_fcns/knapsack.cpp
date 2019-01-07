#include <iostream>
#include <cstddef>
#include <unordered_map>
#include <algorithm>
#include <boost/functional/hash.hpp>

using namespace std;

/* KNAPSACK PROBLEM: BEST TOTAL UNDER WEIGHT LIMIT */
/* f(i, lim) = best total using numbers up to index i (exclusive) with limit lim
   f(i, lim) = if i > lim: f(i-1, lim)
   f(i, lim) = if i <= lim: max(f(i-1, lim), f(i, lim - w_i) + v_i) */

int knapsack_helper(int* arr, int* weights, size_t length, int limit, 
    int index, unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>>& memo)
{
  if (index == 0) {
    return 0;
  }

  // Memoization: save previously calcuated results -- (index, limit) : best_value
  pair<int, int> index_limit = make_pair(index, limit);
  if (memo.count(index_limit)) {
    // cout << "Ran" << endl;
    return (memo.find(index_limit)) -> second;
  }

  int wt = weights[index - 1];
  int value;
  if (wt > limit) {
    value = knapsack_helper(arr, weights, length, limit, index - 1, memo);
    memo.insert(make_pair(index_limit, value));
    return value;
  }
  else {
    int loseit = knapsack_helper(arr, weights, length, limit, index - 1, memo);
    int useit = arr[index - 1] + knapsack_helper(arr, weights, length, limit - wt, index - 1, memo);
    value = max(loseit, useit);

    // cout << "loseit= " << loseit << endl;
    // cout << "useit= " << useit << endl;
    // cout << "index= " << index << endl;
    // cout << "limit= " << limit << endl;
    // cout << "value= " << value << "\n" << endl;
    
    memo.insert(make_pair(index_limit, value));
    return value;
  }
} 

int knapsack(int* arr, int* weights, size_t length, int limit)
{
  unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> memo;
  int value = knapsack_helper(arr, weights, length, limit, length, memo);
  // for (auto& pr : memo) {
  //   cout << "(" << pr.first.first << ", " << pr.first.second << ")" << " : " << pr.second << endl;
  // }
  return value;
}

int main()
{
  int arr[] = {5,7,4,3,2,6,3,5,7,4,9,2};
  int wts[] = {4,3,2,4,7,9,5,3,1,1,5,4};
  cout << knapsack(arr, wts, 12, 20) << endl;
}