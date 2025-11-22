#ifndef GRAPH_PARSER_HPP
#define GRAPH_PARSER_HPP

#include "graph-utils.hpp"
#include <string>

class GraphParser {
public:
  // Constructor that receives file name without extension (e.g. USA_map)
  GraphParser(const std::string &dataset_name);

  // Función principal: parsea y devuelve el grafo completo
  Graph parse();

private:
  // Attributes
  std::string co_file; // e.g. USA_map.co
  std::string gr_file; // e.g. USA_map.gr

  // Parsing methods
  void parseNodes(Graph &g) const;
  void parseEdges(Graph &g) const;
};

#endif
