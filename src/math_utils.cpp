#include "math_utils.hpp"
#include <cmath>
#include <stdexcept>
#include <immintrin.h> // Intel AVX2 Hardware Intrinsics

namespace VectorEngine {
    float euclidean_distance(const std::vector<float>& a, const std::vector<float>& b) {
        size_t size = a.size();
        size_t i = 0;
        
        // Create a 256-bit hardware register filled with zeros to hold our running sum
        __m256 sum_vec = _mm256_setzero_ps();
        
        // Process 8 dimensions at the exact same time
        for (; i + 7 < size; i += 8) {
            // Load 8 floats from vector A and 8 floats from vector B directly into the CPU registers
            __m256 va = _mm256_loadu_ps(&a[i]);
            __m256 vb = _mm256_loadu_ps(&b[i]);
            
            // Subtract all 8 floats simultaneously
            __m256 diff = _mm256_sub_ps(va, vb);
            
            // Square them (diff * diff) simultaneously
            __m256 sq_diff = _mm256_mul_ps(diff, diff);
            
            // Add the 8 squared differences to our running sum simultaneously
            sum_vec = _mm256_add_ps(sum_vec, sq_diff);
        }
        
        // Extract the 8 floats from the hardware register back into normal RAM
        float buffer[8];
        _mm256_storeu_ps(buffer, sum_vec);
        
        // Sum up the final 8 numbers
        float final_sum = buffer[0] + buffer[1] + buffer[2] + buffer[3] + 
                          buffer[4] + buffer[5] + buffer[6] + buffer[7];
        
        // Clean up any remaining dimensions (if dimension size isn't a perfect multiple of 8)
        for (; i < size; ++i) {
            float diff = a[i] - b[i];
            final_sum += diff * diff;
        }
        
        return std::sqrt(final_sum);
    }
}

// #include "math_utils.hpp"
// #include <cmath>
// #include <stdexcept>

// namespace VectorEngine {
//     float euclidean_distance(const std::vector<float>& a, const std::vector<float>& b) {
//         // Safety check: Vectors must exist in the same dimensional space
//         if (a.size() != b.size()) {
//             throw std::invalid_argument("Vectors must have the same dimensionality.");
//         }
        
//         float sum = 0.0f;
//         for (size_t i = 0; i < a.size(); ++i) {
//             float diff = a[i] - b[i];
//             sum += diff * diff;
//         }
        
//         return std::sqrt(sum);
//     }
// }