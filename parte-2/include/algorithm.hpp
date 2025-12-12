#pragma once
#include "closed_list.hpp"
#include "graph_utils.hpp"
#include "open_list.hpp"

class Algorithm {
public:
  Algorithm(Graph g, int start, int goal);

  // Heuristic function h(n)
  [[nodiscard]] double h(int n);

private:
  // Graph components
  Graph g_;
  int start_;
  int current_;
  int goal_;

  // List components
  OpenList open_;
  ClosedList closed_;
};