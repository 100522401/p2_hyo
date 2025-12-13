#ifndef OPEN_LIST_H
#define OPEN_LIST_H

#include <vector>

struct Node {
  int id;
  double f; // f(n) = g(n) + h(n)
};

class OpenList {
private:
  std::vector<Node> openlist_; // va a estar siempre ordenada por f

public:
  // Constructor
  OpenList() = default;

  void insert_nodes(std::vector<Node> &neighbors);

  Node pop();

  bool empty() { return openlist_.empty(); }
};

#endif