#include "algorithm.hpp"
#include "graph_parser.hpp"
#include <cassert>
#include <fstream>
#include <iostream>

static int edge_cost(Graph const &g, int u, int v) {
  auto [begin, end] = g.neighbours(u);
  for (auto it = begin; it != end; ++it) {
    if (*it == v) {
      int edge_idx = it - begin + g.row_ptr[u];
      return g.weights[edge_idx];
    }
  }
  return -1;
}

// Main method: Finds shortest path between two nodes on a graph.
int main(int argc, char **argv) {

  if (argc != 5) {
    // ./programa vertice-1 vertice-2 nombre-del-mapa fichero-salida
    std::cerr
        << "Uso: " << argv[0]
        << " <vertice-1> <vertice-2> <nombre-del-mapa> <fichero-salida>\n";
    return 1;
  }

  int start_node = std::stoi(argv[1]);
  int goal_node = std::stoi(argv[2]);
  std::string map_name = argv[3];
  std::string output_file = argv[4];

  // Convert to base 0
  start_node--;
  goal_node--;

  if (start_node < 0 || goal_node < 0) {
    std::cerr << "Los vértices deben ser >= 1. \n";
    return 1;
  }
  // Initialize parser
  GraphParser parser(map_name);

  // Parse graph
  Graph g = parser.parse_debug();

  // Check that the number of nodes and edges are correct
  const int n_nodes = g.row_ptr.size() - 1;
  const int n_edges = g.weights.size();
  if (n_nodes != g.n || n_edges != g.m) {
    std::cerr << "Error en el parseo" << std::endl;
    return 1;
  }
  // Check that the vertices are in the graph
  if (start_node >= n_nodes || goal_node >= n_nodes) {
    std::cerr << "Vertices fuera de rango. #vertices = " << n_nodes << "\n";
    return 1;
  }

  // Run A* algorithm for shortest path searching

  Algorithm astar(g, start_node, goal_node);
  AlgorithmResult result = astar.run();

  // Run Dijkstra for comparison
  AlgorithmResult result_dijkstra = astar.run_dijkstra();

  if (result.path != result_dijkstra.path) {
    std::cerr << "Los caminos encontrados no coinciden" << "\n";
    return 1;
  }

  // Required prints
  std::cout << "# vertices: " << n_nodes << "\n";
  std::cout << "# arcos : " << n_edges << "\n";

  if (result.path.empty()) {
    std::cout << "No se ha encontrado camino entre " << start_node << " y "
              << goal_node << "\n";
    return 0;
  }

  std::cout << "Solución óptima encontrada con coste " << result.cost << "\n";
  std::cout << "Expansiones A*: " << result.expansions << "\n";
  std::cout << "Expansiones Dijkstra: " << result_dijkstra.expansions << "\n";

  std::ofstream fout(output_file);
  if (!fout.is_open()) {
    std::cerr << "No se pudo abrir fichero de salida: " << output_file << "\n";
    return 1;
  }

  for (size_t i = 0; i < result.path.size(); ++i) {
    int u = result.path[i];
    fout << (u + 1); // base 1

    if (i + 1 < result.path.size()) {
      int v = result.path[i + 1];
      int w = edge_cost(g, u, v);
      fout << " - (" << w << ") - ";
    }
  }
  fout << "\n";
  fout.close();

  return 0;
}
