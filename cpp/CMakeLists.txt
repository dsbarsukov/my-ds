cmake_minimum_required(VERSION 3.26)
project(my_ds)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Google Test
set(GTest_DIR "/usr/local/Cellar/googletest/1.17.0/lib/cmake/GTest")

enable_testing()

find_package(GTest REQUIRED)

# Boost нужен для boost::unordered_flat_map
find_package(Boost REQUIRED)

# nanoflann нужен для KDTree benchmark
find_package(nanoflann CONFIG REQUIRED)

# =========================
# Tests
# =========================

add_executable(test_hash_table
        tests/test_hash_table.cpp
)

target_include_directories(test_hash_table PRIVATE
        src
)

target_link_libraries(test_hash_table PRIVATE
        GTest::gtest_main
)

add_executable(test_binary_heap
        tests/test_binary_heap.cpp
)

target_include_directories(test_binary_heap PRIVATE
        src
)

target_link_libraries(test_binary_heap PRIVATE
        GTest::gtest_main
)

add_executable(test_kd_tree
        tests/test_kd_tree.cpp
)

target_include_directories(test_kd_tree PRIVATE
        src
)

target_link_libraries(test_kd_tree PRIVATE
        GTest::gtest_main
)

# =========================
# Benchmarks
# =========================

add_executable(benchmark_hash_table
        benchmarks/benchmark_hash_table.cpp
)

target_include_directories(benchmark_hash_table PRIVATE
        src
        benchmarks
)

target_link_libraries(benchmark_hash_table PRIVATE
        Boost::headers
)

add_executable(benchmark_binary_heap
        benchmarks/benchmark_binary_heap.cpp
)

target_include_directories(benchmark_binary_heap PRIVATE
        src
        benchmarks
)

add_executable(benchmark_kd_tree
        benchmarks/benchmark_kd_tree.cpp
)

target_include_directories(benchmark_kd_tree PRIVATE
        src
        benchmarks
)

target_link_libraries(benchmark_kd_tree PRIVATE
        nanoflann::nanoflann
)

# =========================
# Register tests with CTest
# =========================

add_test(NAME test_hash_table COMMAND test_hash_table)
add_test(NAME test_binary_heap COMMAND test_binary_heap)
add_test(NAME test_kd_tree COMMAND test_kd_tree)