#ifndef GRID_COMPRESSION_NUMVC_H
#define GRID_COMPRESSION_NUMVC_H

//#include <vector>

#include "int_set.h"
#include "graph.h"

template<typename RandomGenerator>
class numvc {
    const class graph &graph;

    RandomGenerator &random;

    int_set cover_vertices;
    int_set uncovered_edges;
    std::vector<char> vertex_in_cover;
    std::vector<char> vertex_in_minimum_cover;

    std::vector<int> weight;
    std::vector<int> dscore;
    std::vector<int> timestamp;

    std::vector<char> conf_change;
    int taboo_remove;

    int		average_weight=1;
    int		delta_total_weight=0;
    int		weight_threshold;
    float	p_scale=0.3;

public:

    numvc(const class graph &graph, RandomGenerator &random)
    : graph(graph), random(random)
    {
        cover_vertices.reserve(graph.order());
        vertex_in_cover.resize(graph.order(), false);
        vertex_in_minimum_cover.reserve(graph.order());
        uncovered_edges.reserve(graph.size());

        weight.resize(graph.size(), 1);
        dscore.resize(graph.order(), 0);
        timestamp.resize(graph.order(), 0);

        conf_change.resize(graph.size(), true);
        taboo_remove = -1;

        average_weight = 1;
        delta_total_weight = 0;
        weight_threshold = 0.08 * sqrt(graph.order());
        p_scale = 0.9;
    }

private:
    template<bool use_conf_change = true, bool use_taboo = true, bool use_timestamp = true, int steps = 20>
    int find_best_vertex(const std::vector<int> &vertices) {
        int best_v = vertices[0];
        for (int v: vertices) {
//        for (int count = 0; count < steps; ++count) {
//            int v = random(cover_vertices);
            if (use_conf_change && !conf_change[v]) continue;
//            if (dscore[v] < dscore[best_v]) continue;
            if (use_taboo && v == taboo_remove) continue;
//            if (dscore[v] > dscore[best_v] || use_timestamp && timestamp[v] < timestamp[best_v]) {
            if (timestamp[v] < timestamp[best_v]) {
                best_v = v;
            }
        }
        return best_v;
    }

    template<int edges_number = 3>
    int find_best_vertex_to_cover() {
        int uncovered_edge = random(uncovered_edges);
        int best_v, v1 = graph.edges[uncovered_edge].v1, v2 = graph.edges[uncovered_edge].v2;
        if (!conf_change[v1]) {
            best_v = v2;
        } else if (!conf_change[v2]) {
            best_v = v1;
        } else {
//            if (dscore[v1] > dscore[v2] || (dscore[v1] == dscore[v2] && timestamp[v1] < timestamp[v2])) {
            if (timestamp[v1] < timestamp[v2]) {
                best_v = v1;
            } else {
                best_v = v2;
            }
        }
        return best_v;
    }

    void add_cover(int vertex) {
        cover_vertices.insert(vertex);
        vertex_in_cover[vertex] = true;

        dscore[vertex] = -dscore[vertex];

        for (int edge: graph.adjacency_list[vertex]) {
            int adj = graph.edges[edge].v1 == vertex ? edges[edge].v2 : edges[edge].v1;
            if (!vertex_in_cover[adj]) {
                uncovered_edges.erase(edge);
                dscore[adj] -= weight[edge];
                conf_change[adj] = true;
            } else {
                dscore[adj] += weight[edge];
            }
        }
    }

    void remove_cover(int vertex) {
        cover_vertices.erase(vertex);
        vertex_in_cover[vertex] = false;

        dscore[vertex] = -dscore[vertex];
        conf_change[vertex] = false;

        for (int edge: adjacency_list[vertex]) {
            int adj = edges[edge].v1 == vertex ? edges[edge].v2 : edges[edge].v1;
            if (!vertex_in_cover[adj]) {
                uncovered_edges.insert(edge);
                dscore[adj] += weight[edge];
                conf_change[adj] = true;
            } else {
                dscore[adj] -= weight[edge];
            }
        }
    }

    int reduce_cover() {
        int vertex = find_best_vertex(cover_vertices.data);
        remove_cover(vertex);
        return vertex;
    }

//    void update_edge_weight() {
//        for (int edge: uncovered_edges) {
//            weight[edge]++;
//            dscore[edges[edge].v1]++;
//            dscore[edges[edge].v2]++;
//        }
//
//        delta_total_weight += uncovered_edges.size();
//        if (delta_total_weight >= edges.size()) {
//            average_weight += 1;
//            delta_total_weight -= edges.size();
//        }
//
//        if(average_weight >= weight_threshold) {
//            forget_edge_weights();
//        }
//    }

//    void forget_edge_weights() {
//        int total_weight = 0;
//
//        std::fill(dscore.begin(), dscore.end(), 0);
//
//        for (int edge = 0; edge < edges.size(); ++edge) {
//            weight[edge] = weight[edge] * p_scale;
//            total_weight += weight[edge];
//
//            int v1 = edges[edge].v1, v2 = edges[edge].v2;
//            if (!vertex_in_cover[v1] && !vertex_in_cover[v2]) {
//                dscore[v1] += weight[edge];
//                dscore[v2] += weight[edge];
//            } else if (vertex_in_cover[v1] && !vertex_in_cover[v2]) {
//                dscore[v1] -= weight[edge];
//            } else if (!vertex_in_cover[v1] && vertex_in_cover[v2]) {
//                dscore[v2] -= weight[edge];
//            }
//        }
//        average_weight = total_weight / edges.size();
//    }

