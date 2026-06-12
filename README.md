# HNSW Vector Engine

A high-performance, multi-layered Hierarchical Navigable Small World (HNSW) vector search engine built from scratch in C++17. 

This project implements a custom vector database designed to optimize nearest-neighbor search from $O(N)$ linear time to $O(\log N)$ logarithmic time, capable of handling high-dimensional data (e.g., 128D embeddings).

## Key Features

* **Algorithmic Core:** Implements multi-layer spatial graphs, greedy routing, and Beam Search (`efSearch`) for high-recall nearest neighbor lookups.
* **Hardware Acceleration:** Custom math engine utilizing Intel AVX2 SIMD intrinsics and 256-bit CPU registers to process 8 dimensions simultaneously per clock cycle.
* **Systems Optimization:** Designed for maximum cache alignment using flat contiguous memory (`std::vector`) and zero-allocation Epoch Arrays to eliminate heap allocation overhead during graph traversal.
* **Benchmarking Suite:** Built-in performance testing to evaluate Queries-Per-Second (QPS), Speedup Factors, and Recall accuracy against a brute-force baseline.

## Performance Benchmark

Tested on a dataset of **50,000 vectors at 128 Dimensions** (simulating SIFT128). 
By leveraging SIMD and the HNSW graph architecture, the engine successfully achieves >90% recall while significantly reducing the computational load compared to standard linear scans.

## Prerequisites

* CMake (Version 3.10 or higher)
* A C++17 compatible compiler (GCC/Clang)
* A CPU that supports AVX2 instruction sets (Most Intel/AMD processors post-2015)

## Build Instructions

Clone the repository and use CMake to generate the build files. It is highly recommended to build in `Release` mode to enable the `-O3` and `-mavx2` compiler flags.

```bash
git clone [https://github.com/Ketan-Reddy-Yeruva/YOUR_REPO_NAME.git](https://github.com/Ketan-Reddy-Yeruva/YOUR_REPO_NAME.git)
cd YOUR_REPO_NAME

# Generate build files in Release mode
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Compile the project
cmake --build build

```

## Usage

The project compiles into two separate executables:

**1. Run the Benchmarking Suite:**
Stress tests the database, compares HNSW against Brute Force, and prints a performance report (Time, Speedup, Recall).

```bash
./build/run_benchmark

```

**2. Run the Core Engine:**
A smaller, interactive demonstration of inserting vectors and retrieving the closest match.

```bash
./build/vector_engine
