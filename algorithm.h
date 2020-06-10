#ifndef GRID_COMPRESSION_ALGORITHM_H
#define GRID_COMPRESSION_ALGORITHM_H

template<typename InputIterator1, typename InputIterator2, typename Function>
void for_each_difference(InputIterator1 first1, InputIterator1 last1,
        InputIterator2 first2, InputIterator2 last2, Function f) {
    while (first1 != last1 && first2 != last2)
        if (*first1 < *first2)
            f(*first1++);
        else if (*first2 < *first1)
            ++first2;
        else {
            ++first1;
            ++first2;
        }
    while (first1 != last1)
        f(*first1++);
}

#endif //GRID_COMPRESSION_ALGORITHM_H
