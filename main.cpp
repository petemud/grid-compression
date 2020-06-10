//#include <iostream>
//#ifdef FILE_INPUT
//    #include <fstream>
//#endif

#include <bits/stdc++.h>

#include "random_picker.h"
#include "timer.h"
#include "io.h"
#include "maximum_independent_set/iterated_local_search.h"
//#include "numvc.h"

template<typename Iteration, typename Timer>
void run_until_timeout(Iteration iteration, const Timer &timer) {
    int iterations = 0;
    while (iteration()) {
        if (iterations % 100 == 0 && timer.timeout())
            break;
        ++iterations;
    }
    std::cerr << "iterations: " << iterations << "\n";
}

template<typename Rep, typename Period>
std::ostream& operator<<(std::ostream& ostream, const std::chrono::duration<Rep, Period> &duration) {
    auto count = duration.count() * Period::num;
    return ostream << count / Period::den << "." << count % Period::den;
}

int main() {
    auto program_timer = make_timer(std::chrono::milliseconds(3980));
    auto random = make_random_picker<std::mt19937>(13);

#ifndef FILE_INPUT
    std::ios::sync_with_stdio(false);
    io io(std::cin, std::cout);
#else
    std::ifstream input_file("input.txt", std::ifstream::in);
    io io(input_file, std::cout);
#endif
    auto graph = io.construct_graph();

    std::cerr << "time: " << program_timer.duration() << "\n";
    ils ils(graph, random);
    std::cerr << "time: " << program_timer.duration() << "\n";
    run_until_timeout([&ils]() {
        return ils.iteration();
    }, program_timer);

    auto &maximum_independent_set = ils.solution;
    //io.output(maximum_independent_set);

//    auto mvc_solver = make_numvc(graph, random);
//    auto in_minimum_vertex_cover = mvc_solver.run_until_timeout(program_timer);
//
//    std::vector<graph::vertex> maximum_independent_set;
//    maximum_independent_set.reserve(graph.size());
//    for (int vertex = 0; vertex < graph.size(); ++vertex) {
//        if (!in_minimum_vertex_cover[vertex]) {
//            maximum_independent_set.push_back(vertex);
//        }
//    }
//
//    io.output(maximum_independent_set);
#ifdef LOCAL
    std::cerr << "result vertices: " << maximum_independent_set.size() << "\n";
    std::cerr << "total time: " << program_timer.duration() << "\n";

    auto score = maximum_independent_set.size() * 10'000'000ll;
    score /= 1 + io.sum(0, io.h + 1, 0, io.w + 1) / io.tnm;
    std::cerr << "score: " << score << "\n";
#endif
}
