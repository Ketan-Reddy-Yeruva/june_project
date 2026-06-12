#include "hnsw_index.hpp"
#include "math_utils.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_set>
#include <queue>

namespace VectorEngine {

    HNSWIndex::HNSWIndex() : m_L(0.5), distribution(0.0, 1.0) {
        has_entry_point = false;
        entry_point_id = -1;
        max_current_level = 0;
        current_visit_mark = 0; // ADD THIS
    }
    // HNSWIndex::HNSWIndex() : m_L(0.5), distribution(0.0, 1.0) {
    //     has_entry_point = false;
    //     entry_point_id = -1;
    //     max_current_level = 0;
    // }

    // 1. The Math: Assigns a random layer using Geometric Distribution
    int HNSWIndex::generate_random_level() {
        double r = distribution(generator);
        if (r == 0.0) r = 0.00001; // Prevent log(0) error
        return static_cast<int>(-std::log(r) * m_L);
    }

    int HNSWIndex::search_layer(int entry_node, const std::vector<float>& query, int layer) {
        int ef = 80; 
        
        // Advance the "Epoch" - we are on a new search, so we look for the new number!
        current_visit_mark++; 

        std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<>> candidates;
        std::priority_queue<std::pair<float, int>> top_results;

        float initial_dist = euclidean_distance(query, nodes[entry_node].data);
        candidates.push({initial_dist, entry_node});
        top_results.push({initial_dist, entry_node});
        
        // Mark the entry node with our current epoch number
        visited_array[entry_node] = current_visit_mark;

        while (!candidates.empty()) {
            auto current = candidates.top();
            candidates.pop();

            if (current.first > top_results.top().first) {
                break; 
            }

            for (int friend_id : nodes[current.second].neighbors[layer]) {
                // O(1) Check: Does this friend have our current epoch number?
                if (visited_array[friend_id] != current_visit_mark) {
                    
                    // Mark it as visited!
                    visited_array[friend_id] = current_visit_mark;
                    
                    float friend_dist = euclidean_distance(query, nodes[friend_id].data);

                    if (top_results.size() < ef || friend_dist < top_results.top().first) {
                        candidates.push({friend_dist, friend_id});
                        top_results.push({friend_dist, friend_id});
                        
                        if (top_results.size() > ef) {
                            top_results.pop();
                        }
                    }
                }
            }
        }

        int best_id = -1;
        float best_dist = 999999.0f;
        while (!top_results.empty()) {
            if (top_results.top().first < best_dist) {
                best_dist = top_results.top().first;
                best_id = top_results.top().second;
            }
            top_results.pop();
        }
        
        return best_id;
    }
    // int HNSWIndex::search_layer(int entry_node, const std::vector<float>& query, int layer) {
    //     int ef = 50; // Beam width
        
    //     std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<>> candidates;
    //     std::priority_queue<std::pair<float, int>> top_results;
        
    //     // SYSTEMS OPTIMIZATION: Use a flat vector instead of an unordered_set. 
    //     // This avoids thousands of slow heap allocations.
    //     std::vector<bool> visited(nodes.size(), false);

    //     float initial_dist = euclidean_distance(query, nodes[entry_node].data);
    //     candidates.push({initial_dist, entry_node});
    //     top_results.push({initial_dist, entry_node});
    //     visited[entry_node] = true;

    //     while (!candidates.empty()) {
    //         auto current = candidates.top();
    //         candidates.pop();

    //         if (current.first > top_results.top().first) {
    //             break; 
    //         }

    //         for (int friend_id : nodes[current.second].neighbors[layer]) {
    //             // Blazing fast O(1) direct array lookup
    //             if (!visited[friend_id]) {
    //                 visited[friend_id] = true;
    //                 float friend_dist = euclidean_distance(query, nodes[friend_id].data);

    //                 if (top_results.size() < ef || friend_dist < top_results.top().first) {
    //                     candidates.push({friend_dist, friend_id});
    //                     top_results.push({friend_dist, friend_id});
                        
