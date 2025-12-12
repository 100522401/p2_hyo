#ifndef OPEN_LIST_H
#define OPEN_LIST_H

#include <vector>

struct Node {
  int id;
  double f; // f(n) = g(n) + h(n)
};

class OpenList {
private:
  std::vector<Node> openlist; // va a estar siempre ordenada por f

public:
public:
  // Constructor
  OpenList() = default;

  void insert_neighbors(std::vector<Node> &neighbors);

  Node pop();

  bool empty() { return openlist.empty(); }
};

#endif