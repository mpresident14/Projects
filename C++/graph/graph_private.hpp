#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>

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

  relativeMap_.insert({item, unordered_set<T>(relatives)});
  ++numVertices_;
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

  relativeMap_.insert({std::move(item), unordered_set<T>(relatives)});
  ++numVertices_;
}

// TODO: Make sure this ptr doesn't mess up if hashmap is reallocated
template <typename T>
const unordered_set<T>* Graph<T>::getRelatives(const T& item)
{
  auto iter = relativeMap_.find(item);
  return iter == relativeMap_.end() ? nullptr : &(iter->second);
}

template <typename T>
bool Graph<T>::contains(const T& item)
{
  return relativeMap_.count(item) != 0;
}

template <typename T>
bool Graph<T>::removeVertex(const T& item)
{
  return relativeMap_.erase(item) != 0;
}