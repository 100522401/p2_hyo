#include "graph-parser.hpp"
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
    cerr << "[GraphParser] No se pudo abrir el archivo: " << gr_file << endl;
    return Graph();
  }

  string line;
  int n = 0;
  int m = 0;
  bool header_read = false;

  std::vector<int> count;
  std::vector<Edge> edges;

  // Leer cabecera
  while (getline(fin, line)) {
    if (line.empty()) {
      continue;
    }

    // puntero a los caracteres de la línea
    const char *p = line.data();
    const char *end = p + line.size();

    // Se saltan espacios iniciales
    jumpSpaces(p, end);

    if (p == end || *p == 'c') {
      continue;
    }

    if (*p == 'p') {
      p++; // saltamos caracter "p"

      jumpSpaces(p, end);

      // Se saltar sp
      jumpChars(p, end);
      jumpSpaces(p, end);

      // Parseamos n con from_chars (El puntero p no se mueve)
      n = fastAtoi(p, end);

      // Se salta n
      jumpSpaces(p, end);

      // Parseamos m con from_chars
      m = fastAtoi(p, end);

      // inicializamos vector count con n posiciones
      count.assign(n, 0);

      cerr << "[DEBUG] Cabecera: n=" << n << " m=" << m << '\n';

      header_read = true;
      continue;
    }

    if (!header_read) {
      continue;
    }

    // ARISTAS
    if (*p == 'e') {
      p++; // saltamos caracter "a" o "e"

      // saltamos espacios después de "a" o "e" y llegamos a u
      jumpSpaces(p, end);

      // Leer nodo origen u
      int u = fastAtoi(p, end);
      jumpSpaces(p, end);

      //  Leer nodo destino v
      int v = fastAtoi(p, end);

      // Pasamos datos a base 0
      u -= 1;
      v -= 1;
      int w = 1;
      // sumamos al contador
      if (u >= 0 && u < n) {
        count[u]++;
        // cerr << "[DEBUG] count[" << u << "] = " << count[u] << endl;
        edges.push_back({u, v, w});
      }
    } else if (*p == 'a') {
      p++; // saltamos caracter "a" o "e"

      // saltamos espacios después de "a" o "e" y llegamos a u
      jumpSpaces(p, end);

      // Leer nodo origen u
      int u = fastAtoi(p, end);
      jumpSpaces(p, end);

      //  Leer nodo destino v
      int v = fastAtoi(p, end);
      jumpSpaces(p, end);

      // Leer weight
      int w = fastAtoi(p, end);

      // Pasamos datos a base 0
      u -= 1;
      v -= 1;
      // sumamos al contador
      if (u >= 0 && u < n) {
        count[u]++;
        // cerr << "[DEBUG] count[" << u << "] = " << count[u] << endl;
        edges.push_back({u, v, w});
      }
    }
  }

  fin.close();

  /**
   * CREAR GRAFO CSR
   */

  Graph g(n, m);

  // Se crea row_ptr
  g.row_ptr[0] = 0;
  cerr << "[DEBUG] row_ptr empieza:" << endl;
  for (int i = 0; i < n; i++) {
    g.row_ptr[i + 1] = g.row_ptr[i] + count[i];
  }
  cerr << "[DEBUG] row_ptr acaba:" << endl;

  // Se crea temp_ptr
  std::vector<int> temp_ptr = g.row_ptr;

  cerr << "[DEBUG] aristas empiezan:" << endl;

  // Metemos aristas en CSR
  for (const auto &e : edges) {
    int pos = temp_ptr[e.u]++;
    g.col_idx[pos] = e.v;
    g.weights[pos] = e.w;
  }

  cerr << "[DEBUG] Aristas terminan: " << g.row_ptr[n]
       << " aristas almacenadas.\n";

  fin.close();

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
  cerr << "[DEBUG] nodos empieza:" << endl;

  while (getline(fin, line)) {
    if (line.empty() || line[0] == 'c') {
      continue;
    }

    const char *p = line.data();
    const char *end = p + line.size();

    // Saltar espacios iniciales
    jumpSpaces(p, end);
    if (p == end || *p != 'v') {
      continue;
    }

    p++; // saltar 'v'
    jumpSpaces(p, end);

    int id = fastAtoi(p, end);

    jumpSpaces(p, end);

    int lon_int = fastAtoiSigned(p, end);
    jumpSpaces(p, end);

    // leer latitud
    int lat_int = fastAtoiSigned(p, end);

    // DIMACS usa coordenadas multiplicadas por 1e6
    double lon = lon_int / 1e6;
    double lat = lat_int / 1e6;

    g.coords[id - 1] = {lon, lat};
  }
  cerr << "[DEBUG] nodos acaba:" << endl;

  fin.close();
}
