#define ESC

#include "io.h"
#include "random_picker.h"
#include "timer.h"
#include "iterated_local_search.h"

#ifdef LOCAL
#include <iostream>
#include <fstream>
#else
ESC#include <iostream>
#endif

template<typename Iteration, typename Timer>
void run_until_timeout(Iteration iteration, const Timer &timer) {
    int iterations = 0;
    while (iteration()) {
        if (iterations % 50 == 0 && timer.timeout())
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
    auto duration = std::chrono::milliseconds(3960);
    auto program_timer = timer<decltype(duration)>(duration);
    auto random = random_picker<std::mt19937>(23);
#ifdef LOCAL
    std::ifstream input_file("input.txt", std::ifstream::in);
    io io(input_file, std::cout);
#else
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    io io(std::cin, std::cout);
#endif
    graph graph;
    io >> graph;

    std::cerr << "time: " << program_timer.duration() << "\n";
    ils<decltype(random)> ils(graph, random);
    std::cerr << "time: " << program_timer.duration() << "\n";
    run_until_timeout([&ils]() {
        return ils.iteration();
    }, program_timer);

    auto &maximum_independent_set = ils.solution;
    io << maximum_independent_set;
#ifdef LOCAL
    std::cerr << "result vertices: " << maximum_independent_set.size() << "\n";
    std::cerr << "total time: " << program_timer.duration() << "\n";

    auto score = maximum_independent_set.size() * 10'000'000ll;
    score /= 1 + io.sum(0, io.h + 1, 0, io.w + 1) / io.tnm;
    std::cerr << "score: " << score << "\n";
#endif
}
