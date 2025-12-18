#include "algorithm.hpp"
#include "node.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>

// Large constant for initialization
const int INF_INT = 2000000000;

// CONVERSION FACTOR: Microdegrees to Decimeters
// Calculation:
// Earth Radius (R) = 6,371,000 meters = 63,710,000 decimeters.
// Perimeter = 2 * PI * R ≈ 400,301,735 decimeters.
// 1 degree = Perimeter / 360 ≈ 1,111,949 decimeters.
// 1 microdegree (10^-6) ≈ 1.111949 decimeters.
const double MICRODEG_TO_DECIMETERS = 1.111949266;

// SAFETY FACTOR
// Reduces h by 0.1% to absorb errors from the flat projection and ensure
// that h never exceeds the real cost (admissibility).
const double ADMISSIBILITY_FACTOR = 0.999;

const double FINAL_FACTOR = MICRODEG_TO_DECIMETERS * ADMISSIBILITY_FACTOR;

/**
 * Scaled Euclidean Heuristic (Fast Mode).
 * Assumes the graph is in integer coordinates (microdegrees)
 * and weights are in decimeters.
 */
int Algorithm::h(int n, double cos_lat_goal) {
  const Coord &a = graph_.coords[n];
  const Coord &b = graph_.coords[goal_];

  // 1. Direct differences (in microdegrees)
  long long dlat = std::abs(a.lat - b.lat);
  long long dlon = std::abs(a.lon - b.lon);

  // 2. Longitude adjustment by latitude (equirectangular projection)
  // Scales the longitude difference by the cosine of the
  // average/goal latitude.
  double dlon_scaled = dlon * cos_lat_goal;

  // 3. Euclidean distance in "map units"
  // We use double for the sum of squares to avoid long long overflow.
  double dist_sq = (double)(dlat * dlat) + (dlon_scaled * dlon_scaled);

  // 4. Square root and conversion to decimeters
  double dist_raw = std::sqrt(dist_sq);

  return static_cast<int>(dist_raw * FINAL_FACTOR);
}

AlgorithmResult Algorithm::run() {
  auto start_time = std::chrono::high_resolution_clock::now();

  // 1. Precompute the cosine of the goal's latitude for the projection
  // Convert from microdegrees to radians: (lat / 10^6) * (PI / 180)
  double lat_rad = (graph_.coords[goal_].lat / 1000000.0) * (M_PI / 180.0);
  double cos_lat_goal = std::cos(lat_rad);

  // 2. Reset data structures
  std::fill(g_.begin(), g_.end(), INF_INT);
  std::fill(parent_.begin(), parent_.end(), -1);
  std::fill(closed_.begin(), closed_.end(), 0);

  open_.clear();

  std::size_t expansions = 0;

  // 3. Initial node
  g_[start_] = 0;
  int start_h = h(start_, cos_lat_goal);

  // Direct push (id, f_score)
  open_.push(start_, 0 + start_h);

  // 4. Main loop
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
    int gu = g_[u]; // Local cache

    for (auto it = begin; it != end; ++it) {
      int v = *it;

      // Edge index in CSR
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

  // 5. Path reconstruction
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

  // Reset data structures
  std::fill(g_.begin(), g_.end(), INF);
  std::fill(parent_.begin(), parent_.end(), -1);
  std::fill(closed_.begin(), closed_.end(), 0);
  open_.clear();

  std::size_t expansions = 0;

  g_[start_] = 0.0;
  open_.push(start_, 0.0);

  // Main loop
  while (!open_.empty()) {
    Node current = open_.pop();
    int u = current.id;

    if (closed_[u])
      continue;

    // Close node
    closed_[u] = 1;
    expansions++;

    // If goal was reached, stop
    if (u == goal_)
      break;

    // Iterate through neighbours
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

  // Reconstruct path
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