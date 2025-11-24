#ifndef GRAPH_UTILS_HPP
#define GRAPH_UTILS_HPP
#include <vector>

/* Defines an edge in CSR */
struct GEdge {
  int to;
  int distance;
};

/* Defines a node with coordinates and offset to edges */
struct GNode {
  int id;     // Node ID (1-based)
  double cx;  // longitude
  double cy;  // latitude
  int offset; // starting index in edges array
};

class Graph {
public:
  Graph() = default;

  // Constructor with reserved sizes
  Graph(size_t num_nodes, size_t num_edges) {
    nodes.reserve(num_nodes);
    edges.reserve(num_edges);
  }

  // Add node with given coordinates
  inline void addNode(double cx, double cy) {
    int id = static_cast<int>(nodes.size()) + 1; // IDs start at 1
    int offset = static_cast<int>(edges.size());
    nodes.push_back({id, cx, cy, offset});
  }

  // Add edge directly at current position
  inline void addEdge(int idx, int to, int distance) {
    edges[idx] = {to, distance};
  }

  // Get node by ID (1-based)
  inline const GNode &getNode(int id) const { return nodes[id - 1]; }

  // Get edges of a node
  inline std::vector<GEdge> getEdges(int node_index) const {
    int start = nodes[node_index].offset;
    int end = (node_index + 1 < nodes.size()) ? nodes[node_index + 1].offset
                                              : edges.size();
    return std::vector<GEdge>(edges.begin() + start, edges.begin() + end);
  }

  std::vector<GNode> nodes;
  std::vector<GEdge> edges;
};

#endif
