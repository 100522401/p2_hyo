#ifndef GRAPH_PARSER_HPP
#define GRAPH_PARSER_HPP

#include "graph_utils.hpp"
#include <string>

class GraphParser {
public:
  // Constructor that receives the file name without extension (e.g. USA_map)
  GraphParser(const std::string &dataset_name);

  // Main function
  Graph parse();
  Graph parse_with_stats();

private:
  // Attributes
  std::string co_file; // e.g., USA_map.co
  std::string gr_file; // e.g., USA_map.gr

  // Parsing methods
  void parseNodes(Graph &g) const;
  void parseEdges(Graph &g) const;
};

#endif
