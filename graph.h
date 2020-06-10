#ifndef GRID_COMPRESSION_GRAPH_H
#define GRID_COMPRESSION_GRAPH_H

//#include <vector>

class graph {
public:
    struct vertex {
        typedef uint_least32_t index_type;
        index_type index;

//        vertex() = default;
//        explicit constexpr vertex(index_type index): index(index) {}

        operator index_type() const {
            return index;
        }

        vertex &operator++() {
            ++index;
            return *this;
        }

        bool operator!=(const vertex &other) const {
            return index != other.index;
        }

        bool operator<(const vertex &other) const {
            return index < other.index;
        }
    };

    struct vertex_iterator {
        class vertex vertex;

//        vertex_iterator(class vertex vertex): vertex(vertex) {}

        class vertex operator*() {
            return vertex;
        }

        vertex_iterator &operator++() {
            ++vertex;
            return *this;
        }

        bool operator!=(const vertex_iterator &other) const {
            return vertex != other.vertex;
        }

    };

    size_t maximum_degree = 0;
    std::vector<std::vector<vertex>> adjacency_list;

    graph() = default;

    graph(size_t maximum_order, size_t maximum_degree = 0)
    : maximum_degree(maximum_degree) {
        reserve(maximum_order);
    }

    void reserve(size_t maximum_order) {
        adjacency_list.reserve(maximum_order);
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

    auto begin() const {
        return vertex_iterator{vertex{0}};
    }

    auto end() const {
        return vertex_iterator{vertex{adjacency_list.size()}};
    }
};

#endif //GRID_COMPRESSION_GRAPH_H
