#include <bits/stdc++.h>
#include "graph.h"

# 1 "main.cpp"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "main.cpp"







# 1 "random_picker.h" 1





template<typename Generator = std::minstd_rand0>
class random_picker {
    Generator generator;

public:
    random_picker() = default;

    explicit random_picker(typename Generator::result_type seed): generator(seed) {}




    template<typename Int>
    Int operator()(Int min, Int max) {
        std::uniform_int_distribution<Int> distribution(min, max);
        return distribution(generator);
    }



    template<typename Container>
    auto operator()(const Container& c) {
        return c[operator()<decltype(c.size())>(0, c.size() - 1)];
    }
};

template<typename Generator = std::mt19937>
random_picker<Generator> make_random_picker() {
    return random_picker<Generator>();
}

template<typename Generator = std::mt19937>
random_picker<Generator> make_random_picker(typename Generator::result_type seed) {
    return random_picker<Generator>(seed);
}
# 9 "main.cpp" 2
# 1 "timer.h" 1





template<typename Duration, typename Clock = std::chrono::steady_clock>
class timer {
    typename Clock::time_point start_point;
    Duration max_duration;

public:
    explicit timer(Duration max_duration): max_duration(max_duration) {
        start();
    }

    void start() {
        start_point = Clock::now();
    }

    auto duration() const {
        return Clock::now() - start_point;
    }

    bool timeout() const {
        return duration() > max_duration;
    }
};

template<typename Duration>
timer<Duration> make_timer(Duration max_duration) {
    return timer<Duration>(max_duration);
}
# 10 "main.cpp" 2
# 1 "input.h" 1
# 9 "input.h"
# 1 "graph.h" 1





template<typename Vertex>
struct graph {
    struct edge {
        int v1;
        int v2;
    };

    std::vector<std::vector<int>> adjacency_list;
    std::vector<edge> edges;
    std::vector<Vertex> vertices;
    int neighbours_size = 0;

    graph() = default;

    graph(size_t vertices_size, size_t edges_size, size_t neighbours_size = 0): neighbours_size(neighbours_size) {
        reserve(vertices_size, edges_size);
    }

    int add_vertex(Vertex vertex) {
        int number = vertices.size();
        vertices.push_back(vertex);
        adjacency_list.emplace_back();
        adjacency_list.back().reserve(neighbours_size);
        return number;
    }

    void add_edge(int v1, int v2) {
        int e = edges.size();
        adjacency_list[v1].push_back(e);
        adjacency_list[v2].push_back(e);
        edges.push_back({v1, v2});
    }

    void reserve(size_t vertices_size, size_t edges_size) {
        adjacency_list.reserve(vertices_size);
        edges.reserve(edges_size);
    }
};
# 10 "input.h" 2

class input {
    static constexpr int MAX_GRID_SIZE = 250;
    static constexpr int MAX_RECT_SIZE = 10;
    int h, w, n, m, t;
    int s[MAX_GRID_SIZE][MAX_GRID_SIZE];
    int partial_sum[MAX_GRID_SIZE + 1][MAX_GRID_SIZE + 1];
    int vertices[2][MAX_GRID_SIZE + MAX_RECT_SIZE][MAX_GRID_SIZE + MAX_RECT_SIZE];

    void init_partial_sums() {
        for (int row = 0; row < h; ++row) partial_sum[row][0] = 0;
        for (int col = 0; col < w; ++col) partial_sum[0][col] = 0;
        for (int row = 0; row < h; ++row) {
            for (int col = 0; col < w; ++col) {
                partial_sum[row + 1][col + 1] =
                        +partial_sum[row + 1][col]
                        +partial_sum[row][col + 1]
                        -partial_sum[row][col]
                        +s[row][col];
            }
        }
    }

    int sum(int row1, int col1, int row2, int col2) {
        row1 = std::max(row1, 0);
        row2 = std::min(row2, h);
        col1 = std::max(col1, 0);
        col2 = std::min(col2, w);
        return
                +partial_sum[row1][col1]
                -partial_sum[row1][col2]
                -partial_sum[row2][col1]
                +partial_sum[row2][col2];
    }

public:
    explicit input(std::istream &source) {
        source >> h >> w >> n >> m >> t;
        t = t * n * m;
        for (int row = 0; row < h; ++row) {
            for (int col = 0; col < w; ++col) {
                source >> s[row][col];
            }
        }
        init_partial_sums();
    }

    class rectangle {
        int r1, c1, r2, c2;

    public:
        rectangle(int r2, int c2, int n, int m)
        : r1(r2 - n), c1(c2 - m), r2(r2 - 1), c2(c2 - 1) {}

