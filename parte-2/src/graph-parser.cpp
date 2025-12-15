#include "graph_parser.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
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
  ifstream fin(gr_file);
  if (!fin.is_open()) {
    cerr << "[GraphParser] Could not open file: " << gr_file << endl;
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

      cerr << "[DEBUG] Cabecera: n=" << n << " m=" << m << '\n';

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
  cerr << "[DEBUG] row_ptr starts:" << endl;
  for (int i = 0; i < n; i++) {
    g.row_ptr[i + 1] = g.row_ptr[i] + count[i];
  }
  cerr << "[DEBUG] row_ptr ends:" << endl;

  // Create temp_ptr
  std::vector<int> temp_ptr = g.row_ptr;

  cerr << "[DEBUG] edges start:" << endl;

  // Insert edges into CSR
  for (const auto &e : edges) {
    int pos = temp_ptr[e.u]++;
    g.col_idx[pos] = e.v;
    g.weights[pos] = e.w;
  }

  cerr << "[DEBUG] Aristas terminan: " << g.row_ptr[n] << " edges stored.\n";

  fin.close();

  parseNodes(g);

  return g;
}

Graph GraphParser::parse_debug() {
  auto start = high_resolution_clock::now();
  Graph g = parse();
  auto end = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(end - start).count();
  cout << "[GraphParser] Parsing time: " << duration << " ms" << endl;
  cout << "[GraphParser] Number of nodes: " << g.n << endl;
  cout << "[GraphParser] Number of edges: " << g.m << endl;
  return g;
}

void GraphParser::parseNodes(Graph &g) const {
  ifstream fin(co_file);
  if (!fin.is_open()) {
    cerr << "[GraphParser] Could not open .co file: " << co_file << endl;
    return;
  }

  string line;
  cerr << "[DEBUG] nodes start:" << endl;

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

    // DIMACS uses coordinates multiplied by 1e6
    double lon = lon_int / 1e6;
    double lat = lat_int / 1e6;

    g.coords[id - 1] = {lon, lat};
  }
  cerr << "[DEBUG] nodes end:" << endl;

  fin.close();
}
