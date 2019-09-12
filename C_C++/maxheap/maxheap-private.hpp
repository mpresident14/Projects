#include <cmath>
#include <cstddef>
#include <iostream>
#include <iterator>

using namespace std;

template <typename T>
MaxHeap<T>::MaxHeap()
    : arr_{new T*[ORIGINAL_CAPACITY]}, capacity_{ORIGINAL_CAPACITY}, size_{0} {
  // Nothing to do
}

template <typename T>
MaxHeap<T>::MaxHeap(T array[], size_t length)
    : arr_{new T*[length * 2]}, capacity_{length * 2}, size_{0} {
  // Insert each element into the maxheap
  for (size_t i = 0; i < length; ++i) {
    insert(array[i]);
  }
}

template <typename T>
MaxHeap<T>::~MaxHeap() {
  // Delete each element
  for (size_t i = 0; i < size_; ++i) {
    delete arr_[i];
  }
  // Delete array of pointers
  delete[] arr_;
}

template <typename T>
T& MaxHeap<T>::findMax() const {
  return *(arr_[0]);
}

template <typename T>
void MaxHeap<T>::insert(const T& element) {
  // Resize if array is full
  if (size_ == capacity_) {
    resizeArray();
  }
  // Set last element in array to the new element
  arr_[size_] = new T{element};
  ++size_;
  // Swap with parents up to the appropriate location
  bubbleUp(size_ - 1);
}

template <typename T>
void MaxHeap<T>::resizeArray() {
  // Double the capacity of the array
  capacity_ *= 2;
  T** newArr = new T*[capacity_];
  for (size_t i = 0; i < size_; ++i) {
    newArr[i] = arr_[i];
  }
  // Delete the old array and reassign the data memberz
  delete[] arr_;
  arr_ = newArr;
}

template <typename T>
void MaxHeap<T>::bubbleUp(size_t index) {
  // Duplicate values okay
  // Stop if at the root or if child is less than parent
  if (index != 0 && *(arr_[(index - 1) / 2]) < *(arr_[index])) {
    // Swap pointer to child with pointer to parent
    T* temp = arr_[(index - 1) / 2];
    arr_[(index - 1) / 2] = arr_[index];
    arr_[index] = temp;
    bubbleUp((index - 1) / 2);
  }
}

template <typename T>
bool MaxHeap<T>::deleteAll(const T& element) {
  bool exists = deleteElement(element);
  while (deleteElement(element)) {
    // Do nothing
  }
  return exists;
}

template <typename T>
bool MaxHeap<T>::deleteElement(const T& element) {
  if (size_ == 0) {
    return false;
  }
  return deleteElementHelper(element, 0);
}

template <typename T>
bool MaxHeap<T>::deleteElementHelper(const T& element, size_t index) {
  T*& parent = arr_[index];
  size_t leftIndex = 2 * index + 1;
  size_t rightIndex = 2 * index + 2;
  // If we find the element
  if (*parent == element) {
    // Delete root and replace with last node
    delete arr_[index];
    arr_[index] = arr_[size_ - 1];
    arr_[size_ - 1] = nullptr;
    // Move the node down to the correct position
    --size_;
    bubbleDown(index);
    return true;
  }
  // Stop searching if we reach a node that is smaller than the one
  // for which we are searching or if we reach a leaf
  else if (*parent < element) {
    return false;
  }
  // Node with only a left child or no children
  else if (rightIndex >= size_) {
    if (leftIndex < size_) {
      return deleteElementHelper(element, leftIndex);
    }
    return false;
  }
  // Node with 2 children
  else {
    return deleteElementHelper(element, leftIndex) ||
           deleteElementHelper(element, rightIndex);
  }
}

template <typename T>
void MaxHeap<T>::deleteMax() {
  // Delete root and replace with last node
  delete arr_[0];
  arr_[0] = arr_[size_ - 1];
  arr_[size_ - 1] = nullptr;
  // Move the node down to the correct position
  --size_;
  bubbleDown(0);
}

