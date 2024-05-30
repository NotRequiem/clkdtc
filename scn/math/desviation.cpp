#include "desviation.hpp"

void detectSpikesAndOutliers(const std::vector<double>& data, double mean, double stdDev) {
    std::cout << "Outliers (more than 4 STD from mean):\n";
    // a very low number of STD from the mean to compare would false flag human clicking, it should be as high as possible
    for (double value : data) {
        // Outliers are data points that are more than 2 standard deviations away from the mean
        // Normally, you wont have more than one outlier with more than 2 standard desviation
        if (std::abs(value - mean) > 4 * stdDev) {
            std::cout << value << "\n";
        }
    }

    std::cout << "Spikes (adjacent differences more than 4 STD):\n";
    for (size_t i = 1; i < data.size(); ++i) {
        // Spikes are large changes between consecutive data points, you can have multiple of them
        if (std::abs(data[i] - data[i - 1]) > 4 * stdDev) {
            std::cout << data[i - 1] << " -> " << data[i] << "\n";
        }
    }
}