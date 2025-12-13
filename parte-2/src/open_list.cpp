#include "open_list.hpp"

void OpenList::insert_neighbors(std::vector<Node> &neighbors) {
  std::vector<Node> result;
  result.reserve(openlist_.size() + neighbors.size());

  size_t i = 0, j = 0;

  // merge
  while (i < openlist_.size() && j < neighbors.size()) {
    // ordeno la lista de mayor a menor para que hacer el pop sea O(1) en vez de
    // O(n)
    if (openlist_[i].f > neighbors[j].f) {
      result.push_back(openlist_[i]);
      i++;
    } else {
      result.push_back(neighbors[j]);
      j++;
    }
  }

  // se copian los restantes

  while (i < openlist_.size()) {
    result.push_back(openlist_[i]);
    i++;
  }
  while (j < neighbors.size()) {
    result.push_back(neighbors[j]);
    j++;
  }

  // intercambia los punteros internos de los dos vectores (más barato que hacer
  // openlist = result)
  openlist_.swap(result);
}

void OpenList::push(Node n) { openlist_.push_back(n); }

Node OpenList::pop() {
  Node n = openlist_.back();
  openlist_.pop_back();
  return n;
}
