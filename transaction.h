#ifndef GRID_COMPRESSION_TRANSACTION_H
#define GRID_COMPRESSION_TRANSACTION_H

#include "graph.h"

#ifdef LOCAL
#include <vector>
#include <functional>
#else
ESC#include <vector>
ESC#include <functional>
#endif

class transaction {
    enum class type {
        insert, erase, swap
    };

    struct entry {
        enum type type;
        graph::vertex vertex;
        graph::vertex swap_vertex;
    };

    std::function<void(graph::vertex)> insert;
    std::function<void(graph::vertex)> erase;
    std::function<void(graph::vertex, graph::vertex)> swap;

    std::vector<entry> history;

public:
    transaction(std::function<void(graph::vertex)> insert,
                std::function<void(graph::vertex)> erase,
                std::function<void(graph::vertex, graph::vertex)> swap)
    : insert(insert), erase(erase), swap(swap) {}

    auto begin() const {
        return history.rbegin();
    }

    auto end() const {
        return history.rend();
    }

    void record_insert(graph::vertex vertex) {
        history.push_back({type::insert, vertex, graph::null_vertex});
    }

    void record_erase(graph::vertex vertex) {
        history.push_back({type::erase, vertex, graph::null_vertex});
    }

    void record_swap(graph::vertex erased, graph::vertex inserted) {
        history.push_back({type::swap, erased, inserted});
    }

    void clear() {
        history.clear();
    }

    void rollback() {
        for (auto &entry: *this) {
            if (entry.type == type::swap) {
                swap(entry.swap_vertex, entry.vertex);
            } else if (entry.type == type::insert) {
                erase(entry.vertex);
            } else {
                insert(entry.vertex);
            }
        }
        clear();
    }
};

#endif //GRID_COMPRESSION_TRANSACTION_H
