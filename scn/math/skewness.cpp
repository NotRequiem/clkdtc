#include "skewness.hpp"

/*
Measures the asymmetry of the distribution.
Positive skewness indicates that intervals are more spread out on the right (longer intervals)
while negative skewness indicates they are more spread out on the left (shorter intervals)
*/
double calculateSkewness(const std::vector<double>& data, double mean, double stdDev) {
    double m3 = 0.0;
    for (double value : data) {
        double diff = value - mean;
        m3 += std::pow(diff / stdDev, 3);
    }

    // original formula should be: return (data.size() / ((data.size() - 1) * (data.size() - 2))) * m3;
    // however, I cast the return statement to prevent the integer division result from being truncated before casting to a floating point

    return (data.size() / ((data.size() - static_cast<double>(1)) * (data.size() - 2))) * m3;
}