    void update_best_solution() {
        vertex_in_minimum_cover = vertex_in_cover;
    };

    void construct_cover() {
//        for (int vertex = 0; vertex < adjacency_list.size(); ++vertex) {
//            cover_vertices.insert(vertex);
//            vertex_in_cover[vertex] = true;
//        }
//        int best_vertex = -1;

//        while (uncovered_edges.empty() && !cover_vertices.empty()) {
//            best_vertex = reduce_cover();
//        }
//        while (uncovered_edges.empty() && !cover_vertices.empty()) {
//            std::vector<int> best_vertices(1, cover_vertices[0]);
//            for (int v: cover_vertices) {
//                if (dscore[v] > dscore[best_vertices[0]]) {
//                    best_vertices.clear();
//                    best_vertices.push_back(v);
//                } else if (dscore[v] == dscore[best_vertices[0]]) {
//                    best_vertices.push_back(v);
//                }
//            }
//            best_vertex = random(best_vertices);
//            remove_cover(best_vertex);
//        }
//        if (best_vertex >= 0) {
//            add_cover(best_vertex);
//        }


//        for (int edge = 0; edge < edges.size(); ++edge) {
//            int v1 = edges[edge].v1, v2 = edges[edge].v2;
//            uncovered_edges.insert(edge);
//            dscore[v1]++;
//            dscore[v2]++;
//        }
//        std::vector<int> best_vertices;
//        while (!uncovered_edges.empty()) {
//            best_vertices.clear();
//            int best_score = 0;
//            for (int v = 0; v < adjacency_list.size(); ++v) {
//                if (dscore[v] > best_score) {
//                    best_score = dscore[v];
//                    best_vertices.clear();
//                }
//                if (dscore[v] == best_score) {
//                    best_vertices.push_back(v);
//                }
//            }
//            add_cover(random(best_vertices));
//        }


        // EdgeGreedy
        for (int edge = 0; edge < edges.size(); ++edge) {
            int v1 = edges[edge].v1, v2 = edges[edge].v2;
            uncovered_edges.insert(edge);
            dscore[v1]++;
            dscore[v2]++;
        }
        for (int edge = 0; edge < edges.size(); ++edge) {
            int v1 = edges[edge].v1, v2 = edges[edge].v2;
            if (!vertex_in_cover[v1] && !vertex_in_cover[v2]) {
                int best = dscore[v1] > dscore[v2] ? v1 : v2;
                add_cover(best);
            }
        }
        for (int vertex: cover_vertices) {
            if (dscore[vertex] == 0) {
                remove_cover(vertex);
            }
        }
    }

public:
    template<typename Timer, int timer_test_interval = 100>
    auto run_until_timeout(const Timer &timer) {
        std::cerr << "vertices: " << adjacency_list.size() << "; edges: " << edges.size() << "\n";
        std::cerr << "time: " << timer.duration().count() * 1e-9 << "\n";
        construct_cover();
        std::cerr << "time: " << timer.duration().count() * 1e-9 << "\n";
        std::cerr << "size: " << adjacency_list.size() - cover_vertices.size() << "\n";
        update_best_solution();
        if (cover_vertices.empty()) {
            return vertex_in_minimum_cover;
        }

        for (int step = 0; ; step++) {
            if (uncovered_edges.empty()) {
                update_best_solution();
                if (cover_vertices.empty()) break;
                else {
                    reduce_cover();
                    continue;
                }
            }

            if (step % timer_test_interval == 0 && timer.timeout()) {
                std::cerr << "steps: " << step << "\n";
                break;
            }

            int best_uncover = find_best_vertex(cover_vertices.data);
            remove_cover(best_uncover);
            timestamp[best_uncover] = step;

            int best_cover = find_best_vertex_to_cover();
            add_cover(best_cover);
            timestamp[best_cover] = step;

            taboo_remove = best_cover;

//            update_edge_weight();
        }
        return vertex_in_minimum_cover;
    }
};

template<typename RandomGenerator>
numvc<RandomGenerator> make_numvc(const graph &graph, RandomGenerator &random) {
    return numvc<RandomGenerator>(graph, random);
}

#endif //GRID_COMPRESSION_NUMVC_H
