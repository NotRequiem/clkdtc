#include "pattern.hpp"

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

// Checks if all the statisticals of the members of a statistical vector are very similar to all the corresponding members of another statistical vector
static bool areSimilar(const IntervalStats& stats1, const IntervalStats& stats2) {
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

std::vector<std::vector<IntervalStats>> allStats;
std::vector<std::vector<IntervalStats>> predictionVector;
std::vector<size_t> predictionValues;
size_t min = 50;
size_t globalClickCount = 0;
size_t difference = 0;
size_t prediction = 0;
int similarityCount = 0;
bool found_pattern = false;

void detect_patterns() {
    // This function will be called after the hook.cpp code have already pushed 50 clickTimeStamps
    while (true) {
        // Generate a vector of statistics with the total average statistical metrics until the current click
        std::vector<IntervalStats> currentStatsVec;
        currentStatsVec.reserve(globalClickCount);
        for (size_t j = 1; j <= globalClickCount; ++j) {
            std::vector<double> currentIntervals;
            currentIntervals.reserve(clickTimestamps.size() - 1);
            // Calculate time intervals between clicks with the time intervals retrieved by our hook, this is necessary to calculate the statistics
            for (size_t k = 1; k <= j; ++k) {
                currentIntervals.push_back(clickTimestamps[globalClickCount] - clickTimestamps[globalClickCount - k]);
            }

            // We calculate various statistical metrics for the current click
            double meanInterval = calculateMean(currentIntervals);
            double stdDev = calculateStandardDeviation(currentIntervals, meanInterval);
            double variance = calculateVariance(currentIntervals, meanInterval);
            double skewness = calculateSkewness(currentIntervals, meanInterval, stdDev);
            double kurtosis = calculateKurtosis(currentIntervals, meanInterval, stdDev);
            double avgClicksPerSec = 1.0 / meanInterval;
            double serialCorrelation = calculateSerialCorrelation(currentIntervals);
            double entropy = calculateEntropy(currentIntervals);

            IntervalStats currentStats = { static_cast<size_t>(globalClickCount), meanInterval, stdDev, variance, skewness, kurtosis, avgClicksPerSec, serialCorrelation, entropy };
            currentStatsVec.push_back(currentStats);
        }

        // We use another vector to store all the statistics vectors of each click
        allStats.push_back(currentStatsVec);

        if (!found_pattern) {
            // At the first time, the function didn't find any possible autoclicking pattern yet, so this loop will be entered
            std::vector<std::pair<size_t, size_t>> similarStats;

            // Compare current statistics with previous clicks to detect similar patterns
            for (size_t prevClickIndex = 0; prevClickIndex < allStats.size() - 1; ++prevClickIndex) {
                // Iterate over the statistics of previous clicks
                for (size_t prevStatIndex = 0; prevStatIndex < allStats[prevClickIndex].size(); ++prevStatIndex) {
                    // Iterate over the current statistics
                    for (size_t currentStatIndex = 0; currentStatIndex < allStats.back().size(); ++currentStatIndex) {
                        // Check similarity between current and previous statistics
                        if (areSimilar(allStats[prevClickIndex][prevStatIndex], allStats.back()[currentStatIndex])) { // If this function returns true, it means that it found that the player was clicking similarly at different click intervals
                            // Ensure a minimum click difference of 10 before considering as a similar pattern, because it's pretty common to have similar clicking statistics between consecutive clicks
                            if (globalClickCount - allStats[prevClickIndex][prevStatIndex].clickCount >= 10) {
                                // Found a possible pattern, so we store information about where (clickCount) the pattern occurred at how (statistical metrics) occured
                                similarStats.emplace_back(prevClickIndex, prevStatIndex); 
                            }
                        }
                    }
                }
            }

            // Now, we will attempt to predice if the player is always clicking the same way at the same intervals
            if (!similarStats.empty()) {
                found_pattern = true;
                // Process each set of similar statistics
                for (const auto& pair : similarStats) {
                    size_t i = pair.first;
                    size_t j = pair.second;

                    // Output the detected similarity for debugging
                    std::cout << "Similarity detected." << std::endl;
                    std::cout << "Stats from click " << i << std::endl;
                    printStats(allStats[i][j]);
                    std::cout << "-----------------------------" << std::endl;
                    std::cout << "Stats from click " << globalClickCount << ":" << std::endl;
                    printStats(allStats.back().front());
                    std::cout << "-----------------------------" << std::endl;

                  /*  Calculate where the pattern should be repeated again. This is to see if the player has always the same or similar stats at the same clicking interval
                    * The pattern should be repeated at the same interval. Imagine we detected before that the player clicked equally at click 73 and at click 106
                    * If the player is using an autoclicker that follows a mathematical formula that leads to a consistent frequency rate
                    * the player will repeat the same pattern at (106 - 73) + 106, this is exactly what we're calculating here
                      */
                    difference = globalClickCount - allStats[i][j].clickCount; 
                    prediction = globalClickCount + difference;

                    // Store the prediction and corresponding statistics to check if this prediction is true
                    predictionVector.push_back(allStats.back());
                    predictionValues.push_back(prediction);
                }

                // We need to reset all the statistics because the stats vectors are the average of all the statistical metrics until the last click
                // This means that if we don't clear them, the stats will not be equal at the moment where we predicted that the pattern will be repeated
                // Why? Simple. Let's imagine an autoclicker has a  mechanism of exhaustment (or any other thing) that gets triggered after X clicks to simulate human fatigue
                // But, before getting triggered, the autoclicker randomizes perfectly the time interval between clicks. Therefore, this function will detect different TOTAL stats at the prediction value
                allStats.clear();
                globalClickCount = 0;
            }
        }
        // We will end here inside this else statement since we found a pattern now
        else { 
            // Container to store similarity counts for each prediction
            std::vector<int> similarityCounts(predictionValues.size(), 0);

            // Iterate over each prediction stored in predictionValues
            for (size_t predIdx = 0; predIdx < predictionValues.size(); ++predIdx) {
                // Retrieve the current prediction value
                prediction = predictionValues[predIdx];
                // Flag to track whether similarity is found within the range around the prediction
                bool found_similarity = false;

                // Iterate within the range of 5 clicks before and after the prediction
                // This aims to ensure that if the anticheat was not accurate at detecting the prediction value due to lag or other reasons, we still catch the pattern
                for (size_t m = prediction - 5; m <= prediction + 5; ++m) {
                    // Check if the index is within the bounds of allStats vector as a simple sanity check
                    if (m < allStats.size()) {
                        // Iterate over the statistics for the current click index (m)
                        for (size_t n = 0; n < allStats[m].size(); ++n) {
                            // Check similarity between the last stored statistics of the prediction and the current statistics
                            if (areSimilar(predictionVector[predIdx].back(), allStats[m][n])) {
                                // If similarity is found, increment the similarity count for the prediction, we need to reach 10 in order to be sure our prediction is accurate
                                ++similarityCounts[predIdx];
                                found_similarity = true;
                                // Exit the loop once similarity is found to avoid unnecessary comparisons
                                break;
                            }
                        }
                    }

                    // If similarity is found for a prediction multiple times (10 is perfectly enough to be 100% sure that it's a pattern), flag it as an autoclicker
                    if (similarityCounts[predIdx] >= 10) {
                        std::cout << "Autoclicker detected!" << std::endl;
                        return;
                    }
                }

                // If no similarity is found within the range for the prediction, reset the similarity count to 0
                // because our prediction was incorrect and there's no need to continue wasting computational resources in checking for the same estimation
                if (!found_similarity) {
                    similarityCounts[predIdx] = 0;
                }
            }
        }

        ++globalClickCount;
    }
}