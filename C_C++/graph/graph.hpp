#ifndef GRAPH_HPP
#define GRAPH_HPP 1

#include <cstdarg>
#include <cstddef>
#include <forward_list>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

template <typename T, bool (*F)(const T&, const T&) = nullptr>
class Graph {
 private:
  template <bool is_const>
  class Iterator;

 public:
  // Standard STL container type definitions
  using value_type = T;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = Iterator<false>;
  using const_iterator = Iterator<true>;

  Graph();
  ~Graph() = default;
  Graph(const Graph& other);
  Graph(Graph&& other);
  Graph& operator=(Graph other);
  void swap(Graph& second);

  // Iterators iterate over vertices in no particular order
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  /**
   * @brief Adds a vertex to the graph.
   * @details Edges are constructed based on the template function. If no
   * template function is specified, then the inserted vertex will have no
   * adjacent vertices.
   *
   * @param item the item to be inserted into the graph
   * @return true if the item was inserted
   * @return false if the item was already in the graph
   *
   * @note O(V) if F is specified in template. Otherwise O(1);
   */
  bool addVertex(const T& item);
  bool addVertex(T&& item);
  /**
   * @brief Adds an edge between two vertices
   *
   * @param first the first vertex to be connected
   * @param second the second vertex to be connected
   * @return true if edge added
   * @return false if edge already exists or first or second is not in the graph
   *
   * @note O(1)
   */
  bool addEdge(const T& first, const T& second);
  /**
   * @brief Removes a vertex and all its edges from the graph.
   *
   * @param item the item to be removed from the graph
   * @return true if the item was removed
   * @return false if the item was not in the graph
   *
   * @note O(V)
   */
  bool removeVertex(const T& item);
  /**
   * @brief Removes an edge between two vertices
   *
   * @param first the first vertex to be disconnected
   * @param second the second vertex to be disconnected
   * @return true if edge removed
   * @return false if edge did not exist or first or second is not in the graph
   *
   * @note O(1)
   */
  bool removeEdge(const T& first, const T& second);
  /**
   * @brief Retrieves a set of vertices adjacent to a particular item
   *
   * @param item the item for which to retrieve adjacent vertices
   * @return const std::unordered_set<T>* vertices in the graph that are
   * adjacent to item.
   * @return nullptr if item is not in the graph
   *
   * @note O(1)
   */
  const std::unordered_set<T>* getRelatives(const T& item);
  /**
   * @brief Returns the number of vertices adjacent to a particular item
   *
   * @param item the item for which to get the number of adjacent vertices
   * @return int number of vertices adjacent to item
   * @return -1 if item is not in the graph
   *
   * @note O(1)
   */
  int getRelativeCount(const T& item);
  /**
   * @brief Determines whether two vertices are adjacent.
   *
   * @param first the first vertex
   * @param second the second vertex
   * @return true if the two vertices are adjacent
   * @return false if two verticecs are not adjacent or if either vertex is not
   * in the graph
   *
   * @note O(1)
   */
  bool adjacent(const T& first, const T& second);
  /**
   * @brief Return existence of item in graph.
   *
   * @param item the item to search for
   * @return true if the item is in the graph
   *
   * @note O(1)
   */
  bool contains(const T& item);
  /**
   * @brief Find the shortest path between two items
   *
   * @param start the starting vertex
   * @param finish the ending vertex
   * @return std::forward_list<T> a list starting with start and ending with
   * finish that indicating the shortest path between the two. If there are
   * multiple paths of the shortest length, any of them can be returned. Returns
   * empty list if there is no path or if begin or end are not in the graph.
   *
   * @note Worst case O(V)
   */
  template <typename... Args>
  std::forward_list<T> getShortestPath(const T& start, const T& finish);
  /**
   * @brief Find the shortest path between two items where all nodes in between
   * start and finish meet a condition specified in fcn
   *
   * @param start the starting vertex
   * @param finish the ending vertex
   * @param fcn a function specifying a condition that all nodes on the path
   * between start and finish must meet
   * @return std::forward_list<T> a list starting with start and ending with
   * finish indicating the shortest path between the two. If there are multiple
   * paths of the shortest length, any of them can be returned. Returns empty
   * list if there is no path or if begin or end are not in the graph.
   *
   * @note Worst case O(V)
   */
  template <typename... Args, typename... Arg2s>
  std::forward_list<T> getShortestPath(const T& start, const T& finish,
                                       bool (*fcn)(const T& item, Args...),
                                       Arg2s... args);
  /**
   * @brief Returns number of vertices in the graph
   *
   * @return size_t the number of vertices in the graph
   *
   * @note O(1)
   */
  size_t size();
  /**
   * @brief Returns number of edges in the graph
   *
   * @return size_t the number of edges in the graph
   *
   * @note O(V)
   */
  size_t numEdges();

  friend inline std::ostream& operator<<(std::ostream& out,
                                         const Graph& graph) {
    for (auto& pair : graph.relativeMap_) {
      out << pair.first << ", ";
    }
    return out;
  }

 private:
  std::unordered_map<T, std::unordered_set<T>> relativeMap_;

  template <bool is_const>
  class Iterator {
   public:
    Iterator() = default;
    Iterator(const Iterator<false>& iter);

    // Make Iterator STL-friendly with these typedefs:
    using difference_type = ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<const T, std::unordered_set<T>>;
    using const_reference = const value_type&;

    using reference = typename std::conditional<is_const, const value_type&,
                                                value_type&>::type;

    using pointer = typename std::conditional<is_const, const value_type*,
                                              value_type*>::type;

    using map_iter_t = typename std::conditional<
        is_const,
        typename std::unordered_map<T, std::unordered_set<T>>::const_iterator,
        typename std::unordered_map<T, std::unordered_set<T>>::iterator>::type;

    Iterator(map_iter_t mapIter);

    Iterator& operator++();
    reference operator*() const;
    pointer operator->() const;
    bool operator==(const Iterator& other) const;
    bool operator!=(const Iterator& other) const;

   private:
    friend class Graph;
    map_iter_t mapIter_;  ///< Using the relativeMap_ iterator
  };
};

#include "graph_private.hpp"

#endif