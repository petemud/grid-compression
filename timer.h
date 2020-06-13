#ifndef GRID_COMPRESSION_TIMER_H
#define GRID_COMPRESSION_TIMER_H

#ifdef LOCAL
#include <ctime>
#else
ESC#include <ctime>
#endif

class timer {
    std::clock_t start_point;

public:
    float max_duration;

    explicit timer(float max_duration): max_duration(max_duration) {
        start();
    }

    void start() {
        start_point = clock();
    }

    auto duration() const {
        return float(clock() - start_point) / CLOCKS_PER_SEC;
    }

    bool timeout() const {
        return duration() > max_duration;
    }
};


#endif //GRID_COMPRESSION_TIMER_H
