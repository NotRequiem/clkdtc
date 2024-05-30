#include "calculations.hpp"

void start_calculations() {
    std::vector<double> intervals;
    while (true) {
        std::unique_lock<std::mutex> lock(dataMutex);
        cv.wait(lock, [] { return clickTimestamps.size() % 50 == 0 || clickCount >= monitorClicks; });

        intervals.clear();
        for (size_t i = 1; i < clickTimestamps.size(); ++i) {
            intervals.push_back(clickTimestamps[i] - clickTimestamps[i - 1]);
        }

        double meanInterval = calculateMean(intervals);
        double stdDev = calculateStandardDeviation(intervals, meanInterval);
        double variance = calculateVariance(intervals, meanInterval);
        double skewness = calculateSkewness(intervals, meanInterval, stdDev);
        double kurtosis = calculateKurtosis(intervals, meanInterval, stdDev);
        double avgClicksPerSec = 1.0 / meanInterval;
        double serialCorrelation = calculateSerialCorrelation(intervals);
        double entropy = calculateEntropy(intervals);

#ifdef _DEBUG
        std::vector<std::vector<int>> crp = calculateCRP(intervals, stdDev);

        // Output the CRP matrix
        std::cout << "Cross Recurrence Plot (CRP):\n";
        for (const auto& row : crp) {
            for (int val : row) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
#endif

        // The maximum number of decimal places you can print with precise pointer precision (assuming the default precision of double type in C++) is typically around 15 to 17 decimal places.

        std::cout << "Average statistics of the whole click data:" << std::endl;
        std::cout << std::setprecision(15);

        std::cout << "Mean Interval: " << meanInterval << " seconds\n";
        std::cout << "Standard Deviation: " << stdDev << "\n";
        std::cout << "Variance: " << variance << "\n";
        std::cout << "Skewness: " << skewness << "\n";
        std::cout << "Kurtosis: " << kurtosis << "\n";
        std::cout << "Average Clicks per Second: " << avgClicksPerSec << "\n";
        std::cout << "Serial Correlation: " << serialCorrelation << "\n";
        std::cout << "Entropy: " << entropy << "\n";

        if (clickCount >= monitorClicks) break;
    }
}