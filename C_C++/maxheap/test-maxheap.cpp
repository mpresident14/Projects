#include <time.h>
#include <algorithm>
#include <random>
#include "maxheap.hpp"
#include "testing-program.hpp"

using namespace std;

// Make sure all MaxHeap functions work for with required operators
// This sample Cow class is modified from a Harvey Mudd College CS70 assignment
class Cow {
 public:
  /// Non-default constructor.
  // (Classes used with hash tables aren't required to have a default
  //  constructor.)
  explicit Cow(size_t numHats);
  void setNumHats(size_t num) { numHats_ = num; }

 private:
  size_t numHats_;  ///< How many hats does our Cow have?

  // Give external functions friend access
  friend std::ostream& operator<<(std::ostream& out, const Cow& printMe);
  friend bool operator<(const Cow& lhs, const Cow& rhs);
  friend bool operator==(const Cow& lhs, const Cow& rhs);
};

/// Inequality for Cows
bool operator<(const Cow& lhs, const Cow& rhs) {
  return lhs.numHats_ < rhs.numHats_;
}

/// Equality for Cows
bool operator==(const Cow& lhs, const Cow& rhs) {
  return lhs.numHats_ == rhs.numHats_;
}

/// Output a Cow in visual form
std::ostream& operator<<(std::ostream& out, const Cow& printMe) {
  out << printMe.numHats_;
  return out;
}

Cow::Cow(size_t numHats) : numHats_(numHats) {
  // Nothing (else) to do.
}

template class MaxHeap<Cow>;

/***************************
 * Tests for functionality *
 ***************************/
void testDefConstructor() {
  TestingProgram tester{"Default Constructor test"};

  MaxHeap<int> h;
  affirm(h.size() == 0);

  TestingProgram::printResults();
}

void testParConstructor() {
  TestingProgram tester{"Parameterized Constructor test"};

  // Set up array of ints
  size_t numElements = rand() % 50;
  size_t* array = new size_t[numElements];
  for (size_t i = 0; i < numElements; ++i) {
    array[i] = i;
  }

  // Affirm that we extract them greatest to least
  MaxHeap<size_t> h{array, numElements};
  affirm(h.size() == numElements);
  for (int i = numElements - 1; i >= 0; --i) {
    affirm(h.findMax() == (size_t)i);
    h.deleteMax();
  }

  delete[] array;
  TestingProgram::printResults();
}

void testInsertandExtractMax() {
  TestingProgram tester{"Insert/ExtractMax test"};

  MaxHeap<size_t> h;
  size_t numElements = rand() % 50;
  size_t* array = new size_t[numElements];

  // Insert random elements into heap and array
  for (size_t i = 0; i < numElements; ++i) {
    affirm(h.size() == i);
    size_t n = rand() % 1000;
    h.insert(n);
    array[i] = n;
  }
  // Sort the array
  sort(array, array + numElements);

  // Extracting the max should be like iterating backwards
  // through the array
  for (int i = numElements - 1; i >= 0; --i) {
    affirm(h.findMax() == array[i]);
    h.deleteMax();
  }

  delete[] array;
  TestingProgram::printResults();
}

void testFindMax() {
  TestingProgram tester{"FindMax test"};

  MaxHeap<size_t> h;
  size_t numElements = rand() % 50;
  size_t* array = new size_t[numElements];

  // Insert random elements into heap and array
  for (size_t i = 0; i < numElements; ++i) {
    affirm(h.size() == i);
    size_t n = rand() % 1000;
    h.insert(n);
    array[i] = n;
  }
  // Sort the array
  sort(array, array + numElements);

  // Max should be last element in array
  affirm(h.findMax() == array[numElements - 1]);

  // Make sure it works with multiple maxima
  h.insert(h.findMax());
  affirm(h.findMax() == array[numElements - 1]);

  delete[] array;
  TestingProgram::printResults();
}

void testExists() {
  TestingProgram tester{"Exists test"};

  MaxHeap<size_t> h;
  affirm(!h.exists(29));

  size_t numElements = rand() % 50;

  h.insert(76);
  // Insert random elements into heap and array
  // and affirm existence
  for (size_t i = 0; i < numElements; ++i) {
    size_t n = rand() % 1000;
    h.insert(n);
    affirm(h.exists(n));
  }
  affirm(h.exists(76));

  TestingProgram::printResults();
}

void testDeleteMax() {
  TestingProgram tester{"DeleteMax test"};

  MaxHeap<size_t> h;
  size_t numElements = rand() % 50;
  size_t* array = new size_t[numElements];

  // Insert random elements into heap and array
  for (size_t i = 0; i < numElements; ++i) {
    affirm(h.size() == i);
    size_t n = rand() % 1000;
    h.insert(n);
    array[i] = n;
  }
  // Sort the array
  sort(array, array + numElements);

  // Extracting the max should be like iterating backwards
  // through the array
  for (int i = numElements - 1; i >= 0; --i) {
    affirm(h.findMax() == array[i]);
    h.deleteMax();
  }

  delete[] array;
  TestingProgram::printResults();
}

