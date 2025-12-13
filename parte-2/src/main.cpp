#include "algorithm.hpp"
#include "graph_parser.hpp"
#include <iostream>

int main() {
  // Inicializar parser
  GraphParser parser("../graph-data/USA-road-d.USA");

  // Parsear grafo
  Graph g = parser.parse_debug();

  int u = 2; // nodo 3 en base 0

  std::cout << "Vecinos del nodo 3 (indice 2):\n";

  auto [begin, end] = g.neighbours(u);
  for (auto it = begin; it != end; ++it) {
    int v = *it;
    int edge_idx = (it - begin) + g.row_ptr[u];
    int w = g.weights[edge_idx];

    std::cout << "3 -> " << (v + 1) << "  peso = " << w << "\n";
  }

  // Definir nodos de prueba
  int start_node = 2;
  int goal_node = 10;

  // Ejecutar algoritmo A*
  Algorithm astar(g, start_node, goal_node);
  AlgorithmResult result = astar.run();

  // Mostrar resultados
  if (!result.path.empty()) {
    std::cout << "Camino encontrado desde " << start_node << " hasta "
              << goal_node << ":\n";
    for (size_t i = 0; i < result.path.size(); ++i) {
      std::cout << result.path[i];
      if (i + 1 < result.path.size())
        std::cout << " -> ";
    }
    std::cout << "\nCoste total: " << result.cost << "\n";
  } else {
    std::cout << "No se ha encontrado camino entre " << start_node << " y "
              << goal_node << "\n";
  }

  return 0;
}
