#pragma once
#include "closed_list.hpp"
#include "graph_utils.hpp"
#include "open_list.hpp"

class Algorithm {
public:
  Algorithm(Graph const &g, int start, int goal);

  // Node cost functions
  [[nodiscard]] double f(int n);
  [[nodiscard]] double g(int n);
  [[nodiscard]] double h(int n);

private:
  // Graph components
  Graph const &g_;
  int start_;
  int current_;
  int goal_;

  // List components
  OpenList open_;
  ClosedList closed_;
};