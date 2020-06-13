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
#include <functional>

#else
ESC#include <vector>
ESC#include <array>
#endif

template<typename RandomGenerator>
class ils {
    const class graph &graph;
    RandomGenerator &random;
    int_set<graph::vertex, true> one_two_candidates;
    int_set<graph::vertex> zero_tight;
    int_set<graph::vertex> one_tight;
    std::vector<unsigned> solution_neighbor;
    int_set<graph::vertex> nonzero_tight;
    std::vector<unsigned> tightness;
    std::vector<unsigned> taboo;
    class transaction transaction;

    bool with_swaps;

public:
    int_set<graph::vertex, true> solution;

    ils(const class graph &graph, RandomGenerator &random, bool with_swaps = true)
    : graph(graph), random(random), transaction([this](graph::vertex vertex) {
        insert<false, false>(vertex);
    }, [this](graph::vertex vertex) {
        erase<false, false>(vertex);
    }, [this](graph::vertex erased, graph::vertex inserted) {
        swap<false, false>(erased, inserted);
    }), with_swaps(with_swaps) {
        solution.reserve(graph.order());
        tightness.resize(graph.order(), 0);
        solution_neighbor.resize(graph.order(), 0);
        taboo.resize(graph.order(), 0);
        one_tight.reserve(graph.order());

        one_two_candidates.reserve(graph.order());
        nonzero_tight.reserve(graph.order());

        zero_tight.reserve(graph.order());
        for (auto vertex: graph) {
            zero_tight.insert(vertex);
        }
        insert_zero_tight<false>();
        insert_one_two<false>();
    }

    template<bool record_transaction = true, bool update_candidates = true>
    void insert(graph::vertex vertex) {
        solution.insert(vertex);
        if (record_transaction) {
            transaction.record_insert(vertex);
        }
        zero_tight.erase(vertex);
        for (auto neighbor: graph.neighbors(vertex)) {
            solution_neighbor[neighbor] ^= vertex;
            if (tightness[neighbor] == 0) {
                zero_tight.erase(neighbor);
                one_tight.insert(neighbor);
            }
            if (tightness[neighbor] == 1) {
                one_tight.erase(neighbor);
                nonzero_tight.insert(neighbor);
            }
            ++tightness[neighbor];
            if (update_candidates) {
                one_two_candidates.erase<true>(neighbor);
            }
        }
        if (update_candidates) {
            one_two_candidates.insert<true>(vertex);
        }
    }

    template<bool record_transaction = true, bool update_candidates = true>
    void erase(graph::vertex vertex) {
        solution.erase(vertex);
        if (record_transaction)
            transaction.record_erase(vertex);
        zero_tight.insert(vertex);
        for (auto neighbor: graph.neighbors(vertex)) {
            solution_neighbor[neighbor] ^= vertex;
            --tightness[neighbor];
            if (tightness[neighbor] == 0) {
                zero_tight.insert(neighbor);
                one_tight.erase(neighbor);
            }
            if (tightness[neighbor] == 1) {
                one_tight.insert(neighbor);
                nonzero_tight.erase(neighbor);
                if (update_candidates) {
                    for (auto candidate: graph.neighbors(neighbor)) {
                        if (solution.contains(candidate)) {
                            one_two_candidates.insert<true>(candidate);
                            break;
                        }
                    }
                }
            }
        }
    }

    template<bool record_transaction = true, bool update_candidates = true>
    void swap(graph::vertex erased, graph::vertex inserted) {
        solution.swap(erased, inserted);
        if (record_transaction) {
            transaction.record_swap(erased, inserted);
        }
//        nonzero_tight.swap(inserted, erased);
        one_tight.swap(inserted, erased);

        for (auto neighbor: graph.neighbors(erased)) {
            solution_neighbor[neighbor] ^= erased;
            --tightness[neighbor];
            if (neighbor != inserted && tightness[neighbor] == 0) {
                zero_tight.insert(neighbor);
                one_tight.erase(neighbor);
            }
            if (tightness[neighbor] == 1) {
                one_tight.insert(neighbor);
                nonzero_tight.erase(neighbor);
                if (update_candidates) {
                    for (auto candidate: graph.neighbors(neighbor)) {
                        if (solution.contains(candidate)) {
                            one_two_candidates.insert<true>(candidate);
                            break;
                        }
                    }
                }
            }
        }
        for (auto neighbor: graph.neighbors(inserted)) {
            solution_neighbor[neighbor] ^= inserted;
            if (neighbor != erased && tightness[neighbor] == 0) {
                zero_tight.erase(neighbor);
                one_tight.insert(neighbor);
            }
            if (tightness[neighbor] == 1) {
                one_tight.erase(neighbor);
                nonzero_tight.insert(neighbor);
            }
            ++tightness[neighbor];
        }
        if (update_candidates) {
            one_two_candidates.insert<true>(inserted);
        }
    }

