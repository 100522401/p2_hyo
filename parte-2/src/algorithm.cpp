#include "algorithm.hpp"
#include "node.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>

// Constante grande para inicialización
const int INF_INT = 2000000000;

// FACTOR DE CONVERSIÓN: Microgrados a Decímetros
// Cálculo:
// Radio Tierra (R) = 6,371,000 metros = 63,710,000 decímetros.
// Perímetro = 2 * PI * R ≈ 400,301,735 decímetros.
// 1 grado = Perímetro / 360 ≈ 1,111,949 decímetros.
// 1 microgrado (10^-6) ≈ 1.111949 decímetros.
const double MICRODEG_TO_DECIMETERS = 1.111949266;

// FACTOR DE SEGURIDAD
// Reduce la h un 0.1% para absorber errores de la proyección plana y garantizar
// que h nunca supere el coste real (admisibilidad).
const double ADMISSIBILITY_FACTOR = 0.999;

const double FINAL_FACTOR = MICRODEG_TO_DECIMETERS * ADMISSIBILITY_FACTOR;

/**
 * Heurística Euclídea Escalada (Modo Rápido).
 * Asume que el grafo está en coordenadas enteras (microgrados)
 * y los pesos en decímetros.
 */
int Algorithm::h(int n, double cos_lat_goal) {
  const Coord &a = graph_.coords[n];
  const Coord &b = graph_.coords[goal_];

  // 1. Diferencias directas (en microgrados)
  long long dlat = std::abs(a.lat - b.lat);
  long long dlon = std::abs(a.lon - b.lon);

  // 2. Ajuste de longitud por la latitud (proyección equirrectangular)
  // Escala la diferencia de longitud según el coseno de la latitud
  // promedio/objetivo
  double dlon_scaled = dlon * cos_lat_goal;

  // 3. Distancia Euclídea en "unidades de mapa"
  // Usamos double para la suma de cuadrados para evitar overflow de long long
  double dist_sq = (double)(dlat * dlat) + (dlon_scaled * dlon_scaled);

  // 4. Raíz cuadrada y conversión a decímetros
  double dist_raw = std::sqrt(dist_sq);

  return static_cast<int>(dist_raw * FINAL_FACTOR);
}

AlgorithmResult Algorithm::run() {
  auto start_time = std::chrono::high_resolution_clock::now();

  // 1. Precalcular Coseno de la latitud del objetivo para la proyección
  // Convertimos de microgrados a radianes: (lat / 10^6) * (PI / 180)
  double lat_rad = (graph_.coords[goal_].lat / 1000000.0) * (M_PI / 180.0);
  double cos_lat_goal = std::cos(lat_rad);

  // 2. Reiniciar estructuras
  std::fill(g_.begin(), g_.end(), INF_INT);
  std::fill(parent_.begin(), parent_.end(), -1);
  std::fill(closed_.begin(), closed_.end(), 0);

  open_.clear();

  std::size_t expansions = 0;

  // 3. Nodo inicial
  g_[start_] = 0;
  int start_h = h(start_, cos_lat_goal);

  // Push directo (id, f_score)
  open_.push(start_, 0 + start_h);

  // 4. Bucle principal
  while (!open_.empty()) {
    int u = open_.pop();

    // Lazy removal
    if (closed_[u])
      continue;

    closed_[u] = 1;
    expansions++;

    if (u == goal_)
      break;

    auto [begin, end] = graph_.neighbours(u);
    int gu = g_[u]; // Caché local

    for (auto it = begin; it != end; ++it) {
      int v = *it;

      // Índice del arco en CSR
      int edge_idx = it - begin + graph_.row_ptr[u];
      int cost = graph_.weights[edge_idx];

      int new_g = gu + cost;

      if (new_g < g_[v]) {
        g_[v] = new_g;
        parent_[v] = u;

        int f = new_g + h(v, cos_lat_goal);
        open_.push(v, f);
      }
    }
  }

  // 5. Reconstrucción del camino
  std::vector<int> path;
  int total_cost = g_[goal_];

  if (total_cost != INF_INT) {
    int u = goal_;
    while (u != -1) {
      path.push_back(u);
      u = parent_[u];
    }
    std::reverse(path.begin(), path.end());
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                  start_time)
                .count();
  return AlgorithmResult{path, static_cast<double>(total_cost), expansions, ms};
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
  open_.push(start_, 0.0);

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
        open_.push(v, new_g);
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
  return AlgorithmResult{path, total_cost, expansions, ms};
}