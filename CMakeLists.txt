cmake_minimum_required(VERSION 3.16)
project(grid-compression)

set(CMAKE_CXX_STANDARD 14)

add_compile_definitions(LOCAL)
add_compile_definitions($<$<CONFIG:Debug>:FILE_INPUT>)
#add_compile_definitions($<$<CONFIG:Release>:FILE_INPUT>)

add_executable(grid-compression main.cpp random_picker.h timer.h io.h graph.h int_set.h iterated_local_search.h algorithm.h transaction.h)
