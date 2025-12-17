#include "algorithm.hpp"
#include <algorithm> // for std::reverse
#include <chrono>    // Required for std::chrono
#include <cmath>
#include <iostream>

// Heuristic: Haversine distance between n and goal
double Algorithm::h(int n) {

  const Coord &a = graph_.coords[n];
  const Coord &b = graph_.coords[goal_];

  double lat1 = a.lat;
  double lon1 = a.lon;
  double lat2 = b.lat;
  double lon2 = b.lon;

  double dlat = lat2 - lat1;
  double dlon = lon2 - lon1;

  double sin_dlat2 = sin(dlat / 2.0);
  double sin_dlon2 = sin(dlon / 2.0);

  double a_h =
      sin_dlat2 * sin_dlat2 + cos(lat1) * cos(lat2) * sin_dlon2 * sin_dlon2;
  double c = 2.0 * atan2(sqrt(a_h), sqrt(1.0 - a_h));

  const double R = 6371000.0; // Earth Radius in meters
  return R * c;
}

// "Heuristic" for Dijkstra
inline double null_h(int n) { return 0.0; }

// Main method: runs A* and returns path and cost.
AlgorithmResult Algorithm::run() {
  // Start chrono for performance testing
  auto start = std::chrono::high_resolution_clock::now();

  // Initialize SOA for A* parameters (g_, parent_, closed_)
  std::fill(g_.begin(), g_.end(), INF);
  std::fill(parent_.begin(), parent_.end(), -1);
  std::fill(closed_.begin(), closed_.end(), 0);

  std::size_t expansions = 0;

  // Initialize starting node
  g_[start_] = 0.0;
  open_.push(Node(start_, h(start_)));

  while (!open_.empty()) {
    Node current = open_.pop();
    int u = current.id;

    // Case: node already processed
    if (closed_[u])
      continue;

    // Mark node as processed
    closed_[u] = 1;
    expansions++;

    // If objective reached, stop
    if (u == goal_)
      break;

    // Iterate neighbours through CSR
    auto [begin, end] = graph_.neighbours(u);
    for (auto it = begin; it != end; ++it) {

      // Initialize new cost
      int v = *it;
      int edge_idx = it - begin + graph_.row_ptr[u];
      double cost = static_cast<double>(graph_.weights[edge_idx]);
      double new_g = g_[u] + cost;

      // If the new cost is lower, update path
      if (!closed_[v] && new_g < g_[v]) {
        g_[v] = new_g;
        parent_[v] = u;
        double f = new_g + h(v);

        open_.push(Node(v, f));
      }
    }
  }

  // Rebuild path from start to goal
  std::vector<int> path;
  double total_cost = g_[goal_];
  int u = goal_;
  if (parent_[u] != -1 || u == start_) {
    while (u != -1) {
      path.push_back(u);
      u = parent_[u];
    }
    std::reverse(path.begin(), path.end());
  }

  auto end = std::chrono::high_resolution_clock::now();

  auto duration = end - start;

  // Print duration
  std::cerr
      << "Tiempo de ejecución A*: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
      << " milisegundos" << std::endl;

  // Print nodes/sec
  std::cerr << "Nodos/seg: "
            << (expansions * 1000.0) /
                   std::chrono::duration_cast<std::chrono::milliseconds>(
                       duration)
                       .count()
            << std::endl;
  // std::cerr << "Expansiones: " << expansions << std::endl;

  return AlgorithmResult{path, total_cost, expansions};
}

// Dijkstra Akgorithm for comparison
AlgorithmResult Algorithm::run_dijkstra() {
  auto start_time = std::chrono::high_resolution_clock::now();

  // Inicialización
  std::fill(g_.begin(), g_.end(), INF);
  std::fill(parent_.begin(), parent_.end(), -1);
  std::fill(closed_.begin(), closed_.end(), 0);

  std::size_t expansions = 0;

  // Cola de prioridad: (coste acumulado, nodo)
  // Reusa Node, pero f = g
  open_.clear();
  g_[start_] = 0.0;
  open_.push(Node(start_, 0.0));

  while (!open_.empty()) {
    Node current = open_.pop();
    int u = current.id;

    if (closed_[u])
      continue;

    closed_[u] = 1;
    expansions++;

    // En Dijkstra esto ES correcto
    if (u == goal_)
      break;

    auto [begin, end] = graph_.neighbours(u);

    int idx = graph_.row_ptr[u];
    for (auto it = begin; it != end; ++it, ++idx) {
      int v = *it;
      double cost = static_cast<double>(graph_.weights[idx]);
      double new_g = g_[u] + cost;

      if (!closed_[v] && new_g < g_[v]) {
        g_[v] = new_g;
        parent_[v] = u;
        open_.push(Node(v, new_g));
      }
    }
  }

  // Reconstrucción del camino
  std::vector<int> path;
  double total_cost = g_[goal_];

  if (total_cost < INF) {
    for (int u = goal_; u != -1; u = parent_[u])
      path.push_back(u);

    std::reverse(path.begin(), path.end());
  }

  auto end_time = std::chrono::high_resolution_clock::now();

  std::cerr << "Tiempo de ejecución Dijkstra: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                     start_time)
                   .count()
            << " ms\n";

  // Print nodes/sec
  std::cerr << "Nodos/seg: "
            << (expansions * 1000.0) /
                   std::chrono::duration_cast<std::chrono::milliseconds>(
                       end_time - start_time)
                       .count()
            << std::endl;
  // std::cerr << "Expansiones: " << expansions << std::endl;

  return AlgorithmResult{path, total_cost, expansions};
}
