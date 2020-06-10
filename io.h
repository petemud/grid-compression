#ifndef GRID_COMPRESSION_INPUT_H
#define GRID_COMPRESSION_INPUT_H

#include "graph.h"

#ifdef LOCAL
#include <vector>
#include <istream>
#include <ostream>
#else
ESC#include <vector>
ESC#include <istream>
ESC#include <ostream>
#endif

constexpr static graph::vertex null_vertex{graph::vertex::index_type(-1)};

class io {
    class rectangle {
        int row1, col1, row2, col2;

    public:
        rectangle() = default;
        rectangle(int row_end, int col_end, int n, int m)
                : row1(row_end - n), col1(col_end - m), row2(row_end - 1), col2(col_end - 1) {}

        friend std::ostream &operator<<(std::ostream &destination, const rectangle &r) {
            return destination << r.row1 << " " << r.col1 << " " << r.row2 << " " << r.col2;
        }
    };

    static constexpr size_t MAX_GRID_SIZE = 250;
    static constexpr size_t MAX_RECT_SIZE = 10;

    int s[MAX_GRID_SIZE][MAX_GRID_SIZE];

    int partial_sum[MAX_GRID_SIZE + 1][MAX_GRID_SIZE + 1];
    graph::vertex vertices[2][MAX_GRID_SIZE + MAX_RECT_SIZE - 1][MAX_GRID_SIZE + MAX_RECT_SIZE - 1];
    std::vector<rectangle> rectangles;

    std::ostream &output;

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

public:
    int h, w;
    int n, m;
    /** T * N * M */
    int tnm;

    io(std::istream &input, std::ostream &output): output(output) {
        int t;
        input >> h >> w >> n >> m >> t;
        tnm = t * n * m;
        for (int row = 0; row < h; ++row) {
            for (int col = 0; col < w; ++col) {
                input >> s[row][col];
            }
        }
        init_partial_sums();
    }

    int sum(int row_begin, int row_end, int col_begin, int col_end) {
        row_begin = std::max(row_begin, 0);
        row_end = std::min(row_end, h);
        col_begin = std::max(col_begin, 0);
        col_end = std::min(col_end, w);
        return
                +partial_sum[row_begin][col_begin]
                -partial_sum[row_begin][col_end]
                -partial_sum[row_end][col_begin]
                +partial_sum[row_end][col_end];
    }

    void operator>>(graph &graph) {
        int dirs = n != m ? 2 : 1;
        int max_order = (w + n - 2) * (h + m - 2)
                        + (w + m - 2) * (h + n - 2);
        int max_degree = dirs * (2 * n - 1) * (2 * m - 1);
        int max_size = max_order * max_degree;
        rectangles.reserve(max_order);
        graph.reserve(max_order, max_degree);

        for (int dir1 = 0; dir1 < dirs; ++dir1) {
            int n1 = dir1 ? n : m,
                m1 = dir1 ? m : n;
            int row1_begin = 1,
                row1_end = h + n1;
            int col1_begin = 1,
                col1_end = w + m1;
            for (int row1 = row1_begin; row1 < row1_end; ++row1) {
                for (int col1 = col1_begin; col1 < col1_end; ++col1) {
                    if (sum(row1 - n1, row1, col1 - m1, col1) < tnm) {
                        vertices[dir1][row1][col1] = null_vertex;
                        continue;
                    }
                    auto v1 = vertices[dir1][row1][col1] = graph.add_vertex();
                    rectangles[v1] = rectangle{row1, col1, n1, m1};

                    for (int dir2 = 0; dir2 < dirs; ++dir2) {
                        if (dir2 > dir1) break;
                        int n2 = dir2 ? n : m,
                            m2 = dir2 ? m : n;
                        int row2_begin = std::max(row1 - n1 + 1, 1),
                            row2_end = std::min(row1 + n2, h + n2);
                        int col2_begin = std::max(col1 - m1 + 1, 1),
                            col2_end = std::min(col1 + m2, w + m2);
                        for (int row2 = row2_begin; row2 < row2_end; ++row2) {
                            if (dir2 == dir1 && row2 > row1) break;
                            for (int col2 = col2_begin; col2 < col2_end; ++col2) {
                                if (dir2 == dir1 && row2 == row1 && col2 >= col1) break;
                                auto v2 = vertices[dir2][row2][col2];
                                if (v2 == null_vertex) continue;
                                graph.add_edge(v1, v2);
                            }
                        }
                    }
                }
            }
        }
    }

    template<typename T>
    void operator<<(const T &maximum_independent_set) {
        output << maximum_independent_set.size() << "\n";
        for (auto vertex: maximum_independent_set) {
            output << rectangles[vertex] << "\n";
        }
    }
};

#endif //GRID_COMPRESSION_INPUT_H
