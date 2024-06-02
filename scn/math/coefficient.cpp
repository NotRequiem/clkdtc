#include "coefficient.hpp"

// Function to calculate Gini coefficient
double calculateCoefficient(const std::vector<double>& data) {
    std::vector<double> sorted_data = data;
    std::sort(sorted_data.begin(), sorted_data.end());
    size_t size = sorted_data.size();

    double cumulative_sum = 0.0;
    double sum = 0.0;
    for (size_t i = 0; i < size; ++i) {
        cumulative_sum += sorted_data[i];
        sum += (i + 1) * sorted_data[i];
    }

    return (2.0 * sum) / (size * cumulative_sum) - (size + 1.0) / size;
}