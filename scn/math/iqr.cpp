#include "iqr.hpp"

// Function to calculate Interquartile Range (IQR)
double calculateIQR(const std::vector<double>& data) {
    std::vector<double> sorted_data = data;
    std::sort(sorted_data.begin(), sorted_data.end());
    size_t size = sorted_data.size();

    double q1, q3;

    // Calculate Q1 (25th percentile)
    if ((size % 4) == 0) {
        q1 = (sorted_data[size / 4 - 1] + sorted_data[size / 4]) / 2.0;
    }
    else {
        q1 = sorted_data[size / 4];
    }

    // Calculate Q3 (75th percentile)
    if ((3 * size % 4) == 0) {
        q3 = (sorted_data[3 * size / 4 - 1] + sorted_data[3 * size / 4]) / 2.0;
    }
    else {
        q3 = sorted_data[3 * size / 4];
    }

    return q3 - q1;
}
