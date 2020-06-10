#ifndef GRID_COMPRESSION_TRANSACTION_H
#define GRID_COMPRESSION_TRANSACTION_H

#include "graph.h"

#ifdef LOCAL
#include <vector>
#else
ESC#include <vector>
#endif

class transaction {
    enum class type {
        insert, erase
    };

    struct entry {
        enum type type;
        graph::vertex vertex;
    };

    std::vector<entry> history;
    bool started = false;

public:
    void insert(graph::vertex vertex) {
        if (started)
            history.push_back({type::insert, vertex});
    }

    void erase(graph::vertex vertex) {
        if (started)
            history.push_back({type::erase, vertex});
    }

    void begin() {
        started = true;
    }

    void commit() {
        started = false;
        history.clear();
    }

    template<typename Insert, typename Erase>
    void rollback(Insert insert, Erase erase) {
        started = false;
        for (auto iter = history.rbegin(), end = history.rend(); iter != end; ++iter) {
            if (iter->type == type::insert) {
                erase(iter->vertex);
            } else {
                insert(iter->vertex);
            }
        }
        history.clear();
    }
};

#endif //GRID_COMPRESSION_TRANSACTION_H
