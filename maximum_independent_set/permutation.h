#ifndef GRID_COMPRESSION_PERMUTATION_H
#define GRID_COMPRESSION_PERMUTATION_H

#include <vector>
#include "../graph.h"

class permutation {
    std::vector<graph::vertex> vertices;
    std::vector<unsigned> positions;
};

#endif //GRID_COMPRESSION_PERMUTATION_H
