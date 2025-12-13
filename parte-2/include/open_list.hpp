#ifndef OPEN_LIST_H
#define OPEN_LIST_H

#include "node.hpp"
#include <vector>

class OpenList {
private:
  std::vector<Node> openlist_; // va a estar siempre ordenada por f

public:
  // Constructor
  OpenList() = default;

  void push(Node n);
  void insert_neighbors(std::vector<Node> &neighbors);

  Node pop();

  bool empty() { return openlist_.empty(); }
};

#endif