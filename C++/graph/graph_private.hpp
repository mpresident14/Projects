#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <cstddef>
#include <initializer_list>

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
    if (relativeMap_.find(*iter) == relativeMap_.end()) {
      // TODO: Throw exception here
      cerr << "Illegal args." << endl;
      return;
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
    if (relativeMap_.find(*iter) == relativeMap_.end()) {
      // TODO: Throw exception here
      cerr << "Illegal args." << endl;
      return;
    }
  }

  relativeMap_.insert({std::move(item), unordered_set<T>(relatives)});
  ++numVertices_;
}
