#include "graph-parser.hpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

GraphParser::GraphParser(const std::string &dataset_name) {
  co_file = dataset_name + ".co";
  gr_file = dataset_name + ".gr";
}

Graph GraphParser::parse_debug() {
  Graph g;

  std::cout << "Leyendo nodos desde " << co_file << " ..." << std::endl;
  auto start = std::chrono::high_resolution_clock::now();
  parseNodes(g);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Tiempo de parseo de nodos: " << elapsed.count()
            << " segundos.\n";

  std::cout << "Leyendo aristas desde " << gr_file << " ..." << std::endl;
  start = std::chrono::high_resolution_clock::now();
  parseEdges(g);
  end = std::chrono::high_resolution_clock::now();
  elapsed = end - start;
  std::cout << "Tiempo de parseo de aristas: " << elapsed.count()
            << " segundos.\n";

  std::cout << "Grafo cargado. Número de nodos: " << g.nodes.size()
            << std::endl;

  return g;
}

Graph GraphParser::parse() {
  Graph g;
  parseNodes(g);
  parseEdges(g);
  return g;
}

void GraphParser::parseNodes(Graph &g) const {
  std::ifstream in(co_file);
  if (!in.is_open()) {
    std::cerr << "Error al abrir el archivo: " << co_file << std::endl;
    return;
  }

  std::string line;
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream iss(line);
    std::string prefix;
    int id, lat_micro, lon_micro;
    iss >> prefix >> id >> lon_micro >> lat_micro;

    if (prefix != "v")
      continue;

    double lat = lat_micro / 1e6;
    double lon = lon_micro / 1e6;

    GNode node{id, lon, lat, {}};
    g.addNode(node);
  }

  in.close();
  std::cout << "Se han leído " << g.nodes.size() << " nodos desde " << co_file
            << std::endl;
}

void GraphParser::parseEdges(Graph &g) const {
  std::ifstream in(gr_file);
  if (!in.is_open()) {
    std::cerr << "Error al abrir el archivo: " << gr_file << std::endl;
    return;
  }

  std::string line;
  int edge_count = 0;
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream iss(line);
    std::string prefix;
    int u, v, distance;
    iss >> prefix >> u >> v >> distance;

    if (prefix != "a")
      continue;

    g.addEdge(u, v, distance);
    edge_count++;
  }

  in.close();
  std::cout << "Se han leído " << edge_count << " aristas desde " << gr_file
            << std::endl;
}
