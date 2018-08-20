#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <queue>

using namespace std;

template <typename T>
Graph<T>::Graph()
  : numVertices_{0}
{
  // Nothing to do
}

template <typename T>
Graph<T>::Graph(const Graph& other)
  : relativeMap_{other.relativeMap_}, numVertices_{other.numVertices_}
{
   // Nothing to do
}

template <typename T>
Graph<T>::Graph(Graph<T>&& other)
{
  swap(other);
}

template <typename T>
Graph<T>& Graph<T>::operator=(Graph<T> other)
{
  swap(other);
  return *this;
}

template <typename T>
void Graph<T>::swap(Graph<T>& second)
{
  relativeMap_.swap(second.relativeMap_);
  std::swap(numVertices_, second.numVertices_);
}

template <typename T>
size_t Graph<T>::numVertices()
{
  return numVertices_;
}

template <typename T>
void Graph<T>::addVertex(const T& item, initializer_list<T> relatives)
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

template <typename T>
void Graph<T>::addVertex(T&& item, initializer_list<T> relatives)
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
template <typename T>
const unordered_set<T>* Graph<T>::getRelatives(const T& item)
{
  auto iter = relativeMap_.find(item);
  return iter == relativeMap_.end() ? nullptr : &(iter->second);
}

template <typename T>
int Graph<T>::getRelativeCount(const T& item)
{
  const unordered_set<T>* relatives = getRelatives(item);
  return relatives == nullptr ? -1 : relatives->size();
}


template <typename T>
bool Graph<T>::contains(const T& item)
{
  return relativeMap_.count(item) != 0;
}

// TODO: have to remove from all vertices' relatives
template <typename T>
bool Graph<T>::removeVertex(const T& item)
{
  return relativeMap_.erase(item) != 0;
}

template <typename T>
int Graph<T>::getPath(const T& start, const T& finish)
{
  if (getRelativeCount(start) < 1 || getRelativeCount(finish) < 1) {
    throw std::invalid_argument("Params start and finish must be present in graph.");
  }

  /**
   * Dynamic array approach
   * 1 -> (2,3)
   * 2 -> (1,3)
   * 3 -> (1,2)
   * 4 -> (3,5)
   * 5 -> (4)
   * 
   * Done when queue is empty
   * Put in visited set when we add to the queue
   * Check if visited before adding to the queue
   *   
   *   1 2 3 4 5
   * 1   1 1      ------->
   * 2            ------->
   * 3     2      ------->
   * 4         3  ------->
   * 5            ------->
   * 
   */

  // forward_list path{1, start};
  unordered_set<T> visited({start});
  queue<T> q;
  q.push(start);

  int steps = 0;

  while(!q.empty()) {
    T& item = q.front();
    q.pop();

    bool hasUnvisitedRelative = false;
    const unordered_set<T>* relatives = getRelatives(item);
    for (auto iter = relatives->begin(); iter != relatives->end(); ++iter) {
      if (*iter == finish) {
        return ++steps;
      }
      if (visited.count(*iter) == 0) {
        visited.insert(*iter);
        q.push(*iter);
        hasUnvisitedRelative = true;
      }
    }
    if (hasUnvisitedRelative) {
      ++steps;
    }
  }

  return -1;
}