#include "graph-parser.hpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

GraphParser::GraphParser(const std::string &dataset_name)
    : co_file(dataset_name + ".co"), gr_file(dataset_name + ".gr") {}

Graph GraphParser::parse() { // The const is removed to match the header
  Graph g;
  parseNodes(g);
  parseEdges(g);
  return g;
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

  std::cout << "Graph loaded. Number of nodes: " << g.nodes.size()
            << ", number of edges: " << g.edges.size() << std::endl;

  // Optional: print first 10 nodes and their neighbors for debug
  std::cout << std::fixed << std::setprecision(6);
  for (size_t i = 0; i < g.nodes.size() && i < 10; ++i) {
    const GNode &node = g.getNode(i + 1);
    std::cout << "Node " << node.id << " (" << node.cx << ", " << node.cy
              << ") has "
              << ((i + 1 < g.nodes.size()) ? g.nodes[i + 1].offset - node.offset
                                           : g.edges.size() - node.offset)
              << " neighbors\n";

    const auto &edges = g.getEdges(i);
    for (const auto &edge : edges) {
      std::cout << "    -> Node " << edge.to + 1 << " at distance "
                << edge.distance << "\n";
    }
  }

  return g;
}

void GraphParser::parseNodes(Graph &g) const {
  std::ifstream in(co_file);
  if (!in.is_open()) {
    std::cerr << "Error opening file: " << co_file << std::endl;
    return;
  }

  std::string line;
  int total_nodes = 0;

  // First pass: find total nodes
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream iss(line);
    std::string prefix;
    iss >> prefix;
    if (prefix == "p") {
      std::string aux1, aux2, aux3;
      iss >> aux1 >> aux2 >> aux3 >>
          total_nodes; // Corregido: Leer 'aux', 'sp', 'co' antes del número
      break;
    }
  }

  g.nodes.reserve(total_nodes);
  std::cout << "Total nodes: " << total_nodes << std::endl;

  // Rewind file
  in.clear();
  in.seekg(0, std::ios::beg);

  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream iss(line);
    std::string prefix;
    int id, lon_micro, lat_micro;
    iss >> prefix >> id >> lon_micro >> lat_micro;

    if (prefix != "v")
      continue;

    double lon = lon_micro / 1e6;
    double lat = lat_micro / 1e6;

    g.addNode(lon, lat); // addNode sets id and offset automatically
  }

  in.close();
  std::cout << "Read " << g.nodes.size() << " nodes.\n";
}

void GraphParser::parseEdges(Graph &g) const {
  std::ifstream in(gr_file);
  if (!in.is_open()) {
    std::cerr << "Error opening file: " << gr_file << std::endl;
    return;
  }

  std::string line;
  int total_edges = 0;
  int total_nodes_check = 0;

  // First pass: find the 'p' line to get total nodes and edges
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream iss(line);
    std::string prefix;
    iss >> prefix;
    if (prefix == "p") {
      std::string type;
      iss >> type >> total_nodes_check >>
          total_edges; // Leemos 'sp', num_nodos y num_aristas
      break;
    }
  }

  g.edges.resize(total_edges); // Reservamos memoria para todas las aristas
  std::cout << "Total edges: " << total_edges << std::endl;

  // Step 1: compute out-degree
  std::vector<int> out_degree(g.nodes.size(), 0);
  in.clear();
  in.seekg(0, std::ios::beg);
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    std::istringstream iss(line);
    std::string prefix;
    int u, v, d;
    iss >> prefix >> u >> v >> d;
    if (prefix != "a")
      continue;
    out_degree[u - 1]++;
  }

  // Step 2: set offsets
  g.nodes[0].offset = 0;
  for (size_t i = 1; i < g.nodes.size(); ++i)
    g.nodes[i].offset = g.nodes[i - 1].offset + out_degree[i - 1];

  // Step 3: fill edges
  std::vector<int> current_pos(g.nodes.size());
  for (size_t i = 0; i < g.nodes.size(); ++i)
    current_pos[i] = g.nodes[i].offset;

  in.clear();
  in.seekg(0, std::ios::beg);
  int edge_count = 0;
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    std::istringstream iss(line);
    std::string prefix;
    int u, v, d;
    iss >> prefix >> u >> v >> d;
    if (prefix != "a")
      continue;

    int idx = current_pos[u - 1]++;
    g.edges[idx] = {v - 1, d};
    edge_count++;
  }

  in.close();
  std::cout << "Read " << edge_count << " edges into CSR.\n";
}
