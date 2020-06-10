#ifndef GRID_COMPRESSION_GRAPH_H
#define GRID_COMPRESSION_GRAPH_H

#ifdef LOCAL
#include <vector>
#else
ESC#include <vector>
#endif

class graph {
public:
    struct vertex {
        typedef unsigned index_type;
        index_type index;

        operator index_type() const {
            return index;
        }

        bool operator<(const vertex &other) const {
            return index < other.index;
        }
    };

    struct vertex_iterator {
        vertex::index_type index;

        class vertex operator*() {
            return {index};
        }

        vertex_iterator &operator++() {
            ++index;
            return *this;
        }

        bool operator!=(const vertex_iterator &other) const {
            return index != other.index;
        }
    };

    vertex::index_type maximum_degree = 0;
    std::vector<std::vector<vertex>> adjacency_list;

    graph() = default;

    graph(vertex::index_type maximum_order, vertex::index_type maximum_degree = 0) {
        reserve(maximum_order, maximum_degree);
    }

    void reserve(vertex::index_type maximum_order, vertex::index_type maximum_degree = 0) {
        adjacency_list.reserve(maximum_order);
        this->maximum_degree = maximum_degree;
    }

    vertex add_vertex() {
        auto vertex_index = adjacency_list.size();
        adjacency_list.emplace_back();
        adjacency_list.back().reserve(maximum_degree);
        return vertex{vertex_index};
    }

    void add_edge(vertex v1, vertex v2) {
        adjacency_list[v1].push_back(v2);
        adjacency_list[v2].push_back(v1);
    }

    const auto &neighbors(vertex vertex) const {
        return adjacency_list[vertex];
    }

    auto order() const {
        return adjacency_list.size();
    }

    vertex_iterator begin() const {
        return {0};
    }

    vertex_iterator end() const {
        return {adjacency_list.size()};
    }
};

#endif //GRID_COMPRESSION_GRAPH_H
