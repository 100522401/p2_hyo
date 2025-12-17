#include "algorithm.hpp"
#include <algorithm> // for std::reverse
#include <chrono>    // Required for std::chrono
#include <cmath>
#include <iostream>

/**
 * Heurística optimizada: Aproximación Equirrectangular.
 * Es admisible y mucho más rápida que Haversine al eliminar casi toda la
 * trigonometría.
 * * Nota: El factor cos_lat_goal se calcula en run() para maximizar eficiencia.
 */
double Algorithm::h(int n, double cos_lat_goal) {
  const Coord &a = graph_.coords[n];
  const Coord &b = graph_.coords[goal_];

  double dlat = b.lat - a.lat;
  double dlon = (b.lon - a.lon) * cos_lat_goal;

  // Distancia plana: R * sqrt(Δlat² + (Δlon * cos(lat))²)
  const double R = 6371000.0;
  return R * sqrt(dlat * dlat + dlon * dlon);
}

// "Heuristic" for Dijkstra
inline double null_h(int n) { return 0.0; }

// Main method: runs A* and returns path and cost.
AlgorithmResult Algorithm::run() {
  auto start = std::chrono::high_resolution_clock::now();

  // --- OPTIMIZACIÓN PRE-BÚSQUEDA ---
  // Calculamos el coseno de la latitud de destino UNA SOLA VEZ.
  // Esto ahorra millones de llamadas a la función cos() en el bucle principal.
  double cos_lat_goal = cos(graph_.coords[goal_].lat);

  // Initialize SOA for A* parameters (g_, parent_, closed_)
  std::fill(g_.begin(), g_.end(), INF);
  std::fill(parent_.begin(), parent_.end(), -1);
  std::fill(closed_.begin(), closed_.end(), 0);
  open_.clear();

  std::size_t expansions = 0;

  // Initialize starting node
  g_[start_] = 0.0;
  open_.push(Node(start_, h(start_, cos_lat_goal)));

  while (!open_.empty()) {
    Node current = open_.pop();
    int u = current.id;

    if (closed_[u])
      continue;

    closed_[u] = 1;
    expansions++;

    if (u == goal_)
      break;

    auto [begin, end] = graph_.neighbours(u);
    for (auto it = begin; it != end; ++it) {
      int v = *it;
      int edge_idx = it - begin + graph_.row_ptr[u];
      double cost = static_cast<double>(graph_.weights[edge_idx]);
      double new_g = g_[u] + cost;

      if (!closed_[v] && new_g < g_[v]) {
        g_[v] = new_g;
        parent_[v] = u;
        // Pasamos el coseno precalculado
        double f = new_g + h(v, cos_lat_goal);
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
  auto ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

  std::cerr << "Tiempo de ejecución A*: " << ms << " milisegundos" << std::endl;
  if (ms > 0) {
    std::cerr << "Nodos/seg: " << (expansions * 1000.0) / ms << std::endl;
  }

  return AlgorithmResult{path, total_cost, expansions};
}

// Dijkstra Algorithm for comparison
AlgorithmResult Algorithm::run_dijkstra() {
  auto start_time = std::chrono::high_resolution_clock::now();

  std::fill(g_.begin(), g_.end(), INF);
  std::fill(parent_.begin(), parent_.end(), -1);
  std::fill(closed_.begin(), closed_.end(), 0);
  open_.clear();

  std::size_t expansions = 0;

  g_[start_] = 0.0;
  open_.push(Node(start_, 0.0));

  while (!open_.empty()) {
    Node current = open_.pop();
    int u = current.id;

    if (closed_[u])
      continue;

    closed_[u] = 1;
    expansions++;

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

  std::vector<int> path;
  double total_cost = g_[goal_];
  if (total_cost < INF) {
    for (int u = goal_; u != -1; u = parent_[u])
      path.push_back(u);
    std::reverse(path.begin(), path.end());
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                  start_time)
                .count();

  std::cerr << "Tiempo de ejecución Dijkstra: " << ms << " ms\n";
  if (ms > 0) {
    std::cerr << "Nodos/seg: " << (expansions * 1000.0) / ms << std::endl;
  }

  return AlgorithmResult{path, total_cost, expansions};
}