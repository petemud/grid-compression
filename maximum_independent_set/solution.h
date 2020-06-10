#ifndef GRID_COMPRESSION_SOLUTION_H
#define GRID_COMPRESSION_SOLUTION_H

#include <initializer_list>
#include <any>
#include "../graph.h"

template<typename ...InsertListeners>
struct solution {
    std::initializer_list<std::any> insert_listeners;

    solution(InsertListeners... insert_listeners): insert_listeners(insert_listeners...) {}

    void insert(graph::vertex vertex) {
        for (auto listener: insert_listeners) {
            listener.insert(vertex);
        }
    }
};

#endif //GRID_COMPRESSION_SOLUTION_H
