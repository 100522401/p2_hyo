#pragma once
#include "closed_list.hpp"
#include "graph_utils.hpp"
#include "node.hpp"
#include "open_list.hpp"
#include <vector>

class Algorithm {
public:
  Algorithm(Graph &g, int start, int goal)
      : graph_(g), start_(start), current_(start), goal_(goal), open_(),
        closed_(g.n) {}

  // Node cost functions
  [[nodiscard]] double f(Node n);
  [[nodiscard]] double g(Node n);
  [[nodiscard]] double h(Node n);
  [[nodiscard]] std::vector<int> astar();

private:
  // Graph components
  Graph const &graph_;
  Node start_;
  Node current_;
  Node goal_;

  // List components
  OpenList open_;
  ClosedList closed_;
};

/*

#pragma once
#include <vector>
#include <queue>
#include <cmath>
#include <limits>

struct Coord {
    double lat;
    double lon;
};

struct Graph {
    int n; // número de nodos
    int m; // número de aristas
    std::vector<int> row_ptr;   // CSR: inicio de vecinos de cada nodo
    std::vector<int> col_idx;   // CSR: vecinos
    std::vector<double> weights; // CSR: pesos de aristas
    std::vector<Coord> coords;  // coordenadas para heurística Haversine
};

class Algorithm {
public:
    Algorithm(Graph &g, int start, int goal)
        : graph_(g), start_(start), goal_(goal),
          g_score(g.n, std::numeric_limits<double>::infinity()),
          parent(g.n, -1) {}

    // Heurística Haversine
    [[nodiscard]] double h(int u) const {
        double lat1 = graph_.coords[u].lat * M_PI / 180.0;
        double lon1 = graph_.coords[u].lon * M_PI / 180.0;
        double lat2 = graph_.coords[goal_].lat * M_PI / 180.0;
        double lon2 = graph_.coords[goal_].lon * M_PI / 180.0;

        double dlat = lat2 - lat1;
        double dlon = lon2 - lon1;

        double a = sin(dlat / 2) * sin(dlat / 2) +
                   cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
        double c = 2 * atan2(sqrt(a), sqrt(1 - a));
        const double R = 6371000; // radio de la Tierra en metros
        return R * c;
    }

    [[nodiscard]] double g(int u) const { return g_score[u]; }
    [[nodiscard]] double f(int u) const { return g(u) + h(u); }

    std::vector<int> astar() {
        using PQNode = std::pair<double, int>; // {f_score, node_id}
        std::priority_queue<PQNode, std::vector<PQNode>, std::greater<>> open;

        g_score[start_] = 0.0;
        parent[start_] = -1;
        open.push({f(start_), start_});

        std::vector<bool> closed(graph_.n, false);

        while (!open.empty()) {
            int u = open.top().second;
            open.pop();

            if (u == goal_) break;

            if (closed[u]) continue;
            closed[u] = true;

            for (int idx = graph_.row_ptr[u]; idx < graph_.row_ptr[u + 1];
++idx) { int v = graph_.col_idx[idx]; double weight = graph_.weights[idx];

                if (closed[v]) continue;

                double tentative_g = g_score[u] + weight;
                if (tentative_g < g_score[v]) {
                    g_score[v] = tentative_g;
                    parent[v] = u;
                    open.push({f(v), v});
                }
            }
        }

        // Reconstruir camino
        std::vector<int> path;
        if (g_score[goal_] == std::numeric_limits<double>::infinity())
            return path; // sin camino

        for (int v = goal_; v != -1; v = parent[v])
            path.push_back(v);

        std::reverse(path.begin(), path.end());
        return path;
    }

private:
    Graph const &graph_;
    int start_;
    int goal_;
    std::vector<double> g_score;
    std::vector<int> parent;
};

*/