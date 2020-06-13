#ifndef GRID_COMPRESSION_INT_SET_H
#define GRID_COMPRESSION_INT_SET_H

#ifdef LOCAL
#include <vector>
#else
ESC#include <vector>
#endif

template<typename UInt = unsigned, bool with_contains = false>
class int_set {
    std::vector<unsigned> position;
    std::vector<UInt> data;

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
    std::enable_if_t<!check_unique> insert(UInt val) {
        position[val] = data.size();
        data.push_back(val);
    }

    template<bool check_unique>
    std::enable_if_t<check_unique> insert(UInt val) {
        if (!contains(val))
            insert(val);
    }

    template<bool check_existence = false>
    std::enable_if_t<!check_existence> erase(UInt val) {
        auto last_val = data.back();
        position[last_val] = position[val];
        data[position[last_val]] = last_val;
        data.pop_back();
    }

    template<bool check_existence>
    std::enable_if_t<check_existence> erase(UInt val) {
        if (contains(val))
            erase(val);
    }

    void swap(UInt old_val, UInt new_val) {
        position[new_val] = position[old_val];
        data[position[new_val]] = new_val;
    }

    bool contains(UInt val) {
        static_assert(with_contains);
        return position[val] < data.size() && data[position[val]] == val;
    }

    void clear() {
        data.clear();
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