        friend std::ostream &operator<<(std::ostream &destination, const rectangle &r) {
            return destination << r.r1 << " " << r.c1 << " " << r.r2 << " " << r.c2;
        }
    };

    graph<rectangle> construct_graph() {
        int dirs = n != m ? 2 : 1;
        int max_vertices = (w + n - 2) * (h + m - 2) + (w + m - 2) * (h + n - 2);
        int max_neighbours = dirs * (2 * n - 1) * (2 * m - 1);
        int max_edges = max_vertices * max_neighbours;
        graph<rectangle> g(max_vertices, max_edges, max_neighbours);

        for (int dir1 = 0; dir1 < dirs; ++dir1) {
            int n1 = dir1 ? n : m;
            int m1 = dir1 ? m : n;
            int row1_begin = 1, row1_end = h + n1;
            int col1_begin = 1, col1_end = w + m1;
            for (int row1 = row1_begin; row1 < row1_end; ++row1) {
                for (int col1 = col1_begin; col1 < col1_end; ++col1) {
                    if (sum(row1 - n1, col1 - m1, row1, col1) < t) {
                        vertices[dir1][row1][col1] = -1;
                        continue;
                    }
                    int v1 = vertices[dir1][row1][col1] = g.add_vertex({row1, col1, n1, m1});

                    for (int dir2 = 0; dir2 < dirs; ++dir2) {
                        if (dir2 > dir1) break;
                        int n2 = dir2 ? n : m;
                        int m2 = dir2 ? m : n;
                        int row2_begin = std::max(row1 - n1 + 1, 1), row2_end = std::min(row1 + n2, h + n2);
                        int col2_begin = std::max(col1 - m1 + 1, 1), col2_end = std::min(col1 + m2, w + m2);
                        for (int row2 = row2_begin; row2 < row2_end; ++row2) {
                            if (dir2 == dir1 && row2 > row1) break;
                            for (int col2 = col2_begin; col2 < col2_end; ++col2) {
                                if (dir2 == dir1 && row2 == row1 && col2 >= col1) break;
                                int v2 = vertices[dir2][row2][col2];
                                if (v2 < 0) continue;
                                g.add_edge(v1, v2);
                            }
                        }
                    }
                }
            }
        }

        return g;
    }
};
# 11 "main.cpp" 2
# 1 "numvc.h" 1





# 1 "int_set.h" 1





class int_set {
    std::vector<int> position;

public:
    std::vector<int> data;

    void insert(int val) {
        position[val] = data.size();
        data.push_back(val);
    }

    void erase(int val) {
        int last_val = data.back();
        position[last_val] = position[val];
        data[position[val]] = last_val;
        data.pop_back();
    }

    void reserve(int n) {
        data.reserve(n);
        position.reserve(n);
    }

    size_t size() const {
        return data.size();
    }

    bool empty() const {
        return data.empty();
    }

    auto operator[](int index) const {
        return data[index];
    }

    auto begin() {
        return data.begin();
    }

    auto end() {
        return data.end();
    }
};
# 7 "numvc.h" 2

template<typename Graph, typename RandomGenerator>
class numvc {
    const std::vector<std::vector<int>> &adjacency_list;
    const std::vector<typename Graph::edge> &edges;

    RandomGenerator &random;

    int_set cover_vertices;
    std::vector<char> vertex_in_cover;
    int_set uncovered_edges;

    std::vector<int> weight;
    std::vector<int> dscore;
    std::vector<int> timestamp;

    std::vector<char> conf_change;
    int taboo_remove;

    int average_weight=1;
    int delta_total_weight=0;
    int weight_threshold;
    float p_scale=0.3;

public:
    std::vector<char> minimum_vertex_cover;

    numvc(const Graph &g, RandomGenerator &random)
    : adjacency_list(g.adjacency_list),
      edges(g.edges), random(random)
    {
        int vertices_number = adjacency_list.size();
        int edges_number = edges.size();

        cover_vertices.reserve(vertices_number);
        vertex_in_cover.resize(vertices_number, false);
        uncovered_edges.reserve(edges_number);

        weight.resize(edges_number, 1);
        dscore.resize(vertices_number, 0);
        timestamp.resize(vertices_number, 0);

        conf_change.resize(vertices_number, true);
        taboo_remove = -1;

        average_weight = 1;
        delta_total_weight = 0;
        weight_threshold = 0.5 * vertices_number;
        p_scale = 0.3;

        minimum_vertex_cover.reserve(adjacency_list.size());
    }

private:
    template<bool use_taboo = true, bool use_timestamp = true, int steps = 50>
    int find_best_vertex_to_uncover() {
        int best_v = cover_vertices[0];
        for (int v: cover_vertices) {


            if (dscore[v] < dscore[best_v]) continue;
            if (use_taboo && v == taboo_remove) continue;
            if (dscore[v] > dscore[best_v] || use_timestamp && timestamp[v] < timestamp[best_v]) {
                best_v = v;
            }
        }
        return best_v;
    }

