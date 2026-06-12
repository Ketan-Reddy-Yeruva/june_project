#include <iostream>
#include <vector>
#include "math_utils.hpp"
#include "hnsw_index.hpp"

int main() {
    std::cout << "--- HNSW Vector Search Engine: Milestone 3 (Multi-Layer) ---\n\n";

    VectorEngine::HNSWIndex index;
    std::vector<std::vector<float>> database;

    std::cout << "Generating 20 vectors and building HNSW Graph...\n";
    // Generate 20 dummy 4-dimensional vectors
    for (int i = 0; i < 20; ++i) {
        std::vector<float> vec = {
            (float)(i * 0.5), (float)(i * 0.1), (float)(i % 3), 1.0f
        };
        database.push_back(vec);
        index.add_vector(i, vec);
    }
    std::cout << "HNSW Graph built successfully!\n\n";

    // A query somewhere in the middle
    std::vector<float> query = {5.2f, 1.3f, 1.1f, 0.8f};

    // Graph Search
    int graph_closest = index.search(query);
    float graph_dist = VectorEngine::euclidean_distance(query, database[graph_closest]);

    std::cout << "[HNSW Multi-Layer Search Result]\n";
    std::cout << "Closest Vector ID: " << graph_closest << " | Distance: " << graph_dist << "\n\n";

    // Brute Force Verification
    int brute_closest = -1;
    float min_dist = 999999.0f;
    for (int i = 0; i < database.size(); ++i) {
        float dist = VectorEngine::euclidean_distance(query, database[i]);
        if (dist < min_dist) {
            min_dist = dist;
            brute_closest = i;
        }
    }

    std::cout << "[Brute Force Verification]\n";
    std::cout << "Closest Vector ID: " << brute_closest << " | Distance: " << min_dist << "\n";

    return 0;
}


// #include <iostream>
// #include <vector>
// #include "math_utils.hpp"
// #include "hnsw_index.hpp"

// int main() {
//     std::cout << "--- HNSW Vector Search Engine: Milestone 2 (Graph Search) ---\n\n";

//     // 1. Our Database
//     std::vector<std::vector<float>> database = {
//         {1.0f, 2.0f, 0.5f, 0.1f}, // ID 0
//         {5.0f, 5.0f, 5.0f, 5.0f}, // ID 1
//         {2.0f, 2.0f, 0.5f, 0.2f}, // ID 2
//         {8.0f, 8.0f, 8.0f, 8.0f}, // ID 3
//         {1.5f, 1.5f, 0.4f, 0.1f}  // ID 4
//     };
//     std::vector<float> query = {0.0f, 0.0f, 0.0f, 0.0f};

//     // 2. Initialize our Graph Index
//     VectorEngine::HNSWIndex graph_index;

//     // 3. Insert vectors into the graph
//     std::cout << "Building Graph Index...\n";
//     for (int i = 0; i < database.size(); ++i) {
//         graph_index.add_vector(i, database[i]);
//     }
//     std::cout << "Graph built successfully!\n\n";

//     // 4. Run the Graph Search
//     int graph_closest_id = graph_index.greedy_search(query);
//     float graph_dist = VectorEngine::euclidean_distance(query, database[graph_closest_id]);

//     // 5. Output the Graph Result
//     std::cout << "[Graph Search Result]  (O(log N) expected)\n";
//     std::cout << "Closest Vector ID: " << graph_closest_id << " | Distance: " << graph_dist << "\n\n";

//     // 6. Output the Brute Force Result (for verification)
//     std::cout << "[Brute Force Result] (O(N) baseline)\n";
//     // We already know from Milestone 1 that ID 4 is the absolute closest
//     float brute_dist = VectorEngine::euclidean_distance(query, database[4]);
//     std::cout << "Closest Vector ID: 4 | Distance: " << brute_dist << "\n";

//     return 0;
// }


// #include <iostream>
// #include <vector>
// #include <queue>
// #include "math_utils.hpp"

// // Struct to hold search results
// struct SearchResult {
//     int id;
//     float distance;
    
//     // Max-Heap comparison: Priority queue will naturally pop the LARGEST distance first.
//     // We want to keep the SMALLEST distances, so we evict the largest ones.
//     bool operator<(const SearchResult& other) const {
//         return distance < other.distance;
//     }
// };

// int main() {
//     std::cout << "--- HNSW Vector Search Engine: Milestone 1 ---\n\n";

//     // 1. Initialize the "Database" (A flat array of vectors in RAM)
//     // Let's pretend these are 4-dimensional vectors.
//     std::vector<std::vector<float>> database = {
//         {1.0f, 2.0f, 0.5f, 0.1f}, // ID 0
//         {5.0f, 5.0f, 5.0f, 5.0f}, // ID 1
//         {2.0f, 2.0f, 0.5f, 0.2f}, // ID 2
//         {8.0f, 8.0f, 8.0f, 8.0f}, // ID 3
//         {1.5f, 1.5f, 0.4f, 0.1f}  // ID 4
//     };

//     // 2. Define the target query vector
//     std::vector<float> query = {0.0f, 0.0f, 0.0f, 0.0f}; 
//     int K = 3; // We want to find the top 3 closest vectors

//     // 3. Setup the Max-Heap (Priority Queue) to track the top K nearest neighbors
//     std::priority_queue<SearchResult> top_k_queue;

//     std::cout << "Scanning a database of " << database.size() << " vectors...\n";

//     // 4. Brute-Force Scan (O(N) complexity)
//     for (int i = 0; i < database.size(); ++i) {
//         float dist = VectorEngine::euclidean_distance(query, database[i]);
        
//         top_k_queue.push({i, dist});
        
//         // If the heap size exceeds K, pop the element with the largest distance
//         if (top_k_queue.size() > K) {
//             top_k_queue.pop(); 
//         }
//     }

//     // 5. Extract and display the results cleanly
//     // Note: Priority queue pops largest first, so we reverse it for display
//     std::vector<SearchResult> results;
//     while (!top_k_queue.empty()) {
//         results.push_back(top_k_queue.top());
//         top_k_queue.pop();
//     }

//     std::cout << "\nTop " << K << " Closest Vectors (Nearest Neighbors):\n";
//     std::cout << "----------------------------------------------\n";
//     // Iterate backwards to print the absolute closest match first
//     for (auto it = results.rbegin(); it != results.rend(); ++it) {
//         std::cout << "Vector ID: " << it->id << " | Euclidean Distance: " << it->distance << "\n";
//     }

//     return 0;
// }