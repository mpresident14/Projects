#include <cstdarg>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using namespace std;

template <typename T, bool (*F)(const T&, const T&)>
Graph<T, F>::Graph() {
  // Nothing to do
}

template <typename T, bool (*F)(const T&, const T&)>
Graph<T, F>::Graph(const Graph<T, F>& other)
    : relativeMap_{other.relativeMap_} {
  // Nothing to do
}

template <typename T, bool (*F)(const T&, const T&)>
Graph<T, F>::Graph(Graph<T, F>&& other) {
  swap(other);
}

template <typename T, bool (*F)(const T&, const T&)>
Graph<T, F>& Graph<T, F>::operator=(Graph<T, F> other) {
  swap(other);
  return *this;
}

template <typename T, bool (*F)(const T&, const T&)>
void Graph<T, F>::swap(Graph<T, F>& second) {
  relativeMap_.swap(second.relativeMap_);
}

template <typename T, bool (*F)(const T&, const T&)>
bool Graph<T, F>::addVertex(const T& item) {
  // Return if already in the graph
  if (relativeMap_.count(item) != 0) {
    return false;
  }

  // Insert the adjacency set for item
  relativeMap_.insert({item, unordered_set<T>()});

// Return if an adjacency function was not specified
// Suppress warning about F never being nullptr (checked and tested)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress"
  if (F == nullptr) {
    return true;
  }
#pragma GCC diagnostic pop

  // If an adj fcn was specified, insert item into the other sets
  unordered_set<T>& relatives = relativeMap_.at(item);

  for (auto& pair : relativeMap_) {
    if (F(item, pair.first)) {
      pair.second.insert(item);
      relatives.insert(pair.first);
    }
  }

  return true;
}

template <typename T, bool (*F)(const T&, const T&)>
bool Graph<T, F>::addVertex(T&& item) {
  T&& rval = std::move(item);
  // Return if already in the graph
  if (relativeMap_.count(rval) != 0) {
    return false;
  }

  // Insert the adjacency set for item
  relativeMap_.insert({rval, unordered_set<T>()});

// Return if an adjacency function was not specified
// Suppress warning about F never being nullptr (checked and tested)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress"
  if (F == nullptr) {
    return true;
  }
#pragma GCC diagnostic pop

  // If an adj fcn was specified, insert item into the other sets
  unordered_set<T>& relatives = relativeMap_.at(rval);

  for (auto& pair : relativeMap_) {
    if (F(rval, pair.first)) {
      pair.second.insert(rval);
      relatives.insert(pair.first);
    }
  }

  return true;
}

template <typename T, bool (*F)(const T&, const T&)>
bool Graph<T, F>::addEdge(const T& first, const T& second) {
  // Check if first and second are in the graph
  auto firstIter = relativeMap_.find(first);
  auto secondIter = relativeMap_.find(second);

  if (firstIter == relativeMap_.end() || secondIter == relativeMap_.end()) {
    return false;
  }

  // Check if there is already an edge in first's set
  bool wasInserted = firstIter->second.insert(second).second;
  if (wasInserted) {
    secondIter->second.insert(first);
  }

  return wasInserted;
}

template <typename T, bool (*F)(const T&, const T&)>
bool Graph<T, F>::removeVertex(const T& item) {
  // If item is not in graph
  if (relativeMap_.erase(item) == 0) {
    return false;
  }

  // Remove item from other vertices adjacency sets
  for (auto& pair : relativeMap_) {
    pair.second.erase(item);
  }

  return true;
}

template <typename T, bool (*F)(const T&, const T&)>
bool Graph<T, F>::removeEdge(const T& first, const T& second) {
  // Check if first and second are in the graph
  auto firstIter = relativeMap_.find(first);
  auto secondIter = relativeMap_.find(second);

  if (firstIter == relativeMap_.end() || secondIter == relativeMap_.end()) {
    return false;
  }

  // Check if there is an edge in first's set and remove
  size_t numRemoved = firstIter->second.erase(second);
  if (numRemoved != 0) {
    secondIter->second.erase(first);
  }

  return numRemoved != 0;
}

template <typename T, bool (*F)(const T&, const T&)>
const unordered_set<T>* Graph<T, F>::getRelatives(const T& item) {
  auto iter = relativeMap_.find(item);
  return iter == relativeMap_.end() ? nullptr : &(iter->second);
}

template <typename T, bool (*F)(const T&, const T&)>
int Graph<T, F>::getRelativeCount(const T& item) {
  const unordered_set<T>* relatives = getRelatives(item);
  return relatives == nullptr ? -1 : relatives->size();
}

template <typename T, bool (*F)(const T&, const T&)>
bool Graph<T, F>::adjacent(const T& first, const T& second) {
  // Check if first and second are in the graph
  auto firstIter = relativeMap_.find(first);
  auto secondIter = relativeMap_.find(second);

  if (firstIter == relativeMap_.end() || secondIter == relativeMap_.end()) {
    return false;
  }

  // Check if second is in first's set
  return firstIter->second.count(second) != 0;
}

template <typename T, bool (*F)(const T&, const T&)>
bool Graph<T, F>::contains(const T& item) {
  return relativeMap_.count(item) != 0;
}

