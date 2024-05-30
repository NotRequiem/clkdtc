#include "stdDev.hpp"

double calculateStandardDeviation(const std::vector<double>& data, double mean) {
    /*
    The square root of variance, giving a measure of dispersion in the same units as the original data.
    It is useful for identifying how spread out the intervals are.
    This check is not reliable on it's own, it should be combined with another statistical value (example: kurtosis)

    In an example case of having very low standard desviations, with low outliers and therefore negatively skewned data at the same time, this check is reliable after a long time.
    This essentially would check for standard desviation (consistent clicking) while also having low outliers (high clicks per second) for a long period of time
    Therefore, this would flag players clicking too consistently while clicking fastly for a long period of time or click amounts
    */
    double sum = 0.0;
    for (double value : data) {
        sum += (value - mean) * (value - mean);
    }
    return std::sqrt(sum / (data.size() - 1));
}