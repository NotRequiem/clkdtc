#include "frequency.hpp"

// Function to calculate frequency distribution
std::map<double, int> calculateFrequencyDistribution(const std::vector<double>& data) {
    std::map<double, int> frequency;
    for (const auto& value : data) {
        ++frequency[value];
    }
    return frequency;
}
