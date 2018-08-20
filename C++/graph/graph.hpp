#ifndef GRAPH_HPP
#define GRAPH_HPP 1

#include <unordered_map>
#include <unordered_set>
#include <initializer_list>
#include <iostream>
#include <cstddef>

template <typename T>
class Graph {
  public:
    Graph();
    ~Graph() = default;
    Graph(const Graph& other);
    Graph(Graph&& other);
    Graph<T>& operator=(Graph other);
    void addVertex(const T& item, std::initializer_list<T> relatives={});
    void addVertex(T&& item, std::initializer_list<T> relatives={});
    bool removeVertex(const T& item);
    const std::unordered_set<T>* getRelatives(const T& item);
    bool contains(const T& item);

    void swap(Graph& second);
    size_t numVertices();
  
  private:
    std::unordered_map<T, std::unordered_set<T>> relativeMap_;
    size_t numVertices_;

};

#include "graph_private.hpp"

#endif