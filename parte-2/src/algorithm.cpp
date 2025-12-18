#include "algorithm.hpp"
#include "node.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>

// Constante grande para inicialización
const int INF_INT = 2000000000;

/**
 * Heurística Entera.
 * IMPORTANTE: El resultado debe estar en la misma unidad que graph_.weights.
 * Si graph_.weights son tiempos (segundos), y esto calcula distancia (metros),
 * tu A* degradará a Dijkstra (si h es muy pequeña) o no será admisible (si h es
 * muy grande).
 * * Asumiremos aquí que necesitas distancia en metros.
 * Se hace un cast a int al final.
 */
int Algorithm::h(int n, double cos_lat_goal) {
  const Coord &a = graph_.coords[n];
  const Coord &b = graph_.coords[goal_];

  double dlat = b.lat - a.lat;
  double dlon = (b.lon - a.lon) * cos_lat_goal;

  // R = 6371000.0 metros
  // Multiplica por un factor si tus pesos no son metros (ej. si son decímetros
  // o mm)
  double dist = 6371000.0 * std::sqrt(dlat * dlat + dlon * dlon);

  return static_cast<int>(dist);
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