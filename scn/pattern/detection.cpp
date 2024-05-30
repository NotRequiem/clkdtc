#include "detection.hpp"

static bool areSimilar(const IntervalStats& stats1, const IntervalStats& stats2) {
    return (std::abs(stats1.meanInterval - stats2.meanInterval) < threshold &&
        std::abs(stats1.stdDev - stats2.stdDev) < threshold &&
        std::abs(stats1.variance - stats2.variance) < threshold &&
        std::abs(stats1.skewness - stats2.skewness) < threshold &&
        std::abs(stats1.kurtosis - stats2.kurtosis) < threshold &&
        std::abs(stats1.avgClicksPerSec - stats2.avgClicksPerSec) < threshold &&
        std::abs(stats1.serialCorrelation - stats2.serialCorrelation) < threshold &&
        std::abs(stats1.entropy - stats2.entropy) < threshold);
}

static void printStats(const IntervalStats& stats) {
    std::cout << "Mean Interval: " << stats.meanInterval << "\n";
    std::cout << "Standard Deviation: " << stats.stdDev << "\n";
    std::cout << "Variance: " << stats.variance << "\n";
    std::cout << "Skewness: " << stats.skewness << "\n";
    std::cout << "Kurtosis: " << stats.kurtosis << "\n";
    std::cout << "Average Clicks per Second: " << stats.avgClicksPerSec << "\n";
    std::cout << "Serial Correlation: " << stats.serialCorrelation << "\n";
    std::cout << "Entropy: " << stats.entropy << "\n";
}

void detect_patterns() {
    std::vector<double> intervals;
    for (size_t i = 1; i < clickTimestamps.size(); ++i) {
        intervals.push_back(clickTimestamps[i] - clickTimestamps[i - 1]);
    }

    for (size_t i = monitorCount; i < intervals.size(); ++i) {
        std::vector<double> currentIntervals(intervals.begin(), intervals.begin() + i);
        double meanInterval = calculateMean(currentIntervals);
        double stdDev = calculateStandardDeviation(currentIntervals, meanInterval);
        double variance = calculateVariance(currentIntervals, meanInterval);
        double skewness = calculateSkewness(currentIntervals, meanInterval, stdDev);
        double kurtosis = calculateKurtosis(currentIntervals, meanInterval, stdDev);
        double avgClicksPerSec = 1.0 / meanInterval;
        double serialCorrelation = calculateSerialCorrelation(currentIntervals);
        double entropy = calculateEntropy(currentIntervals);

        IntervalStats currentStats = { meanInterval, stdDev, variance, skewness, kurtosis, avgClicksPerSec, serialCorrelation, entropy };
        allStats.push_back(currentStats);

        for (size_t j = 0; j < allStats.size() - 1; ++j) {
            IntervalStats& prevStats = allStats[j];
            if (areSimilar(currentStats, prevStats)) {
                std::cout << "Possible autoclicker pattern detected between clicks " << j + monitorCount << " and " << i + 1 << "!\n";
                std::cout << "Stats for click " << j + monitorCount << ":\n";
                printStats(prevStats);
                std::cout << "---------------------\n";
                std::cout << "Stats for click " << i + 1 << ":\n";
                printStats(currentStats);
                std::cout << "---------------------\n";

                size_t patternInterval = (i + 1) - (j + monitorCount);
                int matchCount = 1;
                size_t nextCheck = i + 1 + patternInterval;

                while (matchCount < repeatCount && nextCheck < allStats.size()) {
                    IntervalStats& nextStats = allStats[nextCheck - monitorCount];
                    if (areSimilar(currentStats, nextStats)) {
                        matchCount++;
                    }
                    else {
                        matchCount = 1;
                        break;
                    }
                    nextCheck += patternInterval;
                }

                if (matchCount >= repeatCount) {
                    std::cout << "Autoclicker detected!\n";
                    return;
                }
            }
        }
    }
}