    int find_best_vertex_to_cover() {
        int uncovered_edge = random(uncovered_edges);
        int best_v, v1 = edges[uncovered_edge].v1, v2 = edges[uncovered_edge].v2;
        if (!conf_change[v1]) {
            best_v = v2;
        } else if (!conf_change[v2]) {
            best_v = v1;
        } else {
            if (dscore[v1] > dscore[v2] || (dscore[v1] == dscore[v2] && timestamp[v1] < timestamp[v2])) {
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

        for (int edge: adjacency_list[vertex]) {
            int adj = edges[edge].v1 == vertex ? edges[edge].v2 : edges[edge].v1;
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

        int vertex = find_best_vertex_to_uncover();
        remove_cover(vertex);
        return vertex;
    }

    void update_edge_weight() {
        for (int edge: uncovered_edges) {
            weight[edge]++;
            dscore[edges[edge].v1]++;
            dscore[edges[edge].v2]++;
        }

        delta_total_weight += uncovered_edges.size();
        if (delta_total_weight >= edges.size()) {
            average_weight += 1;
            delta_total_weight -= edges.size();
        }

        if(average_weight >= weight_threshold) {
            forget_edge_weights();
        }
    }

    void forget_edge_weights() {
        int total_weight = 0;

        std::fill(dscore.begin(), dscore.end(), 0);

        for (int edge = 0; edge < edges.size(); ++edge) {
            weight[edge] = weight[edge] * p_scale;
            total_weight += weight[edge];

            int v1 = edges[edge].v1, v2 = edges[edge].v2;
            if (!vertex_in_cover[v1] && !vertex_in_cover[v2]) {
                dscore[v1] += weight[edge];
                dscore[v2] += weight[edge];
            } else if (vertex_in_cover[v1] && !vertex_in_cover[v2]) {
                dscore[v1] -= weight[edge];
            } else if (!vertex_in_cover[v1] && vertex_in_cover[v2]) {
                dscore[v2] -= weight[edge];
            }
        }
        average_weight = total_weight / edges.size();
    }

    void update_best_solution() {
        minimum_vertex_cover = vertex_in_cover;





    };

    void construct_cover() {
# 214 "numvc.h"
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
    void run_until_timeout(const Timer &timer) {
        std::cerr << "time: " << timer.duration().count() * 1e-9 << "\n";
        construct_cover();
        std::cerr << "time: " << timer.duration().count() * 1e-9 << "\n";
        std::cerr << "size: " << adjacency_list.size() - cover_vertices.size() << "\n";
        update_best_solution();
        if (cover_vertices.empty()) {
            return;
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

            int best_uncover = find_best_vertex_to_uncover();
            remove_cover(best_uncover);
            timestamp[best_uncover] = step;

            int best_cover = find_best_vertex_to_cover();
            add_cover(best_cover);
            timestamp[best_cover] = step;

            taboo_remove = best_cover;

        }
    }
};

template<typename Graph, typename RandomGenerator>
numvc<Graph, RandomGenerator> make_numvc(const Graph &g, RandomGenerator &random) {
    return numvc<Graph, RandomGenerator>(g, random);
}
# 12 "main.cpp" 2

int main() {
    auto program_timer = make_timer(std::chrono::milliseconds(3980));
    auto random = make_random_picker<std::mt19937>(10);


    std::ios::sync_with_stdio(false);
    input input(std::cin);




    auto g = input.construct_graph();

    auto mvc_solver = make_numvc(g, random);
    mvc_solver.run_until_timeout(program_timer);

    auto minimum_vertex_cover = mvc_solver.minimum_vertex_cover;

    std::vector<int> maximum_independent_set;
    maximum_independent_set.reserve(minimum_vertex_cover.size());
    for (int vertex = 0; vertex < minimum_vertex_cover.size(); ++vertex) {
        if (!minimum_vertex_cover[vertex]) {
            maximum_independent_set.push_back(vertex);
        }
    }
    std::cout << maximum_independent_set.size() << "\n";
    for (int vertex: maximum_independent_set) {
        std::cout << g.vertices[vertex] << "\n";
    }
    std::cerr << "result vertices: " << maximum_independent_set.size() << "\n";
    std::cerr << "total time: " << program_timer.duration().count() * 1e-9 << "\n";
}
