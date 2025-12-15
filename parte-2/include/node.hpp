#pragma once

class Node {
public:
  Node() = default;
  Node(int id, double f) : id(id), f(f) {};
  Node(int id) : id(id), f(0) {}

  int id;
  double f; // f(n) = g(n) + h(n)

  // Override '>' operator so that nodes can be sorted by their 'f' attribute
  // (used in OpenList to sort nodes).
  bool operator>(const Node &other) const { return f > other.f; }
};