#ifndef GRAPH_HPP
#define GRAPH_HPP 1

#include <unordered_map>
#include <unordered_set>
#include <initializer_list>
#include <iostream>
#include <cstddef>
#include <forward_list>
#include <functional>

template <typename T, bool(*F)(const T&, const T&)=nullptr>
class Graph {
  private:
    template <bool is_const>
    class Iterator;

  public:
    // Standard STL container type definitions
    using value_type        = T;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using iterator          = Iterator<false>;
    using const_iterator    = Iterator<true>;

    Graph();
    ~Graph() = default;
    Graph(const Graph& other);
    Graph(Graph&& other);
    Graph& operator=(Graph other);
    void swap(Graph& second);
    
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    /**
     * @brief Adds a vertex to the graph. 
     * @details Edges are constructed based on the template function. If no template function is 
     *  specified, then the inserted vertex will have no adjacent vertices.
     * 
     * @param item the item to be inserted into the graph
     * @return true if the item was inserted
     * @return false if the item was already in the graph
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
     */
    bool addEdge(const T& first, const T& second); 
    /**
     * @brief Removes a vertex to the graph. 
     * 
     * @param item the item to be removed from the graph
     * @return true if the item was removed
     * @return false if the item was not in the graph
     */
    bool removeVertex(const T& item);
    /**
     * @brief Removes an edge between two vertices
     * 
     * @param first the first vertex to be disconnected
     * @param second the second vertex to be disconnected
     * @return true if edge removed
     * @return false if edge did not exist or first or second is not in the graph
     */
    bool removeEdge(const T& first, const T& second); 
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
     * @brief Determines whether two vertices are adjacent.
     * 
     * @param first the first vertex
     * @param second the second vertex
     * @return true if the two vertices are adjacent
     * @return false if two verticecs are not adjacent or if either vertex is not in the graph
     */
    bool adjacent(const T& first, const T& second);
    /**
     * @brief Return existence of item in graph.
     * 
     * @param item the item to search for
     * @return true if the item is in the graph
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
  
  private:
    std::unordered_map<T, std::unordered_set<T>> relativeMap_;

    template <bool is_const>
    class Iterator {
      public:
        Iterator() = default;
        Iterator(const Iterator<false>& iter);

        // Make Iterator STL-friendly with these typedefs:
        using difference_type   = ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = T;
        using const_reference   = const value_type&;
        
        using reference = typename std::conditional<is_const, 
                                                    const value_type&, 
                                                    value_type&>::type;
        
        using pointer = typename std::conditional<is_const, 
                                                  const value_type*, 
                                                  value_type*>::type;
        
        using map_iter_t = typename std::conditional<is_const,
                                typename std::unordered_map<value_type, std::unordered_set<value_type>>::const_iterator,
                                typename std::unordered_map<value_type, std::unordered_set<value_type>>::iterator>::type;

        Iterator(map_iter_t mapIter);

        Iterator& operator++();
        Iterator& operator--();
        reference operator*() const;
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;

      private:
        friend class Graph;
        map_iter_t mapIter_; ///< Using the relativeMap_ iterator  
    };
};

#include "graph_private.hpp"

#endif