template <typename T>
void MaxHeap<T>::bubbleDown(size_t index) {
  T*& parent = arr_[index];
  T*& leftChild = arr_[2 * index + 1];
  T*& rightChild = arr_[2 * index + 2];
  size_t childIndex = 0;

  // If node is a leaf, can't go down any farther
  if (2 * index + 1 >= size_ && 2 * index + 2 >= size_) {
    return;
  }
  // If node only has left child, check if they need to be swapped
  else if (2 * index + 2 >= size_) {
    if (*parent < *leftChild) {
      swapParentWithChild(index, true);
    }
  } else {
    // Figure out if left or right child is larger and get its index
    bool left = true;
    T*& largerChild = *leftChild < *rightChild ? rightChild : leftChild;
    if (largerChild == leftChild) {
      childIndex = 2 * index + 1;
    } else {
      childIndex = 2 * index + 2;
      left = false;
    }
    // Swap parent with larger child if either child is larger than parent
    if (*parent < *largerChild) {
      swapParentWithChild(index, left);
      bubbleDown(childIndex);
    }
  }
}

template <typename T>
void MaxHeap<T>::replaceMax(const T& element) {
  if (size_ == 0) {
    return;
  }
  delete arr_[0];
  arr_[0] = new T{element};
  bubbleDown(0);
}

template <typename T>
bool MaxHeap<T>::replaceAll(const T& toDelete, const T& toInsert) {
  bool exists = replaceElement(toDelete, toInsert);
  while (replaceElement(toDelete, toInsert)) {
    // Do nothing
  }
  return exists;
}

template <typename T>
bool MaxHeap<T>::replaceElement(const T& toDelete, const T& toInsert) {
  if (size_ == 0) {
    return false;
  }
  return replaceElementHelper(0, toDelete, toInsert);
}

template <typename T>
bool MaxHeap<T>::replaceElementHelper(size_t index, const T& toDelete,
                                      const T& toInsert) {
  T*& parent = arr_[index];
  size_t leftIndex = 2 * index + 1;
  size_t rightIndex = 2 * index + 2;
  // If we find the element
  if (*parent == toDelete) {
    // Delete current node and replace with the node to insert
    delete arr_[index];
    arr_[index] = new T{toInsert};
    // Move the node down to the correct position
    if (toInsert < toDelete) {
      bubbleDown(index);
    } else {
      bubbleUp(index);
    }
    return true;
  }
  // Stop searching if we reach a node that is smaller than the one
  // for which we are searching or if we reach a leaf
  else if (*parent < toDelete) {
    return false;
  }
  // Node with only a left child or no children
  else if (rightIndex >= size_) {
    if (leftIndex < size_) {
      return replaceElementHelper(leftIndex, toDelete, toInsert);
    }
    return false;
  }
  // Node with 2 children
  else {
    return replaceElementHelper(leftIndex, toDelete, toInsert) ||
           replaceElementHelper(rightIndex, toDelete, toInsert);
  }
}

template <typename T>
bool MaxHeap<T>::exists(const T& element) {
  if (size_ == 0) {
    return false;
  }
  return existsHelper(element, 0);
}

template <typename T>
bool MaxHeap<T>::existsHelper(const T& element, size_t index) {
  T*& parent = arr_[index];
  size_t leftIndex = 2 * index + 1;
  size_t rightIndex = 2 * index + 2;
  // If we find the element
  if (*parent == element) {
    return true;
  }
  // Stop searching if we reach a node that is smaller than the one
  // for which we are searching or if we reach a leaf
  else if (*parent < element) {
    return false;
  }
  // Node with only a left child or no children
  else if (rightIndex >= size_) {
    if (leftIndex < size_) {
      return existsHelper(element, leftIndex);
    }
    return false;
  }
  // Node with 2 children
  else {
    return existsHelper(element, leftIndex) ||
           existsHelper(element, rightIndex);
  }
}

template <typename T>
ostream& MaxHeap<T>::print(std::ostream& out) const {
  return printHelper(0, out, 0);
}

template <typename T>
ostream& MaxHeap<T>::printHelper(size_t index, std::ostream& out,
                                 size_t level) const {
  if (index < size_) {
    for (size_t i = 0; i < level; ++i) {
      out << "   ";
    }
    out << "~ ";
    out << *(arr_[index]) << '\n';
    printHelper(2 * index + 1, out, level + 1);
    printHelper(2 * index + 2, out, level + 1);
  }
  return out;
}

template <typename T>
void MaxHeap<T>::swapParentWithChild(size_t parentIndex, bool left) {
  size_t childIndex = (left) ? 2 * parentIndex + 1 : 2 * parentIndex + 2;
  T* temp = arr_[childIndex];
  arr_[childIndex] = arr_[parentIndex];
  arr_[parentIndex] = temp;
}

template <typename T>
size_t MaxHeap<T>::size() const {
  return size_;
}

template <typename T>
bool MaxHeap<T>::isEmpty() const {
  return size_ == 0;
}
