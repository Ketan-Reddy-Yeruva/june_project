#pragma once
#include <vector>

namespace VectorEngine {
    // Calculates Euclidean Distance (L2 norm) between two high-dimensional vectors.
    // Passed by const reference to prevent expensive memory copying.
    float euclidean_distance(const std::vector<float>& a, const std::vector<float>& b);
}