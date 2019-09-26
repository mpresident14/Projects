
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <unordered_map>

using namespace std;

// cd c:\Users\Miles\Desktop\Cribbage\cribbage

// NOTE: WHEN WE RUN THESE, SORT ARRAY BEFOREHAND AND PASS IN THE SORTED VERSION
size_t scoreFifteens(size_t nums[], size_t length) {
  size_t count = 0;
  size_t total = 0;
  sort(nums, nums + length);

  /* Four Levels:
   * - left iterates forward through array
   * - right iterates backwards through array
   * - i iterates forward through array (after left)
   * - j iterates forward and adds on each value (after i)
   */

  for (size_t left = 0; left < length - 1; ++left) {
    for (size_t right = length - 1; right > left; --right) {
      cout << "left: " << left << ", right: " << right << endl;
      // Reset total
      total = nums[left] + nums[right];
      if (total == 15) {
        cout << nums[left] << ", " << nums[right] << endl;
        ++count;
        continue;  // move the right index backwards
      }

      for (size_t i = left + 1; i < right; ++i) {
        total = nums[left] + nums[right] + nums[i];
        if (total == 15) {
          ++count;
          continue;  // move the i index forward
        } else if (total > 15) {
          break;
        }

        for (size_t j = i + 1; j < right; ++j) {
          total += nums[j];
          if (total == 15) {
            ++count;
            break;  // j adds on to total, so no need to add anymore if we hit
                    // 15 already
          } else if (total > 15) {
            break;
          }
        }
      }
    }
  }
  return 2 * count;  // 2 points per combo of 15
}

size_t scoreFlush(size_t nums[], size_t length) {
  // Populate the hashmap
  unordered_map<size_t, size_t> counts;
  for (size_t i = 0; i < length; ++i) {
    if (counts.count(nums[i])) {
      counts[nums[i]] = counts[nums[i]] + 1;
    } else {
      counts[nums[i]] = 1;
    }
  }

  // Search for a count greater than or equal to 4
  for (auto iter = counts.begin(); iter != counts.end(); ++iter) {
    if (iter->second > 3) {
      if (iter->second == 5) {
        return 5;
      }
      return 4;
    }
  }
  return 0;
}

size_t scoreRuns(size_t nums[], size_t length) {
  sort(nums, nums + length);
  size_t score = 1;
  size_t multiplier = 1;

  for (size_t i = 0; i < length - 1; ++i) {
    if (nums[i + 1] == nums[i] + 1) {
      ++score;
    }

    else if (nums[i + 1] == nums[i]) {
      // If three of one value, triples the number of runs
      if (i > 0 && nums[i + 1] == nums[i - 1]) {
        ++multiplier;
      }
      // A duplicate value doubles the number of runs
      // (Two duplicates quadruples)
      else {
        multiplier *= 2;
      }
    } else {
      // If we already have a run and it has ended
      // return its score now
      if (score >= 3) {
        return score * multiplier;
      }
      score = 1;
      multiplier = 1;
    }
  }
  // If we didn't make a run
  if (score < 3) {
    return 0;
  }
  return score * multiplier;
}

size_t scorePairs(size_t nums[], size_t length) {
  sort(nums, nums + length);
  size_t group1 = 1;
  size_t group2 = 1;
  bool nextGroup = false;

  for (size_t i = 0; i < length - 1; ++i) {
    if (nums[i] == nums[i + 1]) {
      if (nextGroup) {
        ++group2;
      } else {
        ++group1;
      }
    } else {
      // If we've already made at least a pair, start the next group
      if (group1 > 1) {
        nextGroup = true;
      }
    }
  }

  // Convert number of cards in the set to points
  if (group1 > 2) {
    if (group1 == 4) {
      group1 = 12;
    } else {
      group1 = 6;
    }
  }
  if (group1 == 1) {
    group1 = 0;
  }

  if (group2 > 2) {
    if (group2 == 4) {
      group2 = 12;
    } else {
      group2 = 6;
    }
  }
  if (group2 == 1) {
    group2 = 0;
  }
  return group1 + group2;
}

void printArray(size_t nums[], size_t length) {
  cout << "[";

  for (size_t i = 0; i < length; ++i) {
    cout << nums[i];
    if (i != length - 1) {
      cout << ",";
    }
  }
  cout << "]" << endl;
}

void getPermutsHelper(size_t*& hand, size_t*& chosen, size_t length,
                      size_t chosenIndex, size_t getFromIndex) {
  if (chosenIndex == 4) {
    printArray(chosen, 4);
  } else {
    for (size_t i = getFromIndex; i < 9 - length; ++i) {
      chosen[chosenIndex] = hand[i];
      getPermutsHelper(hand, chosen, length - 1, chosenIndex + 1, i + 1);
    }
  }
}

void getPermuts(size_t*& hand, size_t*& chosen, size_t length) {
  getPermutsHelper(hand, chosen, length, 0, 0);
}

int main() {
  // size_t* hand = new size_t[6] {1,2,3,4,5,6,};
  // size_t* chosen = new size_t[4] {0,0,0,0};
  // getPermuts(hand, chosen, 6);

  size_t* nums = new size_t[5]{5, 6, 9, 10, 10};
  cout << scoreFifteens(nums, 5) << endl;

  return 0;
}