#include "graph-parser.hpp"

int main() {
  // Initialize parser
  GraphParser parser("../graph-data/USA-road-d.USA");

  // Parse graph
  Graph g = parser.parse_debug();

  // The parse_debug() function already prints the graph details for debugging.

  return 0;
}
