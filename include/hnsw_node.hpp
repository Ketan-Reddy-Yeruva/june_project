// include/hnsw_node.hpp
#pragma once
#include <vector>

namespace VectorEngine {
    struct Node {
        int id;
        std::vector<float> data;
        
        // A list of IDs representing this node's connected neighbors (edges)
        // std::vector<int> neighbors; 
        // neighbors[level] gives a list of friend IDs at that specific layer
        std::vector<std::vector<int>> neighbors;
    };
}