    std::vector<graph::vertex> one_tight_neighbors;

    /*! Remove one from solution, insert two 1-tight */
    template<bool record_transaction = true>
    void insert_one_two() {
        while (!one_two_candidates.empty()) {
            auto erase_candidate = random(one_two_candidates);
            one_two_candidates.erase(erase_candidate);

            if (!solution.contains(erase_candidate))
                continue;

            unsigned solutions_number = 0;
            std::array<graph::vertex, 2> insert_candidates;

            {
                auto &neighbors = graph.neighbors(erase_candidate);
                one_tight_neighbors.clear();
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
                    if (random(0, solutions_number++) == 0) {
                        insert_candidates = {neighbor1, neighbor2};
                    }
                });
            }

            if (solutions_number > 0) {
                swap<record_transaction>(erase_candidate, insert_candidates[0]);
//                erase<record_transaction>(erase_candidate);
//                insert<record_transaction>(insert_candidates[0]);
                insert<record_transaction>(insert_candidates[1]);
            }
        }
    }

    template<bool record_transaction = true, bool update_candidates = true>
    bool insert_zero_tight() {
        if (zero_tight.empty()) return false;
        while (!zero_tight.empty()) {
            insert<record_transaction, update_candidates>(random(zero_tight));
        }
        return true;
    }

    template<bool record_transaction = true, bool update_candidates = true>
    void force(graph::vertex vertex) {
        for (auto neighbor: graph.neighbors(vertex)) {
            if (solution.contains(neighbor)) {
                erase<record_transaction, update_candidates>(neighbor);
            }
        }
        insert<record_transaction, update_candidates>(vertex);
    }

    template<bool record_transaction = true, bool update_candidates = true, int size = 4>
    void force(const int_set<graph::vertex> &list) {
        auto forced = 1;
        auto probability = list.size();
        if (random(0, probability) == 0) {
            forced++;
            while (random(0, 1)) forced++;
        }
        for (; !list.empty() && forced; --forced) {
            auto candidate = random(list);
            force<record_transaction, update_candidates>(candidate);
        }
        insert_zero_tight<record_transaction, update_candidates>();
    }

    template<bool record_transaction = true, bool update_candidates = true>
    void swap_to_solution(unsigned improve_time = 1000) {
        static unsigned taboo_counter = 0;
        auto taboo_time = graph.order() / 2;
        for (auto improve = improve_time; improve; --improve) {
            if (one_tight.empty()) return;
            auto vertex = random(one_tight);
            ++taboo_counter;
            if (taboo[vertex] > taboo_counter) continue;
            auto neighbor = graph::vertex{solution_neighbor[vertex]};
            swap<record_transaction, update_candidates>(neighbor, vertex);
            taboo[vertex] = taboo_counter + taboo_time;
            if (insert_zero_tight<record_transaction, update_candidates>())
                improve = improve_time;
        }
    }

    bool iteration() {
        if (graph.order() <= solution.size())
            return false;

        force<true, true>(nonzero_tight);
        insert_one_two();

        static unsigned previous_solution_size = 0;
        if (solution.size() < previous_solution_size) {
            transaction.rollback();
        } else {
            transaction.clear();
            if (with_swaps)
                swap_to_solution<false, false>(1000);
            previous_solution_size = solution.size();
        }

        return true;
    }
};

#endif //GRID_COMPRESSION_ITERATED_LOCAL_SEARCH_H
