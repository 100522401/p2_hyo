#include "algorithm.hpp"
#include "graph_parser.hpp"
#include "logger.hpp"
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
    Logger::error("Argumentos incorrectos.");
    std::cout << "Uso:\n"
              << "  " << argv[0]
              << " <v_inicio> <v_fin> <mapa> <fichero_salida>\n"
              << "Opcional:\n"
              << "  --algorithm <astar | dijkstra | both>\n";
    return 1;
  }

  // Parse nodes (in base 0)
  int start_node = std::stoi(argv[1]) - 1; // base 0
  int goal_node = std::stoi(argv[2]) - 1;

  // Node verifications
  if (start_node < 0 || goal_node < 0) {
    Logger::error("Los vértices deben ser >= 1.");
    return 1;
  }
  // Parse map name and outfile
  std::string map_name = argv[3];
  std::string output_filename = argv[4];

  // Default algorithm
  AlgorithmMode mode = AlgorithmMode::ASTAR;

  // If algorithm was specified:
  if (argc == 7) {
    if (std::string(argv[5]) != "--algorithm") {
      Logger::error("Opción desconocida: " + std::string(argv[5]));
      return 1;
    }

    // Parse algorithm
    std::string value = argv[6];
    if (value == "astar")
      mode = AlgorithmMode::ASTAR;
    else if (value == "dijkstra")
      mode = AlgorithmMode::DIJKSTRA;
    else if (value == "both")
      mode = AlgorithmMode::BOTH;
    else {
      Logger::error("Algoritmo desconocido: " + value);
      return 1;
    }
  }

  // Conditionals used for running algorithms
  const bool run_astar = (mode != AlgorithmMode::DIJKSTRA);
  const bool run_dijkstra = (mode != AlgorithmMode::ASTAR);

  Logger::print_header();

  /* =======================
   * Graph parsing
   * ======================= */
  GraphParser parser(map_name);
  Graph g = parser.parse_with_stats();

  const int n_nodes = g.row_ptr.size() - 1;
  const int n_edges = g.weights.size();

  // Case: error parsing
  if (n_nodes != g.n || n_edges != g.m) {
    Logger::error("Error en el parseo del grafo.");
    return 1;
  }

  // Case: Vertices out of range
  if (start_node >= n_nodes || goal_node >= n_nodes) {
    Logger::error("Vértices fuera de rango. El número de vértices es: " +
                  std::to_string(n_nodes));
    return 1;
  }

  /* =======================
   * Run algorithms
   * ======================= */
  Algorithm solver(g, start_node, goal_node);

  AlgorithmResult astar_result{};
  AlgorithmResult dijkstra_result{};

  // Run algorithms if specified
  if (run_astar)
    astar_result = solver.run();

  if (run_dijkstra)
    dijkstra_result = solver.run_dijkstra();

  // Print results
  if (run_astar) {
    Logger::print_alg_stats("A*", astar_result.ms, astar_result.expansions,
                            astar_result.cost);
  }
  if (run_dijkstra) {
    Logger::print_alg_stats("Dijkstra", dijkstra_result.ms,
                            dijkstra_result.expansions, dijkstra_result.cost);
  }
  if (mode == AlgorithmMode::BOTH) {
    Logger::print_comparison(astar_result.cost, dijkstra_result.cost);
  }

  const auto &result_to_write =
      (mode == AlgorithmMode::DIJKSTRA) ? dijkstra_result : astar_result;
  if (result_to_write.path.empty()) {
    Logger::info("No se ha encontrado camino entre " +
                 std::to_string(start_node + 1) + " y " +
                 std::to_string(goal_node + 1) + ".");
    return 0;
  }

  /* =======================
   * Output (file)
   * ======================= */
  std::ofstream fout(output_filename);
  if (!fout) {
    Logger::error("No se pudo abrir el fichero de salida: " + output_filename);
    return 1;
  }

  for (size_t i = 0; i < result_to_write.path.size(); ++i) {
    int u = result_to_write.path[i];
    fout << (u + 1);

    if (i + 1 < result_to_write.path.size()) {
      int v = result_to_write.path[i + 1];
      fout << " - (" << edge_cost(g, u, v) << ") - ";
    }
  }

  fout << "\n";
  return 0;
}
