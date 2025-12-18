#include "algorithm.hpp"
#include "node.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>

// Constante grande para inicialización
const int INF_INT = 2000000000;

#include <cmath>

/// Asegúrate de tener estas constantes definidas arriba o en la clase
constexpr double DIMACS_TO_RAD = 0.00000001745329251994;
constexpr double EARTH_RADIUS_METERS = 6371000.0;

int Algorithm::h(int n, double /*unused*/) {
  const Coord &n_coord = graph_.coords[n];
  const Coord &target_coord = graph_.coords[goal_];

  // 1. Convertir enteros DIMACS a radianes
  double lat1 = n_coord.lat * DIMACS_TO_RAD;
  double lon1 = n_coord.lon * DIMACS_TO_RAD;
  double lat2 = target_coord.lat * DIMACS_TO_RAD;
  double lon2 = target_coord.lon * DIMACS_TO_RAD;

  // 2. Fórmula de Haversine
  double dlat = lat2 - lat1;
  double dlon = lon2 - lon1;

  double a =
      std::sin(dlat / 2) * std::sin(dlat / 2) +
      std::cos(lat1) * std::cos(lat2) * std::sin(dlon / 2) * std::sin(dlon / 2);

  double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

  // 3. Distancia en metros
  double dist_meters = EARTH_RADIUS_METERS * c;

  // 4. ESCALADO (La clave del éxito)
  // El grafo está en decímetros (ratio ~10).
  // Usamos 9.99 en vez de 10.0 para asegurar admisibilidad (h <= g)
  // ante mínimas variaciones del elipsoide vs esfera.
  return static_cast<int>(dist_meters * 9.99);
}

AlgorithmResult Algorithm::run() {
  auto start_time = std::chrono::high_resolution_clock::now();

  double cos_lat_goal = std::cos(graph_.coords[goal_].lat);

  // 1. Reiniciar estructuras (Usamos int para g_)
  // g_ debe ser std::vector<int> en tu header
  std::fill(g_.begin(), g_.end(), INF_INT);
  std::fill(parent_.begin(), parent_.end(), -1);
  std::fill(closed_.begin(), closed_.end(),
            0); // closed puede ser vector<bool> o char

  open_.clear();

  std::size_t expansions = 0;

  // 2. Nodo inicial
  g_[start_] = 0;
  int start_h = h(start_, cos_lat_goal);

  // Push directo (id, f_score)
  open_.push(start_, 0 + start_h);
  // --- INICIO DEBUG DE CALIBRACIÓN ---
  std::cout << "\n[CALIBRANDO ESCALA]" << std::endl;
  // Miramos los primeros 5 vecinos del nodo start para ver la relación
  auto [dbg_begin, dbg_end] = graph_.neighbours(start_);
  int count = 0;

  for (auto it = dbg_begin; it != dbg_end && count < 5; ++it, ++count) {
    int v = *it;

    // 1. Obtenemos el peso que dice el grafo (g)
    int edge_idx = it - dbg_begin + graph_.row_ptr[start_];
    int graph_weight = graph_.weights[edge_idx];

    // 2. Calculamos la distancia Haversine pura (h)
    // (Truco: calculamos h entre start y v)
    int old_goal = goal_;
    goal_ = v;
    int haversine_dist = h(start_, 0);
    goal_ = old_goal;

    if (haversine_dist == 0)
      haversine_dist = 1; // Evitar div/0

    double ratio = (double)graph_weight / (double)haversine_dist;

    std::cout << "  Arista " << start_ << "->" << v
              << " | Peso Grafo: " << graph_weight
              << " | Haversine (m): " << haversine_dist
              << " | RATIO (k): " << ratio << std::endl;
  }
  std::cout << "--------------------------------\n";
  // --- FIN DEBUG ---

  // 3. Bucle principal
  while (!open_.empty()) {
    // Pop devuelve solo el ID (int)
    int u = open_.pop();

    // Lazy removal: Si ya cerramos este nodo con un coste mejor o igual,
    // ignorar. En Bucket Queue es crítico verificar closed inmediatamente.
    if (closed_[u])
      continue;

    closed_[u] = 1;
    expansions++;

    if (u == goal_)
      break;

    auto [begin, end] = graph_.neighbours(u);
    // Optimización: Sacar g_[u] fuera del bucle for
    int gu = g_[u];

    for (auto it = begin; it != end; ++it) {
      int v = *it;

      // Cálculo del índice del peso en el grafo CSR
      int edge_idx = it - begin + graph_.row_ptr[u];

      // Pesos enteros
      int cost = graph_.weights[edge_idx];

      int new_g = gu + cost;

      if (new_g < g_[v]) {
        g_[v] = new_g;
        parent_[v] = u;

        // Calculamos f entero
        int f = new_g + h(v, cos_lat_goal);

        // Insertamos en el bucket
        open_.push(v, f);

        // Nota: En bucket queues no hacemos decrease-key explícito,
        // simplemente insertamos el nuevo par. El viejo se descartará al hacer
        // pop gracias al chequeo `if (closed_[u])`.
      }
    }
  }

  // 4. Reconstrucción del camino
  std::vector<int> path;
  int total_cost = g_[goal_]; // Coste entero

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

  std::cout << "\nTiempo de ejecución A* (Bucket): " << ms << " ms"
            << std::endl;
  std::cout << "Expansiones: " << expansions << std::endl;
  if (ms > 0) {
    std::cout << "Nodos/seg: " << (expansions * 1000.0) / ms << std::endl;
  }

  // Convertir coste a double para mantener compatibilidad con tu struct de
  // retorno si es necesario
  return AlgorithmResult{path, static_cast<double>(total_cost), expansions};
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

  std::cerr << "\nTiempo de ejecución Dijkstra: " << ms << " ms\n";
  std::cerr << "Expansiones Dijkstra: " << expansions << "\n";
  if (ms > 0) {
    std::cerr << "Nodos/seg Dijkstra: " << (expansions * 1000.0) / ms << "\n"
              << std::endl;
  }

  return AlgorithmResult{path, total_cost, expansions};
}