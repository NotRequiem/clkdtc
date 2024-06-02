#include "median.hpp"

// Function to calculate median
double calculateMedian(const std::vector<double>& data) {
    std::vector<double> sorted_data = data;
    std::sort(sorted_data.begin(), sorted_data.end());
    size_t size = sorted_data.size();

    if (size % 2 == 0) {
        return (sorted_data[size / 2 - 1] + sorted_data[size / 2]) / 2;
    }
    else {
        return sorted_data[size / 2];
    }
}