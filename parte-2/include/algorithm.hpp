#pragma once
#include "graph_utils.hpp"
#include "open_list.hpp"
#include <limits>
#include <vector>

struct AlgorithmResult {
  std::vector<int> path;
  double cost;
  std::size_t expansions;
  long long ms;
};

class Algorithm {
public:
  static constexpr double INF = std::numeric_limits<double>::max();

  Algorithm(Graph &g, int start, int goal)
      : graph_(g), start_(start), goal_(goal), open_(), closed_(g.n, 0),
        g_(g.n, INF), parent_(g.n, -1) {}

  // Heuristic
  [[nodiscard]] int h(int n, double cos_lat_goal);

  // Main method
  [[nodiscard]] AlgorithmResult run();

  // Dijkstra for comparison
  [[nodiscard]] AlgorithmResult run_dijkstra();

private:
  // Graph components
  Graph const &graph_;
  int start_;
  int goal_;

  // List components
  OpenList open_;
  std::vector<char> closed_;

  // SOA for g(n) and parent(n)
  std::vector<double> g_;
  std::vector<int> parent_;
};
