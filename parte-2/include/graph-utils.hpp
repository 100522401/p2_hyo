#ifndef GRAPH_UTILS_HPP
#define GRAPH_UTILS_HPP
#include <vector>

// Forward declaration
struct GEdge;

struct GNode {
  /* Defines a node of a graph */
  int id;

  // Map coordinates (simple precision float is not enough)
  double cx;
  double cy;

  // Edges
  std::vector<GEdge> neighbours;
};

struct GEdge {
  /* Defines an edge belonging to a specific node of a graph */
  int to;
  int distance;
};

class Graph {
  /* Defines a graph using an adjacency list implementation  */
public:
  // Constructor
  Graph() = default;

  // Methods
  inline void addNode(GNode const &node) { nodes.push_back(node); }

  inline void addEdge(int from, int to, int distance) {
    // Adjust indices if IDs start at 1
    nodes[from - 1].neighbours.push_back({to - 1, distance});
  }

  // Getters
  inline const GNode &getNode(int id) const { return nodes[id - 1]; }

  // Attributes
  std::vector<GNode> nodes;
};

#endif