template <typename T, bool (*F)(const T&, const T&)>
template <typename... Args>
forward_list<T> Graph<T, F>::getShortestPath(const T& start, const T& finish) {
  bool (*fcn)(const T& item, Args...) = [](const T&, Args...) -> bool {
    return true;
  };
  return getShortestPath(start, finish, fcn);
}

template <typename T, bool (*F)(const T&, const T&)>
template <typename... Args, typename... Arg2s>
forward_list<T> Graph<T, F>::getShortestPath(const T& start, const T& finish,
                                             bool (*fcn)(const T& item,
                                                         Args...),
                                             Arg2s... args) {
  if (getRelativeCount(start) == -1 || getRelativeCount(finish) == -1) {
    return forward_list<T>();
  }

  if (getRelativeCount(start) == 0 || getRelativeCount(finish) == 0) {
    return forward_list<T>();
  }

  if (start == finish) {
    return forward_list<T>({start});
  }

  /*
   * Dynamic approach: example
   * a -> (b,c)
   * b -> (a,c,f)
   * c -> (a,b)
   * d -> (c,e)
   * e -> (d)
   * f -> (b)
   *
   * Done when queue is empty.
   * Map next item to previous item when we add to queue.
   * Check if visited already before adding to the queue.
   *
   *   a  b  c  d  e  f
   * a a  a  a            ------->
   * b                b   ------->
   * c          c         ------->
   * d             d      ------->
   * e                    ------->
   * f
   *
   * {a:nullptr, b:&a, c:&a, f:&b, d:&c, e:&d}
   */

  // Just because we need start in the visited map (value doesn't matter)
  unordered_map<T, T> visited({{start, start}});
  queue<T> q;
  q.push(start);

  while (!q.empty()) {
    T& item = q.front();

    const unordered_set<T>* relatives = getRelatives(item);
    for (const T& rel : *relatives) {
      // Traverse the visited map backwards to find path
      if (rel == finish) {
        forward_list<T> path({finish});
        T&& prevNode = std::move(item);
        while (prevNode != start) {
          path.push_front(prevNode);
          prevNode = visited.at(prevNode);
        }

        path.push_front(start);
        return path;
      }

      // If has not been visited, map the relative to the item that led to it
      if (fcn(rel, forward<Arg2s>(args)...) && visited.count(rel) == 0) {
        visited.insert({{rel, item}});
        q.push(rel);
      }
    }

    q.pop();
  }

  // Return empty list if no path
  return forward_list<T>();
}

template <typename T, bool (*F)(const T&, const T&)>
size_t Graph<T, F>::size() {
  return relativeMap_.size();
}

template <typename T, bool (*F)(const T&, const T&)>
size_t Graph<T, F>::numEdges() {
  // Add up degrees of vertices and divide by 2
  size_t total = 0;

  for (auto& pair : relativeMap_) {
    total += pair.second.size();
  }

  return total / 2;
}

template <typename T, bool (*F)(const T&, const T&)>
typename Graph<T, F>::iterator Graph<T, F>::begin() {
  Graph<T, F>::iterator i{relativeMap_.begin()};
  return i;
}

template <typename T, bool (*F)(const T&, const T&)>
typename Graph<T, F>::iterator Graph<T, F>::end() {
  Graph<T, F>::iterator i{relativeMap_.end()};
  return i;
}

template <typename T, bool (*F)(const T&, const T&)>
typename Graph<T, F>::const_iterator Graph<T, F>::begin() const {
  cout << "called" << endl;
  Graph<T, F>::const_iterator i{relativeMap_.cbegin()};
  return i;
}

template <typename T, bool (*F)(const T&, const T&)>
typename Graph<T, F>::const_iterator Graph<T, F>::end() const {
  Graph<T, F>::const_iterator i{relativeMap_.cend()};
  return i;
}

/***********************
 * Iterator Operations *
 ***********************/

template <typename T, bool (*F)(const T&, const T&)>
template <bool is_const>
Graph<T, F>::Iterator<is_const>::Iterator(const Iterator<false>& iter)
    : mapIter_{iter.mapIter_} {
  // Nothing to do
}

template <typename T, bool (*F)(const T&, const T&)>
template <bool is_const>
Graph<T, F>::Iterator<is_const>::Iterator(map_iter_t mapIter)
    : mapIter_{mapIter} {
  // Nothing to do
}

template <typename T, bool (*F)(const T&, const T&)>
template <bool is_const>
typename Graph<T, F>::template Iterator<is_const>&
Graph<T, F>::Iterator<is_const>::operator++() {
  mapIter_.operator++();
  return *this;
}

template <typename T, bool (*F)(const T&, const T&)>
template <bool is_const>
typename Graph<T, F>::template Iterator<is_const>::reference
    Graph<T, F>::Iterator<is_const>::operator*() const {
  return *mapIter_;
}

template <typename T, bool (*F)(const T&, const T&)>
template <bool is_const>
typename Graph<T, F>::template Iterator<is_const>::pointer
    Graph<T, F>::Iterator<is_const>::operator->() const {
  return mapIter_.operator->();
}

template <typename T, bool (*F)(const T&, const T&)>
template <bool is_const>
bool Graph<T, F>::Iterator<is_const>::operator==(const Iterator& other) const {
  return mapIter_ == other.mapIter_;
}

template <typename T, bool (*F)(const T&, const T&)>
template <bool is_const>
bool Graph<T, F>::Iterator<is_const>::operator!=(const Iterator& other) const {
  return mapIter_ != other.mapIter_;
}
