#ifndef GRID_COMPRESSION_ITERATED_LOCAL_SEARCH_H
#define GRID_COMPRESSION_ITERATED_LOCAL_SEARCH_H

#include "graph.h"
#include "algorithm.h"
#include "int_set.h"
#include "transaction.h"
#include "random_picker.h"

#ifdef LOCAL
#include <vector>
#include <array>
#else
ESC#include <vector>
ESC#include <array>
#endif

template<typename RandomGenerator>
class ils {
    const class graph &graph;
    RandomGenerator &random;
    int_set<graph::vertex, true> one_two_candidates;
    int_set<graph::vertex, true> zero_tight;
    int_set<graph::vertex, true> nonzero_tight;
    std::vector<unsigned> tightness;
    class transaction transaction;

public:
    int_set<graph::vertex, true> solution;

    ils(const class graph &graph, RandomGenerator &random)
    : graph(graph), random(random) {
        solution.reserve(graph.order());
        tightness.resize(graph.order(), 0);

        one_two_candidates.reserve(graph.order());
        nonzero_tight.reserve(graph.order());

        zero_tight.reserve(graph.order());
        for (auto vertex: graph) {
            zero_tight.insert(vertex);
        }
        insert_all_zero_tight();
        insert_one_two();
    }

    void insert(graph::vertex vertex) {
        solution.insert(vertex);
        transaction.insert(vertex);
        if (tightness[vertex] == 0)
            zero_tight.erase(vertex);
        else
            nonzero_tight.erase(vertex);
        for (auto neighbor: graph.neighbors(vertex)) {
            if (tightness[neighbor] == 0) {
                zero_tight.erase(neighbor);
                nonzero_tight.insert(neighbor);
            }
            ++tightness[neighbor];
            one_two_candidates.erase<true>(neighbor);
        }
        one_two_candidates.insert<true>(vertex);
    }

    void erase(graph::vertex vertex) {
        solution.erase(vertex);
        transaction.erase(vertex);
        if (tightness[vertex] == 0)
            zero_tight.insert(vertex);
        else
            nonzero_tight.insert(vertex);
        for (auto neighbor: graph.neighbors(vertex)) {
            --tightness[neighbor];
            if (tightness[neighbor] == 0) {
                zero_tight.insert(neighbor);
                nonzero_tight.erase(neighbor);
            }
            if (tightness[neighbor] == 1) {
                for (auto candidate: graph.neighbors(neighbor)) {
                    if (solution.contains(candidate)) {
                        one_two_candidates.insert<true>(candidate);
                    }
                }
            }
        }
    }

    /*! Remove one from solution, insert two 1-tight */
    void insert_one_two() {
        while (!one_two_candidates.empty()) {
            auto erase_candidate = random(one_two_candidates);
            one_two_candidates.erase(erase_candidate);

            if (!solution.contains(erase_candidate))
                continue;

            unsigned solutions_number = 0;
            std::array<graph::vertex, 2> insert_candidates;

            std::vector<graph::vertex> one_tight_neighbors;
            {
                auto &neighbors = graph.neighbors(erase_candidate);
                one_tight_neighbors.reserve(neighbors.size());
                for (auto neighbor: neighbors) {
                    if (tightness[neighbor] == 1) {
                        one_tight_neighbors.push_back(neighbor);
                    }
                }
            }

            for (auto neighbor1_iter = one_tight_neighbors.begin(),
                      neighbors_end  = one_tight_neighbors.end();
                 neighbor1_iter != neighbors_end; ++neighbor1_iter)
            {
                auto neighbor1 = *neighbor1_iter;
                auto first = graph.neighbors(neighbor1).begin(),
                     last = graph.neighbors(neighbor1).end();
                auto neighbor2_iter = neighbor1_iter + 1;
                for_each_difference(neighbor2_iter, neighbors_end, first, last,
                [neighbor1, &insert_candidates, &solutions_number, &random = random](auto neighbor2) {
                    if (random(0u, solutions_number++) == 0) {
                        insert_candidates = {neighbor1, neighbor2};
                    }
                });
            }

            if (solutions_number > 0) {
                erase(erase_candidate);
                insert(insert_candidates[0]);
                insert(insert_candidates[1]);
            }
        }
    }

    void insert_all_zero_tight() {
        while (!zero_tight.empty()) {
            insert(random(zero_tight));
        }
    }

    void force(graph::vertex vertex) {
        for (auto neighbor: graph.neighbors(vertex)) {
            if (solution.contains(neighbor)) {
                erase(neighbor);
            }
        }
        insert(vertex);
    }

    void force() {
        force(random(nonzero_tight));
//        auto forced = 1;
//        auto outer_probability = 2 * solution.size();
//        auto inner_probability = 2;
//        if (random(1, outer_probability) == 1) {
//            forced++;
//            while (random(1, inner_probability) == 1) forced++;
//        }
    }

    bool iteration() {
        if (graph.order() <= solution.size()) {
            return false;
        }

        transaction.begin();
        auto solution_before = solution.size();
        force();
        insert_all_zero_tight();
        insert_one_two();
        auto solution_after = solution.size();
        if (solution_after < solution_before) {
            transaction.rollback([this](graph::vertex v) {
                insert(v);
            }, [this](graph::vertex v) {
                erase(v);
            });
        } else {
            transaction.commit();
        }
        return true;
    }
};

#endif //GRID_COMPRESSION_ITERATED_LOCAL_SEARCH_H
