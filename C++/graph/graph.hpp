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
    bool addVertex(const T& item);
    bool addVertex(T&& item);
    // TODO: If relatives not in graph, then add to graph ???
    void addVertex(const T& item, std::initializer_list<T> relatives);
    void addVertex(T&& item, std::initializer_list<T> relatives);
    // TODO: Fix this, needs to remove item from relative sets of other vertices
    bool removeVertex(const T& item);
    const std::unordered_set<T>* getRelatives(const T& item);
    int getRelativeCount(const T& item);
    bool contains(const T& item);
    std::forward_list<T> getShortestPath(const T& begin, const T& end);

    void swap(Graph& second);
    size_t numVertices();
  
  private:
    std::unordered_map<T, std::unordered_set<T>> relativeMap_;
    size_t numVertices_;
};

#include "graph_private.hpp"

#endif