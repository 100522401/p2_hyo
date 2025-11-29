#ifndef CLOSED_H
#define CLOSED_H

#include <vector>
#include <cstddef>
#include <algorithm>  

class Closed {
    private:
        std::vector<char> marks; //0 = no abierto, 1 = abierto

    public:
    //Constructor
    Closed(size_t n) {
        marks = std::vector<char>(n, 0);
    }

    inline bool membership(size_t v){ //devuelve True si el nodo se a abierto, False caso contrario
        return marks[v] == 1;
    }

    inline void insert(size_t v) {
        marks[v] = 1;
    }
};

#endif