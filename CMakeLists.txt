cmake_minimum_required(VERSION 3.10)
project(VectorSearchEngine VERSION 1.0)

# Enforce C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tell CMake where to look for header files (.hpp)
include_directories(include)

# Compile the main executable using these specific files
add_executable(vector_engine src/main.cpp src/math_utils.cpp src/hnsw_index.cpp)