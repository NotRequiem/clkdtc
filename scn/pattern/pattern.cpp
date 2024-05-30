#include "pattern.hpp"

std::vector<std::vector<IntervalStats>> allStats;
std::vector<std::vector<IntervalStats>> predictionVector;
size_t min = 50;
size_t clickCount = 0;
size_t prediction = 0;
int similarityCount = 0;

void detect_patterns() {
    while (true) {
        if (clickTimestamps.size() < min) {
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Sleep for 1 second if not enough clicks
            continue;
        }

        // Generate stats for current click
        std::vector<IntervalStats> currentStatsVec;
        currentStatsVec.reserve(clickCount);
        for (size_t j = 1; j <= clickCount; ++j) {
            std::vector<double> currentIntervals;
            currentIntervals.reserve(clickTimestamps.size() - 1);
            for (size_t k = 1; k <= j; ++k) {
                currentIntervals.push_back(clickTimestamps[clickCount] - clickTimestamps[clickCount - k]);
            }

            double meanInterval = calculateMean(currentIntervals);
            double stdDev = calculateStandardDeviation(currentIntervals, meanInterval);
            double variance = calculateVariance(currentIntervals, meanInterval);
            double skewness = calculateSkewness(currentIntervals, meanInterval, stdDev);
            double kurtosis = calculateKurtosis(currentIntervals, meanInterval, stdDev);
            double avgClicksPerSec = 1.0 / meanInterval;
            double serialCorrelation = calculateSerialCorrelation(currentIntervals);
            double entropy = calculateEntropy(currentIntervals);

            IntervalStats currentStats = { static_cast<size_t>(clickCount), meanInterval, stdDev, variance, skewness, kurtosis, avgClicksPerSec, serialCorrelation, entropy };
            currentStatsVec.push_back(currentStats);
        }

        allStats.push_back(currentStatsVec);

        // Compare with previous stats
        for (size_t i = 0; i < allStats.size() - 1; ++i) {
            for (size_t j = 0; j < allStats[i].size(); ++j) {
                for (size_t k = 0; k < allStats.back().size(); ++k) {
                    if (areSimilar(allStats[i][j], allStats.back()[k])) {
                        std::cout << "Similarity detected." << std::endl;
                        std::cout << "Stats from click " << i << " (Click Count: " << allStats[i][j].clickCount << "):" << std::endl;
                        printStats(allStats[i][j]);
                        std::cout << "-----------------------------" << std::endl;
                        std::cout << "Stats from click " << clickCount << ":" << std::endl;
                        printStats(allStats.back()[k]);
                        std::cout << "-----------------------------" << std::endl;

                        size_t difference = clickCount - allStats[i][j].clickCount;
                        prediction = clickCount + difference;

                        predictionVector.push_back(allStats.back());

                        allStats.clear();
                        clickCount = 0;

                        // Continue checking within a range of 5 clicks around the prediction
                        for (int idx = 0; idx < 10; ++idx) {
                            size_t newClickCount = clickCount + i;
                            size_t newPrediction = prediction + i;

                            if (newPrediction < allStats.size()) {
                                for (size_t m = newPrediction - 5; m <= newPrediction + 5; ++m) {
                                    if (m >= 0 && m < allStats.size()) {
                                        for (size_t n = 0; n < allStats[m].size(); ++n) {
                                            if (areSimilar(predictionVector.back()[k], allStats[m][n])) {
                                                ++similarityCount;
                                                break; // Exit the loop once similarity is found
                                            }
                                        }
                                    }
                                    if (similarityCount >= 10) {
                                        // If similarity is found 10 times, there's a blatant pattern and therefore it's an autoclicker
                                        std::cout << "Autoclicker detected!" << std::endl;
                                        return;
                                    }
                                }
                            }
                        }

                        return;
                    }
                }
            }
        }

        ++clickCount;
    }
}