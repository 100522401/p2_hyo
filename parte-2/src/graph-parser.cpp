#include "graph-parser.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

GraphParser::GraphParser(const std::string &dataset_name) {
  // Construct file paths based on dataset name
  co_file = dataset_name + ".co";
  gr_file = dataset_name + ".gr";
}

Graph GraphParser::parse_debug() {
  Graph g;

  // Parse nodes with timing
  std::cout << "Reading nodes from " << co_file << " ..." << std::endl;
  auto start = std::chrono::high_resolution_clock::now();
  parseNodes(g);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Node parsing time: " << elapsed.count() << " seconds.\n";

  // Parse edges with timing
  std::cout << "Reading edges from " << gr_file << " ..." << std::endl;
  start = std::chrono::high_resolution_clock::now();
  parseEdges(g);
  end = std::chrono::high_resolution_clock::now();
  elapsed = end - start;
  std::cout << "Edge parsing time: " << elapsed.count() << " seconds.\n";

  std::cout << "Graph loaded. Number of nodes: " << g.nodes.size() << std::endl;

  return g;
}

Graph GraphParser::parse() {
  Graph g;
  // Parse nodes and edges without debug output
  parseNodes(g);
  parseEdges(g);
  return g;
}

void GraphParser::parseNodes(Graph &g) const {
  // Open the coordinates file
  std::ifstream in(co_file);
  if (!in.is_open()) {
    std::cerr << "Error opening file: " << co_file << std::endl;
    return;
  }

  std::string line;
  while (std::getline(in, line)) {
    // Skip empty lines and comments
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream iss(line);
    std::string prefix;
    int id, lat_micro, lon_micro;
    iss >> prefix >> id >> lon_micro >> lat_micro; // note: lon first

    // Only process lines starting with 'v'
    if (prefix != "v")
      continue;

    // Convert from microdegrees to degrees
    double lat = lat_micro / 1e6;
    double lon = lon_micro / 1e6;

    // Add node to graph
    GNode node{id, lon, lat, {}};
    g.addNode(node);
  }

  in.close();
  std::cout << "Read " << g.nodes.size() << " nodes from " << co_file
            << std::endl;
}

void GraphParser::parseEdges(Graph &g) const {
  // Open the edges file
  std::ifstream in(gr_file);
  if (!in.is_open()) {
    std::cerr << "Error opening file: " << gr_file << std::endl;
    return;
  }

  std::string line;
  int edge_count = 0;
  while (std::getline(in, line)) {
    // Skip empty lines and comments
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream iss(line);
    std::string prefix;
    int u, v, distance;
    iss >> prefix >> u >> v >> distance;

    // Only process lines starting with 'a'
    if (prefix != "a")
      continue;

    // Add edge to graph
    g.addEdge(u, v, distance);
    edge_count++;
  }

  in.close();
  std::cout << "Read " << edge_count << " edges from " << gr_file << std::endl;
}
