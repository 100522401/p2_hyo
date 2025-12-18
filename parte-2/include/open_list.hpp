#ifndef OPEN_LIST_H
#define OPEN_LIST_H

#include <cassert>
#include <limits>
#include <vector>

class OpenList {
private:
  // Un vector de vectores. Cada índice representa un costo f.
  // Usamos int para los IDs de los nodos para ser cache-friendly.
  std::vector<std::vector<int>> buckets_;

  int current_f_; // El cursor que rastrea el mínimo f actual
  int count_;     // Número total de elementos en la openlist
  int width_;     // Tamaño del buffer circular

public:
  // Constructor: width debe ser mayor que el peso máximo de una arista * C
  explicit OpenList(int width = 100000)
      : buckets_(width), current_f_(0), count_(0), width_(width) {}

  inline void push(int u, int f) {
    // Manejo del cursor para retroceso (re-inserción con mejor coste)
    // Aunque con heurística consistente, f nunca debería disminuir por debajo
    // del actual procesado
    if (count_ > 0 && f < current_f_) {
      current_f_ = f;
    } else if (count_ == 0) {
      current_f_ = f;
    }

    buckets_[f % width_].push_back(u);
    count_++;
  }

  inline int pop() {
    if (count_ == 0)
      return -1;

    // Avanzar el cursor hasta encontrar un bucket no vacío
    while (buckets_[current_f_ % width_].empty()) {
      current_f_++;
    }

    int u = buckets_[current_f_ % width_].back();
    buckets_[current_f_ % width_].pop_back();
    count_--;

    return u;
  }

  inline void clear() {
    // Limpieza rápida: no borramos la memoria de los vectores, solo limpiamos
    // Solo limpiamos los buckets que sabemos que tocamos si quisiéramos
    // optimizar más, pero clear() suele ser O(N).
    for (auto &bucket : buckets_) {
      bucket.clear();
    }
    count_ = 0;
    current_f_ = 0;
  }

  inline bool empty() const { return count_ == 0; }
};

#endif