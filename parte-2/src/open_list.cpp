#include "../include/open_list.hpp"
#include <algorithm>

void OpenList::insert_neightbors(std::vector<Node> & neightbors) {
    std::vector<Node> result;
    result.reserve(openlist.size() + neightbors.size());

    size_t i = 0, j = 0;

    //merge
    while(i < openlist.size() && j < neightbors.size()){
        //ordeno la lista de mayor a menor para que hacer el pop sea O(1) en vez de O(n)
        if(openlist[i].f > neightbors[j].f){
            result.push_back(openlist[i]);
            i++;
        } else {
            result.push_back(neightbors[j]);
            j++;
        }
    }

    //se copian los restantes

    while(i < openlist.size()){
        result.push_back(openlist[i]);
        i++;
    }
    while (j<neightbors.size())
    {
        result.push_back(neightbors[j]);
        j++;
    }
    
    //intercambia los punteros internos de los dos vectores (más barato que hacer openlist = result)
    openlist.swap(result);
}

Node OpenList::pop() {
    Node n = openlist.back();
    openlist.pop_back();
    return n;
}