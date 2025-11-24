#pragma once
#include <string>
#include <utility>
#include <vector>

class Graph {
public:
  int n; // número de nodos
  int m; // número de aristas
  std::vector<int> row_ptr;
  std::vector<int> col_idx;
  std::vector<int> weights; // opcional, si el grafo es ponderado
  std::vector<std::pair<double, double>>
      coords; // coordenadas de los nodos, si existen

  Graph() : n(0), m(0) {}
  Graph(int nodes, int edges) : n(nodes), m(edges) {
    row_ptr.resize(n + 1, 0);
    col_idx.resize(m);
    weights.resize(m);
    coords.resize(n);
  }

  // Función auxiliar para iterar vecinos de un nodo
  std::vector<int> neighbors(int u) const {
    return std::vector<int>(col_idx.begin() + row_ptr[u],
                            col_idx.begin() + row_ptr[u + 1]);
  }
};
