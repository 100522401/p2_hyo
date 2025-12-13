#pragma once
#include "algorithm.hpp"
#include <algorithm> // for std::reverse
#include <cmath>

// Heurística: distancia Haversine entre el nodo n y el goal
double Algorithm::h(int n) {
  const Coord &a = graph_.coords[n];
  const Coord &b = graph_.coords[goal_];

  double lat1 = a.lat * M_PI / 180.0;
  double lon1 = a.lon * M_PI / 180.0;
  double lat2 = b.lat * M_PI / 180.0;
  double lon2 = b.lon * M_PI / 180.0;

  double dlat = lat2 - lat1;
  double dlon = lon2 - lon1;

  double sin_dlat2 = sin(dlat / 2.0);
  double sin_dlon2 = sin(dlon / 2.0);

  double a_h =
      sin_dlat2 * sin_dlat2 + cos(lat1) * cos(lat2) * sin_dlon2 * sin_dlon2;
  double c = 2.0 * atan2(sqrt(a_h), sqrt(1.0 - a_h));

  const double R = 6371000.0; // radio de la Tierra en metros
  return R * c;
}

// Método principal: ejecuta A* y devuelve el camino y coste
AlgorithmResult Algorithm::run() {
  // Reinicializar SOA
  std::fill(g_.begin(), g_.end(), INF);
  std::fill(parent_.begin(), parent_.end(), -1);
  std::fill(closed_.begin(), closed_.end(), 0);

  // Inicialización nodo inicial
  g_[start_] = 0.0;
  open_.push(Node(start_, h(start_)));

  while (!open_.empty()) {
    Node current = open_.pop();
    int u = current.id;

    if (closed_[u])
      continue; // ya procesado

    closed_[u] = 1;

    if (u == goal_)
      break; // objetivo alcanzado

    // Iterar vecinos desde CSR
    auto [begin, end] = graph_.neighbours(u);
    for (auto it = begin; it != end; ++it) {
      int v = *it;
      int edge_idx = it - begin + graph_.row_ptr[u];
      double cost = static_cast<double>(graph_.weights[edge_idx]);
      double new_g = g_[u] + cost;

      if (new_g < g_[v]) {
        g_[v] = new_g;
        parent_[v] = u;
        double f = new_g + h(v);
        open_.push(Node(v, f));
      }
    }
  }

  // Reconstruir camino desde goal a start
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

  return AlgorithmResult{path, total_cost};
}
