#include "entropy.hpp"

// Shannon formula btw
// Entropy measures the randomness or unpredictability of a system
// I calculate entropy based on the distribution of click intervals. High entropy suggests more randomness
double calculateEntropy(const std::vector<double>& data) {
    // Count the frequency of each unique click interval
    std::unordered_map<double, int> frequencyMap;
    for (double interval : data) {
        frequencyMap[interval]++;
    }

    // Calculate the probability of each interval
    std::vector<double> probabilities;
    for (const auto& pair : frequencyMap) {
        double probability = static_cast<double>(pair.second) / data.size();
        probabilities.push_back(probability);
    }

    // Calculate entropy
    double entropy = 0.0;
    for (double probability : probabilities) {
        entropy -= probability * std::log2(probability);
    }

    return entropy;
}