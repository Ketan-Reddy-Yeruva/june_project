// benchmarks/perf_benchmark.cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "math_utils.hpp"
#include "hnsw_index.hpp"

using namespace std::chrono;

int main() {
    std::cout << "=== HNSW Performance Benchmark Suite ===\n\n";

    // 1. Benchmark Parameters
    const int NUM_VECTORS = 50000;
    const int DIMENSIONS = 128;
    const int NUM_QUERIES = 100;

    std::cout << "Dataset Size: " << NUM_VECTORS << " vectors\n";
    std::cout << "Dimensionality: " << DIMENSIONS << "D\n";
    std::cout << "Queries to run: " << NUM_QUERIES << "\n\n";

    // 2. Generate Random Dataset
    std::mt19937 gen(42); // Fixed seed for reproducibility
    // std::mt19random_engine gen(42); // Fixed seed for reproducibility
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    std::vector<std::vector<float>> database(NUM_VECTORS, std::vector<float>(DIMENSIONS));
    for (int i = 0; i < NUM_VECTORS; ++i) {
        for (int d = 0; d < DIMENSIONS; ++d) {
            database[i][d] = dist(gen);
        }
    }

    // Generate Random Queries
    std::vector<std::vector<float>> queries(NUM_QUERIES, std::vector<float>(DIMENSIONS));
    for (int i = 0; i < NUM_QUERIES; ++i) {
        for (int d = 0; d < DIMENSIONS; ++d) {
            queries[i][d] = dist(gen);
        }
    }

    // 3. Build the Graph
    VectorEngine::HNSWIndex index;
    std::cout << "Building HNSW Graph... (This may take a few seconds)\n";
    
    auto start_build = high_resolution_clock::now();
    for (int i = 0; i < NUM_VECTORS; ++i) {
        index.add_vector(i, database[i]);
    }
    auto end_build = high_resolution_clock::now();
    std::cout << "Graph built in " << duration_cast<milliseconds>(end_build - start_build).count() << " ms\n\n";

    // 4. Race 1: Brute Force Scan O(N)
    std::vector<int> brute_force_results(NUM_QUERIES);
    std::cout << "Running Brute-Force Scan...\n";
    auto start_bf = high_resolution_clock::now();
    
    for (int q = 0; q < NUM_QUERIES; ++q) {
        int best_id = -1;
        float min_dist = 999999.0f;
        for (int i = 0; i < NUM_VECTORS; ++i) {
            float d = VectorEngine::euclidean_distance(queries[q], database[i]);
            if (d < min_dist) {
                min_dist = d;
                best_id = i;
            }
        }
        brute_force_results[q] = best_id;
    }
    auto end_bf = high_resolution_clock::now();
    double bf_time_ms = duration_cast<milliseconds>(end_bf - start_bf).count();

    // 5. Race 2: HNSW Graph Search O(log N)
    std::vector<int> hnsw_results(NUM_QUERIES);
    std::cout << "Running HNSW Graph Search...\n";
    auto start_hnsw = high_resolution_clock::now();
    
    for (int q = 0; q < NUM_QUERIES; ++q) {
        hnsw_results[q] = index.search(queries[q]);
    }
    auto end_hnsw = high_resolution_clock::now();
    double hnsw_time_ms = duration_cast<milliseconds>(end_hnsw - start_hnsw).count();

    // 6. Calculate Recall (Accuracy)
    int correct_matches = 0;
    for (int q = 0; q < NUM_QUERIES; ++q) {
        // Did the graph find the exact same absolute closest vector as brute force?
        if (hnsw_results[q] == brute_force_results[q]) {
            correct_matches++;
        }
    }
    float recall = (float)correct_matches / NUM_QUERIES * 100.0f;

    // 7. Print the Final Report
    std::cout << "\n=== PERFORMANCE REPORT ===\n";
    std::cout << "Brute Force Time:  " << bf_time_ms << " ms\n";
    std::cout << "HNSW Graph Time:   " << hnsw_time_ms << " ms\n";
    
    if (hnsw_time_ms > 0) {
        std::cout << "Speedup Factor:    " << (bf_time_ms / hnsw_time_ms) << "x FASTER\n";
    }
    std::cout << "Recall (Accuracy): " << recall << "%\n";
    std::cout << "==========================\n";

    return 0;
}