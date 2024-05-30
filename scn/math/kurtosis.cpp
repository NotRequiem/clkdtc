#include "kurtosis.hpp"

/*
Measures the "tailedness" of the distribution.
High kurtosis indicates more outliers (heavy tails), while low kurtosis indicates fewer outliers (light tails)
This is also the reason why there's no need to directly calculate the outliers
*/
double calculateKurtosis(const std::vector<double>& data, double mean, double stdDev) {
    // Kurtosis measures the "tailedness" of the data distribution

    double m4 = 0.0;
    for (double value : data) {
        double diff = value - mean;
        m4 += std::pow(diff / stdDev, 4);
    }
    double n = static_cast<double>(data.size());
    double numerator = n * (n + 1) * m4 - 3 * (n - 1) * (n - 1);
    return numerator / ((n - 1) * (n - 2) * (n - 3));
}