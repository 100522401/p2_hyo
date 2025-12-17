#include "algorithm.hpp"
#include "graph_parser.hpp"
#include <fstream>
#include <iostream>

static int edge_cost(const Graph &g, int u, int v) {
  auto [begin, end] = g.neighbours(u);
  for (auto it = begin; it != end; ++it) {
    if (*it == v) {
      int edge_idx = (it - begin) + g.row_ptr[u];
      return g.weights[edge_idx];
    }
  }
  return -1;
}

enum class AlgorithmMode { ASTAR, DIJKSTRA, BOTH };

int main(int argc, char **argv) {

  /* =======================
   * Argument parsing
   * ======================= */
  if (argc != 5 && argc != 7) {
    std::cerr << "Uso:\n"
              << argv[0]
              << " <vertice-1> <vertice-2> <nombre-del-mapa> <fichero-salida>\n"
              << "Opcional:\n"
              << "  --algorithm astar | dijkstra | both\n";
    return 1;
  }

  int start_node = std::stoi(argv[1]) - 1; // base 0
  int goal_node = std::stoi(argv[2]) - 1;
  std::string map_name = argv[3];
  std::string output_file = argv[4];

  if (start_node < 0 || goal_node < 0) {
    std::cerr << "Los vértices deben ser >= 1\n";
    return 1;
  }

  AlgorithmMode mode = AlgorithmMode::ASTAR;

  if (argc == 7) {
    if (std::string(argv[5]) != "--algorithm") {
      std::cerr << "Opción desconocida: " << argv[5] << "\n";
      return 1;
    }

    std::string value = argv[6];
    if (value == "astar")
      mode = AlgorithmMode::ASTAR;
    else if (value == "dijkstra")
      mode = AlgorithmMode::DIJKSTRA;
    else if (value == "both")
      mode = AlgorithmMode::BOTH;
    else {
      std::cerr << "Algoritmo desconocido: " << value << "\n";
      return 1;
    }
  }

  const bool run_astar = (mode != AlgorithmMode::DIJKSTRA);
  const bool run_dijkstra = (mode != AlgorithmMode::ASTAR);

  /* =======================
   * Graph parsing
   * ======================= */
  GraphParser parser(map_name);
  Graph g = parser.parse_debug();

  const int n_nodes = g.row_ptr.size() - 1;
  const int n_edges = g.weights.size();

  if (n_nodes != g.n || n_edges != g.m) {
    std::cerr << "Error en el parseo del grafo\n";
    return 1;
  }

  if (start_node >= n_nodes || goal_node >= n_nodes) {
    std::cerr << "Vértices fuera de rango. #vértices = " << n_nodes << "\n";
    return 1;
  }

  /* =======================
   * Run algorithms
   * ======================= */
  Algorithm solver(g, start_node, goal_node);

  AlgorithmResult astar_result{};
  AlgorithmResult dijkstra_result{};

  if (run_astar)
    astar_result = solver.run();

  if (run_dijkstra)
    dijkstra_result = solver.run_dijkstra();

  if (mode == AlgorithmMode::BOTH &&
      astar_result.cost != dijkstra_result.cost) {
    std::cerr << "[ERROR] Los costes no coinciden entre A* y Dijkstra\n";
    return 1;
  }

  const AlgorithmResult &final_result =
      (mode == AlgorithmMode::DIJKSTRA) ? dijkstra_result : astar_result;

  /* =======================
   * Output (console)
   * ======================= */
  std::cout << "# vertices: " << n_nodes << "\n";
  std::cout << "# arcos: " << n_edges << "\n";

  if (final_result.path.empty()) {
    std::cout << "No se ha encontrado camino entre " << (start_node + 1)
              << " y " << (goal_node + 1) << "\n";
    return 0;
  }

  std::cout << "Solución óptima encontrada con coste " << final_result.cost
            << "\n";

  if (run_astar)
    std::cout << "Expansiones A*: " << astar_result.expansions << "\n";

  if (run_dijkstra)
    std::cout << "Expansiones Dijkstra: " << dijkstra_result.expansions << "\n";

  // Print nodes/sec

  /* =======================
   * Output (file)
   * ======================= */
  std::ofstream fout(output_file);
  if (!fout) {
    std::cerr << "No se pudo abrir fichero de salida: " << output_file << "\n";
    return 1;
  }

  for (size_t i = 0; i < final_result.path.size(); ++i) {
    int u = final_result.path[i];
    fout << (u + 1);

    if (i + 1 < final_result.path.size()) {
      int v = final_result.path[i + 1];
      fout << " - (" << edge_cost(g, u, v) << ") - ";
    }
  }

  fout << "\n";
  return 0;
}
