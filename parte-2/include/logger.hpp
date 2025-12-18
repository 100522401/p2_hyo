#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cmath>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

namespace Logger {

// --- ANSI Colors
const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string CYAN = "\033[36m";

// --- FORMAT UTILITIES ---

// Struct to add thousands separators (1,000,000)
struct ThousandsSeparator : std::numpunct<char> {
  char do_thousands_sep() const override {
    return '.';
  } // Use dot or comma as you prefer
  std::string do_grouping() const override { return "\3"; }
};

// Formats integers with separators: 1000000 -> "1.000.000"
inline std::string fmt_int(long long n) {
  std::stringstream ss;
  ss.imbue(std::locale(std::locale(), new ThousandsSeparator));
  ss << n;
  return ss.str();
}

// Formats speed: 3500000 -> "3.50 M/s"
inline std::string fmt_speed(size_t expansions, long long ms) {
  if (ms == 0)
    return "Inf M/s";
  double nodes_sec = (expansions * 1000.0) / ms;
  std::stringstream ss;
  ss << std::fixed << std::setprecision(2) << (nodes_sec / 1000000.0) << " M/s";
  return ss.str();
}

// --- LOGGING FUNCTIONS ---

inline void print_header() {
  std::cout << "\n"
            << BOLD
            << "============================================================"
            << RESET << "\n";
  std::cout << "        Problema de Búsqueda y Búsqueda Heurística     "
            << "\n";
  std::cout << BOLD
            << "============================================================"
            << RESET << "\n";
}

inline void info(const std::string &msg) {
  std::cout << "[" << BLUE << "INFO" << RESET << "]  " << msg << std::endl;
}

inline void print_load_graph(std::string const &filename) {
  std::cout << "[" << BLUE << "INFO" << RESET
            << "]  Cargando grafo: " << filename << "...\n";
}
// Prints graph loading statistics
inline void print_graph_stats(long long ms, int nodes, int edges) {

  std::cout << "[" << GREEN << " OK " << RESET << "]  Grafo cargado en " << BOLD
            << (ms / 1000.0) << "s" << RESET << "\n";
  std::cout << "        -> Vértices:    " << fmt_int(nodes) << "\n";
  std::cout << "        -> Arcos:       " << fmt_int(edges) << "\n";
  std::cout << "------------------------------------------------------------\n";
}

// Prints statistics for an algorithm (A* or Dijkstra)
// alg_name: "A*" or "Dijkstra"
inline void print_alg_stats(const std::string &alg_name, long long ms,
                            size_t expansions, double cost) {
  // Tag color depends on the algorithm
  std::string color =
      (alg_name.find("A*") != std::string::npos) ? CYAN : YELLOW;

  std::cout << "\n[" << color << alg_name << RESET << "] Resultados:\n";

  // Internally aligned table
  std::cout << "        -> Tiempo:      " << BOLD << fmt_int(ms) << " ms"
            << RESET << "\n";
  std::cout << "        -> Expansiones: " << fmt_int(expansions) << "\n";
  std::cout << "        -> Velocidad:   " << fmt_speed(expansions, ms) << "\n";
  // Cost formatted as an integer
  std::cout << "        -> " << BOLD
            << "Coste:       " << fmt_int((long long)cost) << RESET << "\n";
}

// Final comparison (if both are run)
inline void print_comparison(double cost_astar, double cost_dijkstra) {
  std::cout
      << "\n------------------------------------------------------------\n";
  long long diff = std::abs((long long)cost_astar - (long long)cost_dijkstra);

  if (diff == 0) {
    std::cout << "[" << GREEN << " OK " << RESET << "]  Los costes coinciden ("
              << fmt_int((long long)cost_astar) << ").\n";
  } else {
    std::cout << "[" << RED << "FAIL" << RESET
              << "]  Discrepancia de costes: " << diff << "\n";
  }
  std::cout
      << "============================================================\n\n";
}

// For errors
inline void error(const std::string &msg) {
  std::cerr << "[" << RED << "ERROR" << RESET << "] " << msg << std::endl;
}
} // namespace Logger

#endif // LOGGER_HPP