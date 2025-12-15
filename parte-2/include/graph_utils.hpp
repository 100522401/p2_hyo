#pragma once
#include <vector>

struct Coord {
  double lon;
  double lat;
};

class Graph {
public:
  int n; // number of nodes
  int m; // number of edges

  /* row_ptr[u] -> where neighbors of u start
     row_ptr[u+1] -> where neighbors of u end */
  std::vector<int> row_ptr;

  /* storage for the neighbors of each node */
  std::vector<int> col_idx;

  std::vector<int> weights;
  std::vector<Coord> coords;

  Graph() : n(0), m(0) {}
  Graph(int nodes, int edges) : n(nodes), m(edges) {
    row_ptr.resize(n + 1, 0);
    col_idx.resize(m);
    weights.resize(m);
    coords.resize(n);
  }

  /***
   * This function returns two pointers:
   *    - Pointer to the start of neighbors for node u in col_idx
   *    - Pointer to the end of neighbors for node u in col_idx (one past the
   * last one)
   */
  inline std::pair<const int *, const int *> neighbours(int u) const {
    return {col_idx.data() + row_ptr[u], col_idx.data() + row_ptr[u + 1]};
  }
};
