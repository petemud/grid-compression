#ifndef GRID_COMPRESSION_FREE_VERTICES_H
#define GRID_COMPRESSION_FREE_VERTICES_H

#include <vector>
#include "../graph.h"

std::vector<graph::vertex> free;

void on_insert(graph::vertex vertex) {
    for (auto neighbour: neighbors[vertex]) {
        neighbors_in_solution[neighbour].insert(vertex);
    }
}

#endif //GRID_COMPRESSION_FREE_VERTICES_H
