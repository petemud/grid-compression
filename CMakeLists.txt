cmake_minimum_required(VERSION 3.16)
project(grid-compression)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions($<$<CONFIG:Debug>:FILE_INPUT>)
add_compile_definitions($<$<CONFIG:Debug>:LOCAL>)


add_compile_definitions($<$<CONFIG:Release>:FILE_INPUT>)
add_compile_definitions($<$<CONFIG:Release>:LOCAL>)

add_executable(grid-compression main.cpp random_picker.h timer.h io.h graph.h numvc.h int_set.h maximum_independent_set/iterated_local_search.h algorithm.h maximum_independent_set/free_vertices.h maximum_independent_set/solution.h maximum_independent_set/transaction.h maximum_independent_set/permutation.h)