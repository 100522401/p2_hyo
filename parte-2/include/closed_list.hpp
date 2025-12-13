#ifndef CLOSED_LIST_H
#define CLOSED_LIST_H

#include <vector>

struct ClosedNode {
  int id;
  int parent;
};

class ClosedList {
private:
  std::vector<ClosedNode> closed_;

public:
  // Constructor reserva espacio
  ClosedList(int size) : closed_(size) {}

  // Insertar/actualizar nodo por índice
  void insert(const ClosedNode &node) { closed_[node.id] = node; }

  // Obtener nodo por índice
  ClosedNode &get(int id) { return closed_[id]; }
};

#endif
