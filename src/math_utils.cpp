#include "math_utils.hpp"
#include <cmath>
#include <stdexcept>

namespace VectorEngine {
    float euclidean_distance(const std::vector<float>& a, const std::vector<float>& b) {
        // Safety check: Vectors must exist in the same dimensional space
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must have the same dimensionality.");
        }
        
        float sum = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            float diff = a[i] - b[i];
            sum += diff * diff;
        }
        
        return std::sqrt(sum);
    }
}