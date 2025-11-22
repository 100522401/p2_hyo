#include "graph-utils.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

void parseNodes(const std::string &infile, Graph &g) {
  // Open file
  std::ifstream in(infile);
  if (!in.is_open()) {
    std::cerr << "Error al abrir el archivo: " << infile << std::endl;
    return;
  }

  // Read file line by line
  std::string line;
  while (std::getline(in, line)) {
    // Ignore empty and commented lines
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream iss(line);
    std::string prefix;
    int id;
    int lat_micro, lon_micro; // Microgrados

    iss >> prefix >> id >> lon_micro >>
        lat_micro; // nota: lon primero en el archivo

    // Only process lines that start with 'v'
    if (prefix != "v")
      continue;

    // Convert to correct format
    double lat = lat_micro / 1e6;
    double lon = lon_micro / 1e6;

    // Add node
    GNode node{id, lon, lat, {}};
    g.addNode(node);
  }

  in.close();

  std::cout << "Se han leído " << g.nodes.size() << " nodos desde " << infile
            << std::endl;
}

void parseEdges(std::string const &infile, Graph &g) {
  // Open file
  std::ifstream in(infile);
  if (!in.is_open()) {
    std::cerr << "Error al abrir el archivo: " << infile << std::endl;
    return;
  }

  // Read file line by line
  std::string line;
  while (std::getline(in, line)) {
    // Ignore empty and commented lines
    if (line.empty() || line[0] == '#')
      continue;

    // Definitions
    std::istringstream iss(line);
    std::string prefix;
    int id;
    int u, v; // Nodes
    int distance;

    // Structure of the line
    iss >> prefix >> u >> v >> distance;

    // Only process lines that start with 'a'
    if (prefix != "a")
      continue;

    // Add edge
    g.addEdge(u, v, distance);
  }

  in.close();

  std::cout << "Se han leído " << g.nodes.size() << " nodos desde " << infile
            << std::endl;
}

int main() {
  std::string co_file = "../graph-data/USA-road-d.USA.co";
  std::string gr_file = "../graph-data/USA-road-d.USA.gr";

  Graph g;

  std::cout << "Leyendo nodos desde " << co_file << " ..." << std::endl;
  auto start = std::chrono::high_resolution_clock::now();
  parseNodes(co_file, g);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Tiempo de parseo de nodos: " << elapsed.count()
            << " segundos.\n";

  std::cout << "Leyendo aristas desde " << gr_file << " ..." << std::endl;
  start = std::chrono::high_resolution_clock::now();
  parseEdges(gr_file, g);
  end = std::chrono::high_resolution_clock::now();
  elapsed = end - start;
  std::cout << "Tiempo de parseo de aristas: " << elapsed.count()
            << " segundos.\n";

  std::cout << "Grafo cargado. Número de nodos: " << g.nodes.size()
            << std::endl;

  std::cout << std::fixed << std::setprecision(6);
  for (size_t i = 0; i < g.nodes.size() && i < 10; ++i) {
    const GNode &node = g.getNode(i + 1);
    std::cout << "Nodo " << node.id << " con coordenadas (" << node.cx << ", "
              << node.cy << ") y " << node.neighbours.size() << " vecinos:\n";

    for (const auto &edge : node.neighbours) {
      std::cout << "    -> Nodo " << edge.to + 1 << " a distancia "
                << edge.distance << "\n";
    }
  }

  return 0;
}