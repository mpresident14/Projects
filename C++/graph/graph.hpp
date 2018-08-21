#ifndef GRAPH_HPP
#define GRAPH_HPP 1

#include <unordered_map>
#include <unordered_set>
#include <initializer_list>
#include <iostream>
#include <cstddef>
#include <forward_list>
#include <functional>

template <typename T, bool(*F)(const T&, const T&)>
class Graph {
  public:
    Graph();
    ~Graph() = default;
    Graph(const Graph& other);
    Graph(Graph&& other);
    Graph& operator=(Graph other);
    void swap(Graph& second);
    /**
     * @brief Adds a vertex to the graph. 
     * @details Edges are constructed based on the template function.
     * 
     * @param item the item to be inserted into the graph
     * @return true if the item was inserted
     * @return false if the item was already in the graph
     */
    bool addVertex(const T& item);
    bool addVertex(T&& item);    
    /**
     * @brief Removes a vertex to the graph. 
     * 
     * @param item the item to be removed from the graph
     * @return true if the item was removed
     * @return false if the item was not in the graph
     */
    bool removeVertex(const T& item);
    /**
     * @brief Retrieves a set of vertices adjacent to a particular item
     * 
     * @param item the item for which to retrieve adjacent vertices
     * @return const std::unordered_set<T>* vertices in the graph that are adjacent to item.
     * @return nullptr if item is not in the graph
     */
    const std::unordered_set<T>* getRelatives(const T& item);
    /**
     * @brief Returns the number of vertices adjacent to a particular item
     * 
     * @param item the item for which to get the number of adjacent vertices
     * @return int number of vertices adjacent to item
     * @return -1 if item is not in the graph
     */
    int getRelativeCount(const T& item);
    /**
     * @brief Return existence of item in graph.
     * 
     * @param item the item to search for
     * @return true if the item is in the graph
     * @return false otherwise
     */
    bool contains(const T& item);
    /**
     * @brief Find the shortest path between two items
     * 
     * @param begin the starting vertex
     * @param end the ending vertex
     * @return std::forward_list<T> a list starting with start and ending with end that indicating
     *  the shortest path between the two. If there are multiple paths of the shortest length, any
     *  of them can be returned. Returns empty list if there is no path. 
     * @throws invalid_argument if begin or end are not in the graph
     */
    std::forward_list<T> getShortestPath(const T& begin, const T& end);
    /**
     * @brief Returns number of vertices in the graph
     * 
     * @return size_t the number of vertices in the graph
     */
    size_t size();
  
  private:
    std::unordered_map<T, std::unordered_set<T>> relativeMap_;
};

#include "graph_private.hpp"

#endif