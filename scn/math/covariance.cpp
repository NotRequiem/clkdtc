#include "covariance.hpp"

// Measures how two variables change together.
double calculateCovariance(const std::vector<double>& dataX, const std::vector<double>& dataY, double meanX, double meanY) {
    double covariance = 0.0;
    // Here, it’s used to measure the relationship between click intervals and other statistics

    for (size_t i = 0; i < dataX.size(); ++i) {
        covariance += (dataX[i] - meanX) * (dataY[i] - meanY);
    }
    return covariance / dataX.size();
}
