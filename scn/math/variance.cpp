#include "variance.hpp"

/*
For the analysis of click intervals, we should use sample variance.
This is because we are dealing with a sample of all possible click intervals (not the entire population).
Sample variance divides by (n - 1) to account for the degrees of freedom and provides an unbiased estimate of the population variance.
*/
double calculateVariance(const std::vector<double>& data, double mean) {
    /*
      A measure of how much the click intervals vary from the mean.
      Higher variance indicates more variability, while lower variance suggests consistent intervals.
      In the case of autoclickers, we're interested on very high variances only
      due to superhumans like Wizzard clicking with 0 standard deviation
    */
    double sum = 0.0;
    for (double value : data) {
        sum += (value - mean) * (value - mean);
    }
    return sum / (data.size() - 1);
}