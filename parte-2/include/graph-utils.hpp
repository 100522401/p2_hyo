#pragma once
#include <utility>
#include <vector>

struct Coord {
  double lon;
  double lat;
};

class Graph {
public:
  int n; // número de nodos
  int m; // número de aristas

  /* row_ptr[u] -> dónde empiezan los vecinos de u
     row_ptr[u+1] -> dónde acaban los vecinos de u*/
  std::vector<int> row_ptr;

  /*lugar en el que se almacenan los vecinos de cada nodo*/
  std::vector<int> col_idx;

  /*contiene los pesos correspondientes a las aristas de col_idx*/
  std::vector<int> weights;
  std::vector<Coord> coords; // coordenadas de los nodos, si existen

  Graph() : n(0), m(0) {}
  Graph(int nodes, int edges) : n(nodes), m(edges) {
    row_ptr.resize(n + 1, 0);
    col_idx.resize(m);
    weights.resize(m);
    coords.resize(n);
  }

  // Función auxiliar para iterar vecinos de un nodo
  // std::vector<int> neighbors(int u) const {
  //   return std::vector<int>(col_idx.begin() + row_ptr[u],
  //                           col_idx.begin() + row_ptr[u + 1]);
  // }

  /***
   * Esta función devuelve dos punteros:
   *    - Puntero que indica el inicio de los vecinos del nodo u en col_idx
   *    - Puntero que indica el final de los vecinos del nodo u en col_idx (uno
   * después del último)
   */
  inline std::pair<const int *, const int *> neighbors(int u) const {
    return {col_idx.data() + row_ptr[u], col_idx.data() + row_ptr[u + 1]};
  }
};
