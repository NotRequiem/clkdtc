#include "pattern.hpp"
#include <unordered_map>
#include <utility>

// Custom hash function for std::pair<size_t, size_t>
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 ^ hash2;
    }
};

struct IntervalStats {
    size_t clickCount;
    double correlation;
    double coefficient;
    double iqr;
    double entropy;
    double mode;
    double mean;
    double median;
    double stddev;
    double avgClicksPerSec;
};

// Checks if all the statisticals of the members of a statistical vector are very similar to all the corresponding members of another statistical vector
static bool areSimilar(const IntervalStats& stats1, const IntervalStats& stats2) {
    // Check if both stats are all zeros or nulls as a sanity check
    if (stats1.correlation == 0 && stats1.coefficient == 0 && stats1.iqr == 0 && stats1.entropy == 0 &&
        stats1.mode == 0 && stats1.mean == 0 && stats1.median == 0 && stats1.stddev == 0 && stats1.avgClicksPerSec == 0 &&
        stats2.correlation == 0 && stats2.coefficient == 0 && stats2.iqr == 0 && stats2.entropy == 0 &&
        stats2.mode == 0 && stats2.mean == 0 && stats2.median == 0 && stats2.stddev == 0 && stats2.avgClicksPerSec == 0) {
        return false;
    }

    double threshold = 0.2; // 0.2 instead of 2 for more precision, but this could be changed to 2 without false positives problems due to checking if the pattern is repeated over and over
    return (std::abs(stats1.correlation - stats2.correlation) < threshold &&
        std::abs(stats1.coefficient - stats2.coefficient) < threshold &&
        std::abs(stats1.iqr - stats2.iqr) < threshold &&
        std::abs(stats1.entropy - stats2.entropy) < threshold &&
        std::abs(stats1.mode - stats2.mode) < threshold &&
        std::abs(stats1.mean - stats2.mean) < threshold &&
        std::abs(stats1.median - stats2.median) < threshold &&
        std::abs(stats1.stddev - stats2.stddev) < threshold &&
        std::abs(stats1.avgClicksPerSec - stats2.avgClicksPerSec) < threshold);
}

static void printStats(const IntervalStats& stats) {
    std::cout << "Serial Correlation: " << stats.correlation << "\n";
    std::cout << "Coefficient: " << stats.coefficient << "\n";
    std::cout << "IQR: " << stats.iqr << "\n";
    std::cout << "Entropy: " << stats.entropy << "\n";
    std::cout << "Mode: " << stats.mode << "\n";
    std::cout << "Mean: " << stats.mean << "\n";
    std::cout << "Median: " << stats.median << "\n";
    std::cout << "Standard Deviation: " << stats.stddev << "\n";
    std::cout << "Average Clicks per Second: " << stats.avgClicksPerSec << "\n";
}

std::vector<std::vector<IntervalStats>> allStats;
std::vector<std::vector<IntervalStats>> predictionVector;
std::vector<size_t> predictionValues;
std::unordered_map<std::pair<size_t, size_t>, bool, pair_hash> analyzedPatterns;
size_t min = 50;
size_t globalClickCount = 0;
size_t difference = 0;
size_t prediction = 0;
int similarityCount = 0;
bool found_pattern = false;

// Make sure the detect_patterns function runs in a separate thread in a production case
// Also, you might want to add memory checks so that the thread does not allocate more than what you want in the buffer for your autoclicker check
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
            double mean = calculateMean(currentIntervals);
            double correlation = calculateSerialCorrelation(currentIntervals);
            double coefficient = calculateCoefficient(currentIntervals);
            double iqr = calculateIQR(currentIntervals);
            double entropy = calculateEntropy(currentIntervals);
            double mode = calculateMode(currentIntervals);
            double median = calculateMedian(currentIntervals);
            double stddev = calculateStandardDeviation(currentIntervals, mean);
            double avgClicksPerSec = 1.0 / mean;

            IntervalStats currentStats = { static_cast<size_t>(globalClickCount), mean, correlation, coefficient, iqr, entropy, mode, median, stddev, avgClicksPerSec };
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
                        if (areSimilar(allStats[prevClickIndex][prevStatIndex], allStats.back()[currentStatIndex])) {
                            // Ensure a minimum click difference of 10 before considering as a similar pattern, because it's pretty common to have similar clicking statistics between consecutive clicks
                            if (globalClickCount - allStats[prevClickIndex][prevStatIndex].clickCount >= 10) {
                                // Check if this similarity has already been analyzed
                                if (analyzedPatterns.find({ prevClickIndex, prevStatIndex }) == analyzedPatterns.end()) {
                                    // Found a possible pattern, so we store information about where (clickCount) the pattern occurred at how (statistical metrics) occurred
                                    similarStats.emplace_back(prevClickIndex, prevStatIndex);
                                }
                            }
                        }
                    }
                }
            }

            // Now, we will attempt to predict if the player is always clicking the same way at the same intervals
            if (!similarStats.empty()) {
                found_pattern = true;
                // Process each set of similar statistics
                for (const auto& pair : similarStats) {
                    size_t i = pair.first;
                    size_t j = pair.second;

                    // Output the detected similarity for debugging purposes
                    std::cout << "Similarity detected.\n";
                    std::cout << "-----------------------------\n";
                    std::cout << "Stats from click " << i << "\n";
                    printStats(allStats[i][j]);
                    std::cout << "-----------------------------\n";
                    std::cout << "Stats from click " << globalClickCount << ":\n";
                    printStats(allStats.back().front());
                    std::cout << "-----------------------------\n";

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

                    // Add this similarity to the map to avoid future redundant checks
                    analyzedPatterns[{i, j}] = true;
                }

                // We need to reset all the statistics because the stats vectors are the average of all the statistical metrics until the last click
                // This means that if we don't clear them, the stats will not be equal at the moment where we predicted that the pattern will be repeated
                // Why? Simple. Let's imagine an autoclicker has a mechanism of exhaustion (or any other thing) that gets triggered after X clicks to simulate human fatigue
                // But, before getting triggered, the autoclicker randomizes perfectly the time interval between clicks. Therefore, this function will detect different TOTAL stats at the prediction value
                allStats.clear();
                globalClickCount = 0;
            }
        }
        // We will end here inside this else statement since we found a pattern now
        else {
            std::cout << "Analyzing pattern\n";
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
                        std::cout << "Autoclicker detected!\n";
                        return;
                    }
                }

                // If no similarity is found within the range for the prediction, reset the similarity count to 0
                // because our prediction was incorrect and there's no need to continue wasting computational resources in checking for the same estimation
                if (!found_similarity) {
                    similarityCounts[predIdx] = 0;
                    found_pattern = false;
                    std::cout << "Prediction failed, scanning for other possible patterns...\n";
                    break;
                }
            }
        }

        ++globalClickCount;
    }
}
