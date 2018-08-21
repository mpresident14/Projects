#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <queue>

using namespace std;

template <typename T, bool(*F)(const T&, const T&)>
Graph<T,F>::Graph()
  : numVertices_{0}
{
  // Nothing to do
}

template <typename T, bool(*F)(const T&, const T&)>
Graph<T,F>::Graph(const Graph<T,F>& other)
  : relativeMap_{other.relativeMap_}, numVertices_{other.numVertices_} 
{
  // Nothing to do
}

template <typename T, bool(*F)(const T&, const T&)>
Graph<T,F>::Graph(Graph<T,F>&& other)
{
  swap(other);
}

template <typename T, bool(*F)(const T&, const T&)>
Graph<T,F>& Graph<T,F>::operator=(Graph<T,F> other)
{
  swap(other);
  return *this;
}

template <typename T, bool(*F)(const T&, const T&)>
void Graph<T,F>::swap(Graph<T,F>& second)
{
  relativeMap_.swap(second.relativeMap_);
  std::swap(numVertices_, second.numVertices_);
}

template <typename T, bool(*F)(const T&, const T&)>
size_t Graph<T,F>::numVertices()
{
  return numVertices_;
}

template <typename T, bool(*F)(const T&, const T&)>
bool Graph<T,F>::addVertex(const T& item)
{
  // Return if already in the graph
  if (relativeMap_.count(item) != 0) {
    return false;
  }

  // Insert the adjacency set for item
  relativeMap_.insert( {item, unordered_set<T>()} );
  ++numVertices_;

  unordered_set<T>& relatives = relativeMap_.at(item);

  for (auto& pair : relativeMap_) {
    if (F(item, pair.first)) {
      pair.second.insert(item);
      relatives.insert(pair.first);
    }
  }

  return true;
}

template <typename T, bool(*F)(const T&, const T&)>
bool Graph<T,F>::addVertex(T&& item)
{
  T&& rval = std::move(item);
  // Return if already in the graph
  if (relativeMap_.count(rval) != 0) {
    return false;
  }

  // Insert the adjacency set for item
  relativeMap_.insert({rval, unordered_set<T>()});
  ++numVertices_;

  unordered_set<T>& relatives = relativeMap_.at(rval);

  for (auto& pair : relativeMap_) {
    if (F(rval, pair.first)) {
      pair.second.insert(rval);
      relatives.insert(pair.first);
    }
  }

  return true;
}


template <typename T, bool(*F)(const T&, const T&)>
void Graph<T,F>::addVertex(const T& item, initializer_list<T> relatives)
{
  // Check to make sure all relatives are in the graph
  for (auto iter = relatives.begin(); iter != relatives.end(); ++iter) {
    if (relativeMap_.count(*iter) == 0) {
      throw std::invalid_argument("All relatives must be present in graph.");
    }
  }

  // Insert the adjacency set for item
  relativeMap_.insert({item, unordered_set<T>(relatives)});
  ++numVertices_;

  // Add item to each of the relatives' adjacency sets
  for (auto iter = relatives.begin(); iter != relatives.end(); ++iter) {
    relativeMap_.at(*iter).insert(item);
  }  
}

template <typename T, bool(*F)(const T&, const T&)>
void Graph<T,F>::addVertex(T&& item, initializer_list<T> relatives)
{
  // Check to make sure all relatives are in the graph
  for (auto iter = relatives.begin(); iter != relatives.end(); ++iter) {
    if (relativeMap_.count(*iter) == 0) {
      throw std::invalid_argument("All relatives must be present in graph.");
    }
  }

  T&& rval = std::move(item);

  // Insert the adjacency set for item
  relativeMap_.insert({rval, unordered_set<T>(relatives)});
  ++numVertices_;

  // Add item to each of the relatives' adjacency sets
  for (auto iter = relatives.begin(); iter != relatives.end(); ++iter) {
    relativeMap_.at(*iter).insert(rval);
  }  
}

// TODO: Make sure this ptr doesn't mess up if hashmap is reallocated
template <typename T, bool(*F)(const T&, const T&)>
const unordered_set<T>* Graph<T,F>::getRelatives(const T& item)
{
  auto iter = relativeMap_.find(item);
  return iter == relativeMap_.end() ? nullptr : &(iter->second);
}

template <typename T, bool(*F)(const T&, const T&)>
int Graph<T,F>::getRelativeCount(const T& item)
{
  const unordered_set<T>* relatives = getRelatives(item);
  return relatives == nullptr ? -1 : relatives->size();
}


template <typename T, bool(*F)(const T&, const T&)>
bool Graph<T,F>::contains(const T& item)
{
  return relativeMap_.count(item) != 0;
}

// TODO: have to remove from all vertices' relatives
template <typename T, bool(*F)(const T&, const T&)>
bool Graph<T,F>::removeVertex(const T& item)
{
  // If item is not in graph
  if (relativeMap_.erase(item) == 0) {
    return false;
  }

  // Remove item from other vertices adjacency sets
  for (auto& pair : relativeMap_) {
    pair.second.erase(item);
  }

  --numVertices_;

  return true;
}

template <typename T, bool(*F)(const T&, const T&)>
forward_list<T> Graph<T,F>::getShortestPath(const T& start, const T& finish)
{
  if (getRelativeCount(start) < 1 || getRelativeCount(finish) < 1) {
    throw std::invalid_argument("Params start and finish must be present in graph.");
  }

  if (start == finish) {
    return forward_list<T>( {start} );
  }

  /**
   * Dynamic approach: example
   * a -> (b,c)
   * b -> (a,c,f)
   * c -> (a,b)
   * d -> (c,e)
   * e -> (d)
   * f -> (b)
   * 
   * Done when queue is empty.
   * Map next item to previous item addr when we add to queue.
   * Check if visited already before adding to the queue.
   *   
   *   a  b  c  d  e  f
   * a np &a &a            ------->
   * b                &b   ------->
   * c          &c         ------->
   * d             &d      ------->
   * e                     ------->
   * f
   * 
   * {a:nullptr, b:&a, c:&a, f:&b, d:&c, e:&d}
   */

  // TODO: map reallocation may change memory address of T objects
  unordered_map<T, T*> visited( {{start, nullptr}} );
  queue<T> q;
  q.push(start);

  while(!q.empty()) {
    T& item = q.front();
    q.pop();

    const unordered_set<T>* relatives = getRelatives(item);
    for (const T& rel : *relatives) {

      // Traverse the visited map backwards to find path
      if (rel == finish) {
        forward_list<T> path( {finish} ); // Probably can include item here and init current node to item
        T* prevNode = &item;
        while (prevNode != nullptr) {
          path.push_front(*prevNode);
          prevNode = visited.at(*prevNode);
        }
        return path;
      }

      // If has not been visited, map the relative to the item that led to it
      if (visited.count(rel) == 0) {
        visited.insert( {{rel, &item}} );
        q.push(rel);
      }
    }
  }

  // Return empty list if no path
  return forward_list<T>();
}