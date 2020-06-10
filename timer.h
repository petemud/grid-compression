#ifndef GRID_COMPRESSION_TIMER_H
#define GRID_COMPRESSION_TIMER_H

//#include <chrono>

template<typename Duration, typename Clock = std::chrono::steady_clock>
class timer {
    typename Clock::time_point start_point;

public:
    Duration max_duration;

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


#endif //GRID_COMPRESSION_TIMER_H
