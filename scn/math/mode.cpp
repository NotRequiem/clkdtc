#include "mode.hpp"

// Function to calculate mode
double calculateMode(const std::vector<double>& data) {
    std::map<double, int> frequency;
    for (const auto& value : data) {
        ++frequency[value];
    }

    int max_count = 0;
    double mode = data[0];
    for (const auto& pair : frequency) {
        if (pair.second > max_count) {
            max_count = pair.second;
            mode = pair.first;
        }
    }

    return mode;
}