    //                     if (top_results.size() > ef) {
    //                         top_results.pop();
    //                     }
    //                 }
    //             }
    //         }
    //     }

    //     int best_id = -1;
    //     float best_dist = 999999.0f;
    //     while (!top_results.empty()) {
    //         if (top_results.top().first < best_dist) {
    //             best_dist = top_results.top().first;
    //             best_id = top_results.top().second;
    //         }
    //         top_results.pop();
    //     }
        
    //     return best_id;
    // }

    // The upgraded Beam Search algorithm (ef = Expansion Factor)
    // int HNSWIndex::search_layer(int entry_node, const std::vector<float>& query, int layer) {
    //     int ef = 50; // The "Beam Width". Higher = slower but MUCH more accurate.
        
    //     // Min-heap to find the closest candidates to explore next
    //     std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<>> candidates;
    //     // Max-heap to keep track of the absolute best 'ef' results we've found so far
    //     std::priority_queue<std::pair<float, int>> top_results;
    //     std::unordered_set<int> visited;

    //     float initial_dist = euclidean_distance(query, nodes[entry_node].data);
    //     candidates.push({initial_dist, entry_node});
    //     top_results.push({initial_dist, entry_node});
    //     visited.insert(entry_node);

    //     while (!candidates.empty()) {
    //         auto current = candidates.top();
    //         candidates.pop();

    //         // If our closest unexplored node is further than our worst top result, we can safely stop exploring
    //         if (current.first > top_results.top().first) {
    //             break; 
    //         }

    //         for (int friend_id : nodes[current.second].neighbors[layer]) {
    //             // Only evaluate friends we haven't seen yet
    //             if (visited.find(friend_id) == visited.end()) {
    //                 visited.insert(friend_id);
    //                 float friend_dist = euclidean_distance(query, nodes[friend_id].data);

    //                 // If our basket isn't full, or this friend is better than our worst result
    //                 if (top_results.size() < ef || friend_dist < top_results.top().first) {
    //                     candidates.push({friend_dist, friend_id});
    //                     top_results.push({friend_dist, friend_id});
                        
    //                     // Keep basket size strictly at 'ef'
    //                     if (top_results.size() > ef) {
    //                         top_results.pop();
    //                     }
    //                 }
    //             }
    //         }
    //     }

    //     // We want to return the absolute best node in our basket
    //     int best_id = -1;
    //     float best_dist = 999999.0f;
    //     while (!top_results.empty()) {
    //         if (top_results.top().first < best_dist) {
    //             best_dist = top_results.top().first;
    //             best_id = top_results.top().second;
    //         }
    //         top_results.pop();
    //     }
        
    //     return best_id;
    // }
    // // 2. Single Layer Hop: Same as Milestone 2, but restricted to a specific layer
    // int HNSWIndex::search_layer(int entry_node, const std::vector<float>& query, int layer) {
    //     int current_node_id = entry_node;
    //     float current_min_dist = euclidean_distance(query, nodes[current_node_id].data);
    //     bool changed = true;

    //     while (changed) {
    //         changed = false;
    //         for (int friend_id : nodes[current_node_id].neighbors[layer]) {
    //             float friend_dist = euclidean_distance(query, nodes[friend_id].data);
    //             if (friend_dist < current_min_dist) {
    //                 current_min_dist = friend_dist;
    //                 current_node_id = friend_id;
    //                 changed = true;
    //             }
    //         }
    //     }
    //     return current_node_id;
    // }

    // 3. Multi-Layer Search (The Google Highway)
    int HNSWIndex::search(const std::vector<float>& query) {
        if (!has_entry_point) return -1;
        
        int current_node = entry_point_id;
        
        // Drop down from the absolute top layer down to layer 1
        for (int layer = max_current_level; layer >= 1; --layer) {
            current_node = search_layer(current_node, query, layer);
        }
        
        // Final fine-grained search on layer 0 to find the absolute closest
        return search_layer(current_node, query, 0);
    }