void testDeleteElement() {
  TestingProgram tester{"DeleteElement test"};

  // Delete from empty MaxHeap
  MaxHeap<size_t> h1;
  affirm(!h1.deleteElement(0));

  // Delete from not empty MaxHeap
  int array[10] = {4, 6, 3, -1, -63, 2, 22, 88, 3, 3};
  MaxHeap<int> h{array, 10};
  affirm(!h.deleteElement(0));
  affirm(!h.deleteElement(-7));
  affirm(!h.deleteElement(100));

  for (int i = 0; i < 10; ++i) {
    affirm(h.size() == (size_t)10 - i);
    affirm(h.deleteElement(array[i]));
  }

  TestingProgram::printResults();
}

void testDeleteAll() {
  TestingProgram tester{"DeleteAll test"};

  // Delete from empty MaxHeap
  MaxHeap<size_t> h1;
  affirm(!h1.deleteAll(0));

  // Delete from not empty MaxHeap
  int array[10] = {8, 33, -90, 37, -34, -1, 8, 5, 66, 8};
  MaxHeap<int> h{array, 10};
  affirm(!h.deleteAll(4));
  affirm(!h.deleteAll(-73));
  affirm(!h.deleteAll(34));

  affirm(h.deleteAll(8));
  affirm(h.size() == 7);
  affirm(!h.exists(8));

  TestingProgram::printResults();
}

void testReplaceMax() {
  TestingProgram tester{"ReplaceMax test"};

  // ReplaceMax from empty MaxHeap shouldn't do anything
  MaxHeap<size_t> h1;
  h1.replaceMax(0);

  // Replace from not empty MaxHeap
  int array[10] = {3, 4, 6, -34, 8, -1, 9, 30, -5, -48};
  MaxHeap<int> h{array, 10};

  h.replaceMax(60);
  affirm(h.findMax() == 60);

  h.replaceMax(-7);
  affirm(h.findMax() == 9);
  affirm(h.exists(-7));

  h.replaceMax(-23);
  affirm(h.findMax() == 8);
  affirm(h.exists(-23));

  TestingProgram::printResults();
}

void testReplaceElement() {
  TestingProgram tester{"ReplaceElement test"};

  // ReplaceElement from empty MaxHeap should
  MaxHeap<size_t> h1;
  affirm(!h1.replaceElement(0, 9));

  // Replace from not empty MaxHeap
  int array[10] = {-3, 29, 55, 2, -3, -3, 38, 4, 54, 7};
  MaxHeap<int> h{array, 10};

  // toDelete not in MaxHeap
  affirm(!h.replaceElement(60, 47));

  affirm(h.replaceElement(55, 77));
  affirm(h.findMax() == 77);

  affirm(h.replaceElement(77, 3));
  affirm(h.findMax() == 54);
  affirm(h.exists(3));

  affirm(h.replaceElement(-3, 4));
  affirm(h.findMax() == 54);
  affirm(h.exists(4));

  affirm(h.size() == 10);

  TestingProgram::printResults();
}

void testReplaceAll() {
  TestingProgram tester{"ReplaceAll test"};

  // ReplaceAll from empty MaxHeap should
  MaxHeap<size_t> h1;
  affirm(!h1.replaceAll(0, 9));

  // Replace from not empty MaxHeap
  int array[10] = {-3, 29, 55, 2, -3, -3, 38, 4, 54, 7};
  MaxHeap<int> h{array, 10};

  // toDelete not in MaxHeap
  affirm(!h.replaceElement(60, 47));

  affirm(h.replaceElement(-3, 77));
  affirm(h.findMax() == 77);

  affirm(h.replaceElement(77, 1));
  affirm(h.findMax() == 55);
  affirm(h.exists(1));

  affirm(h.size() == 10);

  TestingProgram::printResults();
}

void testIsEmpty() {
  TestingProgram tester{"IsEmpty test"};

  MaxHeap<size_t> h;
  affirm(h.isEmpty());

  h.insert(7);
  affirm(!h.isEmpty());

  TestingProgram::printResults();
}

int main() {
  srand((size_t)time(NULL));

  testDefConstructor();
  testParConstructor();
  testInsertandExtractMax();
  testFindMax();
  testExists();
  testDeleteMax();
  testDeleteElement();
  testDeleteAll();
  testReplaceMax();
  testReplaceElement();
  testReplaceAll();
  testIsEmpty();

  TestingProgram::summarize();

  return 0;
}
