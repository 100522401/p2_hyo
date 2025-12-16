#ifndef OPEN_LIST_H
#define OPEN_LIST_H

#include "node.hpp"
#include <functional>
#include <queue>
#include <vector>

class OpenList {
private:
  std::priority_queue<Node, std::vector<Node>, std::greater<>> openlist_;

public:
  // Constructor
  OpenList() = default;

  // List operations
  inline void push(Node const &n) { openlist_.push(n); }

  inline Node pop() {
    Node n = openlist_.top();
    openlist_.pop();
    return n;
  };

  inline void clear() {
    openlist_ = std::priority_queue<Node, std::vector<Node>, std::greater<>>();
  }

  // List properties
  inline bool empty() { return openlist_.empty(); }
};

#endif