#ifndef GRAPH_UTILS_HPP
#define GRAPH_UTILS_HPP
#include <vector>

// Forward declaration
struct GEdge;

struct GNode {
  /* Defines a node of a graph */
  int id;

  // Map coordinates (simple precission float is enough)
  float cx;
  float cy;

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
  int addNode(int id, float x, float y) {
    int newId = nodes.size();
    nodes.push_back({newId, x, y, {}});
    return newId;
  }

  void addEdge(int from, int to, int distance) {
    nodes[from].neighbours.push_back({to, distance});
  }

  // Getters
  const GNode &getNode(int id) const { return nodes[id]; }

  // Attributes
  std::vector<GNode> nodes;
};

#endif