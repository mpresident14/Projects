#ifndef MAXHEAP_HPP_INCLUDED
#define MAXHEAP_HPP_INCLUDED 1

#include <iostream> 
#include <cmath>

// cd c:\Users\Miles\Desktop\MaxHeap\maxheap

/**
 * @file maxheap.hpp
 *
 * @author Miles President
 *
 * @brief Provides MaxHeap<T>
 * @note Type T requires < and == operators and default copy constructor
 */

/**
 * @class MaxHeap
 * @brief Implementation of a max heap binary tree
 * 
 */
template<typename T>
class MaxHeap{
    public:
        /**
         * @brief Default Constructor
         * @details Creates an empty MaxHeap
         * 
         */
        MaxHeap();
        /**
         * @brief Parameterized Constructor
         * @details Creates a MaxHeap from elements of an array
         * 
         * @param array A pointer to an array
         * @param length The length of the array
         */
        MaxHeap(T array[], size_t length);
        MaxHeap(const MaxHeap& otherMaxHeap) = delete;
        /**
         * @brief Default Destructor
         * 
         */
        ~MaxHeap();
        MaxHeap& operator=(const MaxHeap& otherMaxHeap) = delete;
        /**
         * @brief Insert an element into the MaxHeap
         * 
         * @param element to insert
         */
        void insert(const T& element);
        /**
         * @brief Deletes an element from MaxHeap
         * 
         * @param element 
         * @return true if element is in MaxHeap
         * @note 
         *  If MaxHeap contains multiple occurrences of element,
         *  only one is deleted.
         */
        bool deleteElement(const T& element);
        /**
         * @brief Deletes all occurrences of an element from the MaxHeap
         * 
         * @param element Object to delete
         * @return true if element is in MaxHeap
         */
        bool deleteAll(const T& element);
        /**
         * @brief Delete the largest element in the MaxHeap
         * 
         */
        void deleteMax();
        /**
         * @brief Replace the largest element in the MaxHeap with another element
         * @details More efficient than deleting and then inserting because it
         *          only has to rebalance once
         * 
         * @param element Object to insert
         */
        void replaceMax(const T& element);
        /**
         * @brief Replace an element in the MaxHeap with another element
         * @details More efficient than deleting and then inserting because it
         *          only has to rebalance once
         * 
         * @param toDelete Object to delete
         * @param toInsert Object to insert
         */
        bool replaceElement(const T& toDelete, const T& toInsert);
        /**
         * @brief Replace all occurrences of an element in the MaxHeap with another element
         * @details More efficient than deleting and then inserting because it
         *          only has to rebalance once for each occurrence of toDelete
         * 
         * @param toDelete Object to delete
         * @param toInsert Object to insert
         * @note 
         *  If MaxHeap contains multiple occurrences of toDelete,
         *  only one is deleted and replaced.
         */
        bool replaceAll(const T& toDelete, const T& toInsert);
        /**
         * @brief Checks whether an element exists in the MaxHeap
         * 
         * @param element Element to search for
         * @return true if element is in the MaxHeap
         */
        bool exists(const T& element);
        /**
         * @brief Return, but do not delete the largest element in the MaxHeap
         * 
         * @return The largest element in the Heap
         * @warning calling findMax on an empty MaxHeap is undefined behavior
         */
        T& findMax() const;
        /**
         * @brief Returns the number of elements in the MaxHeap
         * 
         * @return The number of elements in the MaxHeap
         */
        size_t size() const;
        /**
         * @brief Checks whether the MaxHeap has any elements
         * 
         * @return true if the MaxHeap is empty
         */
        bool isEmpty() const;
        std::ostream& print(std::ostream& out) const; ///< To print the MaxHeap
        
    private:
        static const size_t ORIGINAL_CAPACITY = 4; ///< Starting size of array data member

        T** arr_; ///< Array of pointers storing the elements in the MaxHeap
        size_t capacity_; ///< Capacity of the array
        size_t size_; ///< Number of elements in MaxHeaps
        /**
         * @brief Doubles the capacity of the array data member
         * 
         */
        void resizeArray();
        /**
         * @brief Brings a larger element up to the correct position
         * 
         * @param index Index of element in array
         */
        void bubbleUp(size_t index);
        /**
         * @brief Brings a smaller element down to the correct position
         * 
         * @param index Index of element in array
         */
        void bubbleDown(size_t index);
        /**
         * @brief Recursive helper for deleteElement
         * 
         * @param element Element to delete
         * @param index Index of element in array
         * 
         */
        bool deleteElementHelper(const T& element, size_t index);
        /**
         * @brief Recursive helper for replaceElement
         * 
         * @param index Index of element in array
         * @param toDelete Element to delete
         * @param toInsert Element to insert
         * @return true if element is in the MaxHeap 
         */
        bool replaceElementHelper(size_t index, const T& toDelete, const T& toInsert);
        /**
         * @brief Recursive helper for exists
         * 
         * @param element Element to search for
         * @param index Index of element in array
         * @return true if element is in the MaxHeap 
         */
        bool existsHelper(const T& element, size_t index);
        /**
         * @brief Helper function to swap a parent node with its child
         * 
         * @param parentIndex Index of parent in array
         * @param left true for swap with left child,
         *             false for swap with right child
         */
        void swapParentWithChild(size_t parentIndex, bool left);
        /**
         * @brief Recursive Helper for string printing
         * 
         * @param index Index of parent in array
         * @param out ostream to send it to
         * @param level The depth of the current Node
         */
        std::ostream& printHelper(size_t index, std::ostream& out, size_t level) const;
};

template<typename T>
///< Operator to print a MaxHeap
inline std::ostream& operator<<(std::ostream& out, const MaxHeap<T>& h)
{
    return h.print(out);
}

#include "maxheap-private.hpp"

#endif
