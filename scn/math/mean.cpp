#include "mean.hpp"

// This helps establish a baseline for normal clicking behavior.
double calculateMean(const std::vector<double>& data) {
    // Mean is the average value of the dataset
    return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}