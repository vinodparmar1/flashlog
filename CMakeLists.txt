cmake_minimum_required(VERSION 3.16)

project(flashlog
        VERSION 1.0.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(flashlog_test src/main.cpp)
target_include_directories(flashlog_test PRIVATE include/flashlog)
target_link_libraries(flashlog_test PRIVATE pthread)

# benchmark
find_package(benchmark REQUIRED)

add_executable(bench_flashlog benchmarks/bench_flashlog.cpp)
target_include_directories(bench_flashlog PRIVATE include/flashlog)
target_link_libraries(bench_flashlog PRIVATE pthread benchmark::benchmark)
