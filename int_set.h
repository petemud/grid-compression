#ifndef GRID_COMPRESSION_INT_SET_H
#define GRID_COMPRESSION_INT_SET_H

#include <vector>

template<typename Int = int, bool with_contains = false>
class int_set {
    std::vector<unsigned> position;
    std::vector<Int> data;

public:
    void reserve(size_t size) {
        data.reserve(size);
        if (with_contains) {
            position.resize(size, 0);
        } else {
            position.reserve(size);
        }
    }

    template<bool check_unique = false>
    void insert(Int val) {
        if (check_unique && contains(val)) return;
        position[val] = data.size();
        data.push_back(val);
    }

    template<bool check_existence = false>
    void erase(Int val) {
        if (check_existence && !contains(val)) return;
        Int last_val = data.back();
        position[last_val] = position[val];
        data[position[last_val]] = last_val;
        data.pop_back();
    }

    bool contains(Int val) {
        static_assert(with_contains);
        return position[val] < data.size() && data[position[val]] == val;
    }

    auto size() const {
        return data.size();
    }

    auto empty() const {
        return data.empty();
    }

    auto operator[](size_t index) const {
        return data[index];
    }

    auto begin() const {
        return data.begin();
    }

    auto end() const {
        return data.end();
    }
};

#endif //GRID_COMPRESSION_INT_SET_H
