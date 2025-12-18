#include "graph_parser.hpp"
#include "logger.hpp"
#include <chrono>
#include <cmath>
#include <fstream>
#include <vector>

using namespace std;
using namespace std::chrono;

struct Edge {
  int u;
  int v;
  int w;
};

GraphParser::GraphParser(const string &dataset_name) {
  gr_file = dataset_name + ".gr";
  co_file = dataset_name + ".co";
}

inline void jumpSpaces(const char *&p, const char *end) {
  while (p < end && (*p == ' ' || *p == '\t')) {
    p++;
  }
}

inline void jumpChars(const char *&p, const char *end) {
  while (p < end && *p != ' ') {
    p++;
  }
}

inline int fastAtoi(const char *&p, const char *end) {
  int x = 0;
  while (p < end) {
    unsigned char c = *p;
    if (c < '0' || c > '9')
      break;
    x = x * 10 + (c - '0');
    p++;
  }
  return x;
}

inline int fastAtoiSigned(const char *&p, const char *end) {
  bool neg = false;
  if (p < end && *p == '-') {
    neg = true;
    p++;
  }
  int x = 0;
  while (p < end) {
    unsigned char c = *p;
    if (c < '0' || c > '9')
      break;
    x = x * 10 + (c - '0');
    p++;
  }
  if (neg) {
    x = -x;
  }
  return x;
}

Graph GraphParser::parse() {
  Logger::print_load_graph(gr_file);

  ifstream fin(gr_file);
  if (!fin.is_open()) {
    Logger::error("No se pudo abrir el archivo: " + gr_file);
    return Graph();
  }

  string line;
  int n = 0;
  int m = 0;
  bool header_read = false;

  std::vector<int> count;
  std::vector<Edge> edges;

  // Read header
  while (getline(fin, line)) {
    if (line.empty()) {
      continue;
    }

    // pointer to line characters
    const char *p = line.data();
    const char *end = p + line.size();

    // Skip leading spaces
    jumpSpaces(p, end);

    if (p == end || *p == 'c') {
      continue;
    }

    if (*p == 'p') {
      p++; // skip 'p' character

      jumpSpaces(p, end);

      // Skip "sp"
      jumpChars(p, end);
      jumpSpaces(p, end);

      // Parse n
      n = fastAtoi(p, end);

      // Skip n
      jumpSpaces(p, end);

      // Parse m
      m = fastAtoi(p, end);

      // initialize count vector with n positions
      count.assign(n, 0);

      header_read = true;
      continue;
    }

    if (!header_read) {
      continue;
    }

    // EDGES
    if (*p == 'e') {
      p++; // skip 'a' or 'e' character

      // skip spaces after 'a' or 'e' and get to u
      jumpSpaces(p, end);

      // Read source node u
      int u = fastAtoi(p, end);
      jumpSpaces(p, end);

      //  Read destination node v
      int v = fastAtoi(p, end);

      // Convert to 0-based indexing
      u -= 1;
      v -= 1;
      int w = 1;
      // add to counter
      if (u >= 0 && u < n) {
        count[u]++;
        // cerr << "[DEBUG] count[" << u << "] = " << count[u] << endl;
        edges.push_back({u, v, w});
      }
    } else if (*p == 'a') {
      p++; // skip 'a' or 'e' character

      // skip spaces after 'a' or 'e' and get to u
      jumpSpaces(p, end);

      // Read source node u
      int u = fastAtoi(p, end);
      jumpSpaces(p, end);

      //  Read destination node v
      int v = fastAtoi(p, end);
      jumpSpaces(p, end);

      // Read weight
      int w = fastAtoi(p, end);

      // Convert to 0-based indexing
      u -= 1;
      v -= 1;
      // add to counter
      if (u >= 0 && u < n) {
        count[u]++;
        // cerr << "[DEBUG] count[" << u << "] = " << count[u] << endl;
        edges.push_back({u, v, w});
      }
    }
  }

  fin.close();

  /**
   * CREATE CSR GRAPH
   */

  Graph g(n, m);

  // Create row_ptr
  g.row_ptr[0] = 0;
  for (int i = 0; i < n; i++) {
    g.row_ptr[i + 1] = g.row_ptr[i] + count[i];
  }

  // Create temp_ptr
  std::vector<int> temp_ptr = g.row_ptr;

  // Insert edges into CSR
  for (const auto &e : edges) {
    int pos = temp_ptr[e.u]++;
    g.col_idx[pos] = e.v;
    g.weights[pos] = e.w;
  }
  fin.close();

  parseNodes(g);

  return g;
}

Graph GraphParser::parse_with_stats() {
  auto start = high_resolution_clock::now();
  Graph g = parse();
  auto end = high_resolution_clock::now();
  long long duration = duration_cast<milliseconds>(end - start).count();
  Logger::print_graph_stats(duration, g.n, g.m);
  return g;
}

void GraphParser::parseNodes(Graph &g) const {
  ifstream fin(co_file);
  if (!fin.is_open()) {
    Logger::error("No se pudo abrir el archivo .co: " + co_file);
    return;
  }

  string line;

  while (getline(fin, line)) {
    if (line.empty() || line[0] == 'c') {
      continue;
    }

    const char *p = line.data();
    const char *end = p + line.size();

    // Skip leading spaces
    jumpSpaces(p, end);
    if (p == end || *p != 'v') {
      continue;
    }

    p++; // skip 'v'
    jumpSpaces(p, end);

    int id = fastAtoi(p, end);

    jumpSpaces(p, end);

    int lon_int = fastAtoiSigned(p, end);
    jumpSpaces(p, end);

    // read latitude
    int lat_int = fastAtoiSigned(p, end);

    g.coords[id - 1] = {lon_int, lat_int};
  }

  fin.close();
}
