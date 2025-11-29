#ifndef CLOSED_LIST_H
#define CLOSED_LIST_H

#include <vector>
#include <cstddef>
#include <algorithm>  

class ClosedList {
    private:
        std::vector<char> mark; //0 = no abierto, 1 = abierto

    public:
    //Constructor
    ClosedList(size_t n) {
        mark = std::vector<char>(n, 0);
    }

    inline bool membership(size_t v){ //devuelve True si el nodo se a abierto, False caso contrario
        return mark[v] == 1;
    }

    inline void insert(size_t v) {
        mark[v] = 1;
    }
};

#endif