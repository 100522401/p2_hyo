#include "graph-parser.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;
using namespace std::chrono;

GraphParser::GraphParser(const string &dataset_name) {
  gr_file = dataset_name + ".gr";
  co_file = dataset_name + ".co";
}

Graph GraphParser::parse() {
  ifstream fin(gr_file);
  if (!fin.is_open()) {
    cerr << "[GraphParser] No se pudo abrir el archivo: " << gr_file << endl;
    return Graph();
  }

  int n = 0;
  vector<tuple<int, int, int>> edges;
  string line;
  bool reserved = false;

  // Una sola pasada sobre el archivo
  while (getline(fin, line)) {
    if (line.empty() || line[0] == 'c')
      continue;

    stringstream ss(line);
    char type;
    ss >> type;

    if (type == 'p' && !reserved) {
      string format;
      int m;
      ss >> format >> n >> m;
      edges.reserve(m); // Reservar memoria para todas las aristas
      reserved = true;
      continue;
    }

    if (type == 'a') {
      int u, v, w;
      ss >> u >> v >> w;
      edges.emplace_back(u - 1, v - 1, w);
    } else if (type == 'e') {
      int u, v;
      ss >> u >> v;
      edges.emplace_back(u - 1, v - 1, 1);
    }
  }
  fin.close();

  Graph g(n, edges.size());

  // Construcción CSR
  for (auto &[u, v, w] : edges)
    g.row_ptr[u + 1]++;
  for (int i = 1; i <= n; i++)
    g.row_ptr[i] += g.row_ptr[i - 1];

  vector<int> temp_ptr = g.row_ptr;
  for (auto &[u, v, w] : edges) {
    int pos = temp_ptr[u]++;
    g.col_idx[pos] = v;
    g.weights[pos] = w;
  }

  // Parsear coordenadas
  parseNodes(g);

  return g;
}

Graph GraphParser::parse_debug() {
  auto start = high_resolution_clock::now();
  Graph g = parse();
  auto end = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(end - start).count();
  cout << "[GraphParser] Tiempo de parseo: " << duration << " ms" << endl;
  cout << "[GraphParser] Número de nodos: " << g.n << endl;
  cout << "[GraphParser] Número de aristas: " << g.m << endl;
  return g;
}

void GraphParser::parseNodes(Graph &g) const {
  ifstream fin(co_file);
  if (!fin.is_open()) {
    cerr << "[GraphParser] No se pudo abrir el archivo .co: " << co_file
         << endl;
    return;
  }

  string line;
  while (getline(fin, line)) {
    if (line.empty() || line[0] == 'c')
      continue;
    if (line[0] == 'v') {
      int id;
      double x, y;
      stringstream ss(line);
      char type;
      ss >> type >> id >> x >> y;
      g.coords[id - 1] = {x, y};
    }
  }
  fin.close();
}
