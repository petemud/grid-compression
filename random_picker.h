#ifndef GRID_COMPRESSION_RANDOM_PICKER_H
#define GRID_COMPRESSION_RANDOM_PICKER_H

#ifdef LOCAL
#include <random>
#else
ESC#include <random>
#endif

template<typename Generator = std::minstd_rand0>
class random_picker {
    Generator generator;

public:
    random_picker() = default;

    explicit random_picker(typename Generator::result_type seed): generator(seed) {}

    /**
     * Random integer on the range [min, max]
     */
    template<typename Int>
    Int operator()(Int min, Int max) {
        std::uniform_int_distribution<Int> distribution(min, max);
        return distribution(generator);
    }

    /**
     * Random element in the container
     */
    template<typename Container>
    auto operator()(const Container& c) {
        return c[operator()<decltype(c.size())>(0, c.size() - 1)];
    }
};

#endif //GRID_COMPRESSION_RANDOM_PICKER_H
