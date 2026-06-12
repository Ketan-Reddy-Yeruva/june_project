#pragma once
#include <vector>
#include <unordered_map>
#include <random>
#include "hnsw_node.hpp"

namespace VectorEngine {
    class HNSWIndex {
    private:
        std::unordered_map<int, Node> nodes;
        int entry_point_id;
        int max_current_level;   // Tracks the highest layer currently in our graph
        bool has_entry_point;
        
        // Probability components
        float m_L; 
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution;

        // Core logic helpers
        int generate_random_level();
        int search_layer(int entry_node, const std::vector<float>& query, int layer);

    public:
        HNSWIndex();

        void add_vector(int id, const std::vector<float>& data);
        
        // The new multi-layer search function
        int search(const std::vector<float>& query); 
    };
}


// #pragma once
// #include <vector>
// #include <unordered_map>
// #include <random>
// #include "hnsw_node.hpp"

// namespace VectorEngine {
//     class HNSWIndex {
//     private:
//         std::unordered_map<int, Node> nodes;
//         int entry_point_id;
//         int max_current_level;   // Tracks the highest layer currently in our graph
//         bool has_entry_point;
        
//         // Probability components
//         float m_L; 
//         std::default_random_engine generator;
//         std::uniform_real_distribution<double> distribution;

//         // Core logic helpers
//         int generate_random_level();
//         int search_layer(int entry_node, const std::vector<float>& query, int layer);

//     public:
//         HNSWIndex();

//         void add_vector(int id, const std::vector<float>& data);
        
//         // The new multi-layer search function
//         int search(const std::vector<float>& query); 
//     };
// }




// // include/hnsw_index.hpp
// #pragma once
// #include <vector>
// #include <unordered_map>
// #include "hnsw_node.hpp"

// namespace VectorEngine {
//     class HNSWIndex {
//     private:
//         std::unordered_map<int, Node> nodes; // Fast lookup for nodes by their ID
//         int entry_point_id;                  // The ID of the node where our search always starts
//         bool has_entry_point;                // To check if the graph is empty

//     public:
//         HNSWIndex();

//         // 1. Adds a single vector to our graph
//         void add_vector(int id, const std::vector<float>& data);

//         // 2. The core algorithm: Navigates the graph to find the closest node
//         int greedy_search(const std::vector<float>& query);
//     };
// }