    // 4. Multi-Layer Insertion
    void HNSWIndex::add_vector(int id, const std::vector<float>& data) {
        if (id >= nodes.size()) {
            nodes.resize(id + 1);
            visited_array.resize(id + 1, 0);
        }
        int level = generate_random_level();
        
        // Initialize node with enough empty arrays to support its assigned level
        nodes[id] = Node{id, data, std::vector<std::vector<int>>(level + 1)};

        if (!has_entry_point) {
            entry_point_id = id;
            max_current_level = level;
            has_entry_point = true;
            return;
        }

        int current_node = entry_point_id;

        // Phase 1: Navigate top layers to find a good starting point for insertion
        for (int curr_layer = max_current_level; curr_layer > level; --curr_layer) {
            current_node = search_layer(current_node, data, curr_layer);
        }

        // Phase 2: Insert and make friends at all assigned layers (from 'level' down to 0)
        int max_connections = 32; 
        for (int curr_layer = std::min(level, max_current_level); curr_layer >= 0; --curr_layer) {
            current_node = search_layer(current_node, data, curr_layer);
            
            // Link mutually
            nodes[id].neighbors[curr_layer].push_back(current_node);
            nodes[current_node].neighbors[curr_layer].push_back(id);
            
            for (int friend_id : nodes[current_node].neighbors[curr_layer]) {
                if (nodes[id].neighbors[curr_layer].size() >= max_connections) break;
                if (friend_id != id && friend_id != current_node) {
                    nodes[id].neighbors[curr_layer].push_back(friend_id);
                    nodes[friend_id].neighbors[curr_layer].push_back(id);
                }
            }
        }

        // Phase 3: If this node got a new highest level, it becomes the new global entry point
        if (level > max_current_level) {
            max_current_level = level;
            entry_point_id = id;
        }
    }
}

// #include "hnsw_index.hpp"
// #include "math_utils.hpp"
// #include <cmath>
// #include <algorithm>

// namespace VectorEngine {

//     HNSWIndex::HNSWIndex() : m_L(0.5), distribution(0.0, 1.0) {
//         has_entry_point = false;
//         entry_point_id = -1;
//         max_current_level = 0;
//     }

//     // 1. The Math: Assigns a random layer using Geometric Distribution
//     int HNSWIndex::generate_random_level() {
//         double r = distribution(generator);
//         if (r == 0.0) r = 0.00001; // Prevent log(0) error
//         return static_cast<int>(-std::log(r) * m_L);
//     }

//     // 2. Single Layer Hop: Same as Milestone 2, but restricted to a specific layer
//     int HNSWIndex::search_layer(int entry_node, const std::vector<float>& query, int layer) {
//         int current_node_id = entry_node;
//         float current_min_dist = euclidean_distance(query, nodes[current_node_id].data);
//         bool changed = true;

//         while (changed) {
//             changed = false;
//             for (int friend_id : nodes[current_node_id].neighbors[layer]) {
//                 float friend_dist = euclidean_distance(query, nodes[friend_id].data);
//                 if (friend_dist < current_min_dist) {
//                     current_min_dist = friend_dist;
//                     current_node_id = friend_id;
//                     changed = true;
//                 }
//             }
//         }
//         return current_node_id;
//     }

//     // 3. Multi-Layer Search (The Google Highway)
//     int HNSWIndex::search(const std::vector<float>& query) {
//         if (!has_entry_point) return -1;
        
//         int current_node = entry_point_id;
        
//         // Drop down from the absolute top layer down to layer 1
//         for (int layer = max_current_level; layer >= 1; --layer) {
//             current_node = search_layer(current_node, query, layer);
//         }
        
//         // Final fine-grained search on layer 0 to find the absolute closest
//         return search_layer(current_node, query, 0);
//     }

//     // 4. Multi-Layer Insertion
//     void HNSWIndex::add_vector(int id, const std::vector<float>& data) {
//         int level = generate_random_level();
        
//         // Initialize node with enough empty arrays to support its assigned level
//         nodes[id] = Node{id, data, std::vector<std::vector<int>>(level + 1)};

