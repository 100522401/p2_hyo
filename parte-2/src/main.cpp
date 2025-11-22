#include "graph-parser.hpp"
#include <iomanip>
#include <iostream>

int main() {
  // Initialize parser
  GraphParser parser("../graph-data/USA-road-d.USA");

  // Parse graph
  Graph g = parser.parse();

  // Output results
  std::cout << std::fixed << std::setprecision(6);
  for (size_t i = 0; i < g.nodes.size() && i < 10; ++i) {
    const GNode &node = g.getNode(i + 1);
    std::cout << "Nodo " << node.id << " (" << node.cx << ", " << node.cy
              << ") " << node.neighbours.size() << " vecinos\n";

    for (const auto &edge : node.neighbours) {
      std::cout << "    -> Nodo " << edge.to + 1 << " a distancia "
                << edge.distance << "\n";
    }
  }

  return 0;
}
