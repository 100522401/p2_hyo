#pragma once

class Node {
public:
  Node() = default;
  Node(int id, double f) : id(id), f(f) {};
  Node(int id) : id(id), f(0) {}

  int id;
  double f; // f(n) = g(n) + h(n)

  // Sobrecargar el operador > para que se puedan comparar nodos por f
  // (usado en la lista abierta para ordenarla).
  bool operator>(const Node &other) const { return f > other.f; }
};