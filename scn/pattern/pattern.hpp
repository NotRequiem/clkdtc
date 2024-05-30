#pragma once

#include "..\math\math.h"
#include "..\include.h"

struct IntervalStats {
    size_t clickCount;
    double meanInterval;
    double stdDev;
    double variance;
    double skewness;
    double kurtosis;
    double avgClicksPerSec;
    double serialCorrelation;
    double entropy;
};

bool areSimilar(const IntervalStats& stats1, const IntervalStats& stats2) {
    double threshold = 2;
    return (std::abs(stats1.meanInterval - stats2.meanInterval) < threshold &&
        std::abs(stats1.stdDev - stats2.stdDev) < threshold &&
        std::abs(stats1.variance - stats2.variance) < threshold &&
        std::abs(stats1.skewness - stats2.skewness) < threshold &&
        std::abs(stats1.kurtosis - stats2.kurtosis) < threshold &&
        std::abs(stats1.avgClicksPerSec - stats2.avgClicksPerSec) < threshold &&
        std::abs(stats1.serialCorrelation - stats2.serialCorrelation) < threshold &&
        std::abs(stats1.entropy - stats2.entropy) < threshold);
}

void printStats(const IntervalStats& stats) {
    std::cout << std::setprecision(15);
    std::cout << "Mean Interval: " << stats.meanInterval << "\n";
    std::cout << "Standard Deviation: " << stats.stdDev << "\n";
    std::cout << "Variance: " << stats.variance << "\n";
    std::cout << "Skewness: " << stats.skewness << "\n";
    std::cout << "Kurtosis: " << stats.kurtosis << "\n";
    std::cout << "Average Clicks per Second: " << stats.avgClicksPerSec << "\n";
    std::cout << "Serial Correlation: " << stats.serialCorrelation << "\n";
    std::cout << "Entropy: " << stats.entropy << "\n";
}

void detect_patterns();
