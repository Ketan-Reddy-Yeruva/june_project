cmake_minimum_required(VERSION 3.10)
project(VectorSearchEngine VERSION 1.0)

# Enforce C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SYSTEM OPTIMIZATION: Enable AVX2 hardware acceleration globally for all builds
add_compile_options(-O3 -march=native -mavx2 -mfma)

# Tell CMake where to look for header files
include_directories(include)

# 1. The old Milestone executable
add_executable(vector_engine src/main.cpp src/math_utils.cpp src/hnsw_index.cpp)

# 2. The Benchmark executable
add_executable(run_benchmark benchmarks/perf_benchmark.cpp src/math_utils.cpp src/hnsw_index.cpp)
