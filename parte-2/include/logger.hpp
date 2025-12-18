#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cmath>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

namespace Logger {

// --- COLORES ANSI (Opcional: Si da problemas en Windows antiguo, déjalos
// vacíos) ---
const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string CYAN = "\033[36m";

// --- UTILIDADES DE FORMATO ---

// Estructura para poner separadores de miles (1,000,000)
struct ThousandsSeparator : std::numpunct<char> {
  char do_thousands_sep() const override {
    return '.';
  } // Usa punto o coma según prefieras
  std::string do_grouping() const override { return "\3"; }
};

// Formatea números enteros con separadores: 1000000 -> "1.000.000"
inline std::string fmt_int(long long n) {
  std::stringstream ss;
  ss.imbue(std::locale(std::locale(), new ThousandsSeparator));
  ss << n;
  return ss.str();
}

// Formatea velocidad: 3500000 -> "3.50 M/s"
inline std::string fmt_speed(size_t expansions, long long ms) {
  if (ms == 0)
    return "Inf M/s";
  double nodes_sec = (expansions * 1000.0) / ms;
  std::stringstream ss;
  ss << std::fixed << std::setprecision(2) << (nodes_sec / 1000000.0) << " M/s";
  return ss.str();
}

// --- FUNCIONES DE LOGGING ---

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
// Imprime estadísticas de carga del grafo
inline void print_graph_stats(long long ms, int nodes, int edges) {

  std::cout << "[" << GREEN << " OK " << RESET << "]  Grafo cargado en " << BOLD
            << (ms / 1000.0) << "s" << RESET << "\n";
  std::cout << "        -> Vértices:    " << fmt_int(nodes) << "\n";
  std::cout << "        -> Arcos:       " << fmt_int(edges) << "\n";
  std::cout << "------------------------------------------------------------\n";
}

// Imprime estadísticas de un algoritmo (A* o Dijkstra)
// alg_name: "A* (Bucket)" o "Dijkstra"
inline void print_alg_stats(const std::string &alg_name, long long ms,
                            size_t expansions, double cost) {
  // Color del tag según el algoritmo
  std::string color =
      (alg_name.find("A*") != std::string::npos) ? CYAN : YELLOW;

  std::cout << "\n[" << color << alg_name << RESET << "] Resultados:\n";

  // Tabla alineada internamente
  std::cout << "        -> Tiempo:      " << BOLD << fmt_int(ms) << " ms"
            << RESET << "\n";
  std::cout << "        -> Expansiones: " << fmt_int(expansions) << "\n";
  std::cout << "        -> Velocidad:   " << fmt_speed(expansions, ms) << "\n";
  // Coste formateado como entero si es grande, o científico si se prefiere
  std::cout << "        -> " << BOLD
            << "Coste:       " << fmt_int((long long)cost) << RESET << "\n";
}

// Comparación final (si se corren ambos)
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

// Para errores
inline void error(const std::string &msg) {
  std::cerr << "[" << RED << "ERROR" << RESET << "] " << msg << std::endl;
}
} // namespace Logger

#endif // LOGGER_HPP