#include "correlation.hpp"

/*
 This measures the correlation between successive intervals.
 Low serial correlation suggests randomness, while high correlation suggests patterned behavior.
 Humans will always high a medium serial correlation
 */
double calculateSerialCorrelation(const std::vector<double>& data) {
    // Calculate the serial correlation between successive intervals
    double sumCrossProduct = 0.0;
    double sumSquaredPrev = 0.0;
    double sumSquaredCurr = 0.0;

    for (size_t i = 1; i < data.size(); ++i) {
        sumCrossProduct += (data[i] - data[i - 1]) * (data[i] - data[i - 1]);
        sumSquaredPrev += (data[i - 1] * data[i - 1]);
        sumSquaredCurr += (data[i] * data[i]);
    }

    double numerator = sumCrossProduct;
    double denominator = std::sqrt(sumSquaredPrev * sumSquaredCurr);
    return numerator / denominator;
}