//         if (!has_entry_point) {
//             entry_point_id = id;
//             max_current_level = level;
//             has_entry_point = true;
//             return;
//         }

//         int current_node = entry_point_id;

//         // Phase 1: Navigate top layers to find a good starting point for insertion
//         for (int curr_layer = max_current_level; curr_layer > level; --curr_layer) {
//             current_node = search_layer(current_node, data, curr_layer);
//         }

//         // Phase 2: Insert and make friends at all assigned layers (from 'level' down to 0)
//         int max_connections = 3; 
//         for (int curr_layer = std::min(level, max_current_level); curr_layer >= 0; --curr_layer) {
//             current_node = search_layer(current_node, data, curr_layer);
            
//             // Link mutually
//             nodes[id].neighbors[curr_layer].push_back(current_node);
//             nodes[current_node].neighbors[curr_layer].push_back(id);
            
//             for (int friend_id : nodes[current_node].neighbors[curr_layer]) {
//                 if (nodes[id].neighbors[curr_layer].size() >= max_connections) break;
//                 if (friend_id != id && friend_id != current_node) {
//                     nodes[id].neighbors[curr_layer].push_back(friend_id);
//                     nodes[friend_id].neighbors[curr_layer].push_back(id);
//                 }
//             }
//         }

//         // Phase 3: If this node got a new highest level, it becomes the new global entry point
//         if (level > max_current_level) {
//             max_current_level = level;
//             entry_point_id = id;
//         }
//     }
// }



// // src/hnsw_index.cpp
// #include "hnsw_index.hpp"
// #include "math_utils.hpp"
// #include <iostream>

// namespace VectorEngine {

//     HNSWIndex::HNSWIndex() {
//         has_entry_point = false;
//         entry_point_id = -1;
//     }

//     // Update this specific function in src/hnsw_index.cpp
//     void HNSWIndex::add_vector(int id, const std::vector<float>& data) {
//         // 1. Create the node with an empty neighbor list
//         nodes[id] = Node{id, data, {}};
        
//         // 2. If the graph is empty, this is our starting point. We are done.
//         if (!has_entry_point) {
//             entry_point_id = id;
//             has_entry_point = true;
//             return;
//         }

//         // 3. Find the closest existing node in the graph using our fast search
//         int closest_id = greedy_search(data);

//         // 4. Create a bi-directional connection to the closest node
//         nodes[id].neighbors.push_back(closest_id);
//         nodes[closest_id].neighbors.push_back(id);

//         // 5. Build the "Small World" by connecting to the closest node's friends
//         // We cap connections at M=3 to prevent the graph from becoming too dense
//         int max_connections = 3; 
//         for (int friend_id : nodes[closest_id].neighbors) {
//             if (nodes[id].neighbors.size() >= max_connections) break;
            
//             // Ensure we don't connect to ourselves, and connect bidirectionally
//             if (friend_id != id && friend_id != closest_id) {
//                 nodes[id].neighbors.push_back(friend_id);
//                 nodes[friend_id].neighbors.push_back(id);
//             }
//         }
//     }

//     // THIS IS THE MAGIC. Instead of looping through all nodes, we "hop" through the graph.
//     int HNSWIndex::greedy_search(const std::vector<float>& query) {
//         if (!has_entry_point) return -1;

//         int current_node_id = entry_point_id;
//         float current_min_dist = euclidean_distance(query, nodes[current_node_id].data);

//         bool changed = true;

//         // Keep hopping until we can't find a closer friend
//         while (changed) {
//             changed = false;

//             // Check all friends (neighbors) of our current node
//             for (int friend_id : nodes[current_node_id].neighbors) {
//                 float friend_dist = euclidean_distance(query, nodes[friend_id].data);
                
//                 // If the friend is closer to the query, jump to the friend!
//                 if (friend_dist < current_min_dist) {
//                     current_min_dist = friend_dist;
//                     current_node_id = friend_id;
//                     changed = true; // We moved, so we must keep searching
//                 }
//             }
//         }
        
//         // When the loop finishes, we are at a local minimum (the closest node)
//         return current_node_id;
//     }
// }