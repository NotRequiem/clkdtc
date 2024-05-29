#include <Windows.h>
#include <vector>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <cmath>
#include <random>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>

HHOOK hMouseHook;
int monitorClicks = 0;
int clickCount = 0;
WPARAM monitorButton;
std::vector<double> clickTimestamps;

void SpawnConsole() {
    if (!AllocConsole()) {
        fprintf(stderr, "Failed to allocate console: %lu\n", GetLastError());
        return;
    }

    FILE* file;
    freopen_s(&file, "CONOUT$", "w", stdout);
    freopen_s(&file, "CONOUT$", "w", stderr);
    freopen_s(&file, "CONIN$", "r", stdin);

    HANDLE hConout = CreateFileA("CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hConout == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Failed to open CONOUT$ handle: %lu\n", GetLastError());
        return;
    }

    if (!SetStdHandle(STD_OUTPUT_HANDLE, hConout) || !SetStdHandle(STD_ERROR_HANDLE, hConout)) {
        fprintf(stderr, "Failed to set standard handles: %lu\n", GetLastError());
        CloseHandle(hConout);
        return;
    }
}

void askUserForDetails() {
    int buttonChoice;
    std::cout << "Which mouse button do you want to monitor?\n";
    std::cout << "1. Left Button\n";
    std::cout << "2. Right Button\n";
    std::cout << "3. Middle Button\n";
    std::cout << "4. Side/Extended Button\n";
    std::cin >> buttonChoice;

    switch (buttonChoice) {
    case 1:
        monitorButton = WM_LBUTTONDOWN;
        break;
    case 2:
        monitorButton = WM_RBUTTONDOWN;
        break;
    case 3:
        monitorButton = WM_MBUTTONDOWN;
        break;
    case 4:
        monitorButton = WM_XBUTTONDOWN;
        break;
    default:
        std::cout << "Invalid choice. Defaulting to Left Button.\n";
        monitorButton = WM_LBUTTONDOWN;
        break;
    }

    std::cout << "Enter the number of clicks to monitor: ";
    std::cin >> monitorClicks;
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == monitorButton) {
            clickCount++;
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = now.time_since_epoch();
            double timestamp = std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
            clickTimestamps.push_back(timestamp);
            std::cout << "Mouse event detected. Click count: " << clickCount << "\n";

            if (clickCount >= monitorClicks) {
                UnhookWindowsHookEx(hMouseHook);
                PostQuitMessage(0);
                return 0;
            }
        }
    }

    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

void InstallHook() {
    hMouseHook = SetWindowsHookExA(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(NULL), 0);
    if (!hMouseHook) {
        std::cout << "Failed to install mouse hook. Error code: " << GetLastError() << "\n";
        return;
    }

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

// This helps establish a baseline for normal clicking behavior.
double calculateMean(const std::vector<double>& data) {
    // Mean is the average value of the dataset
    return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}

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

double calculateStandardDeviation(const std::vector<double>& data, double mean) {
    /*
    The square root of variance, giving a measure of dispersion in the same units as the original data.
    It is useful for identifying how spread out the intervals are. 
    This check is not reliable on it's own, it should be combined with another statistical value (example: kurtosis)

    In an example case of having very low standard desviations, with low outliers and therefore negatively skewned data at the same time, this check is reliable after a long time.
    This essentially would check for standard desviation (consistent clicking) while also having low outliers (high clicks per second) for a long period of time
    Therefore, this would flag players clicking too consistently while clicking fastly for a long period of time or click amounts
    */
    return std::sqrt(calculateVariance(data, mean));
}

/*
Measures the asymmetry of the distribution. 
Positive skewness indicates that intervals are more spread out on the right (longer intervals)
while negative skewness indicates they are more spread out on the left (shorter intervals)
*/
double calculateSkewness(const std::vector<double>& data, double mean, double stdDev) {
    double m3 = 0.0;
    for (double value : data) {
        double diff = value - mean;
        m3 += std::pow(diff / stdDev, 3);
    }

    // original formula should be: return (data.size() / ((data.size() - 1) * (data.size() - 2))) * m3;
    // however, I cast the return statement to prevent the integer division result from being truncated before casting to a floating point

    return (data.size() / ((data.size() - static_cast<double>(1)) * (data.size() - 2))) * m3;
}

/*
Measures the "tailedness" of the distribution. 
High kurtosis indicates more outliers (heavy tails), while low kurtosis indicates fewer outliers (light tails)
This is also the reason why there's no need to directly calculate the outliers
*/
double calculateKurtosis(const std::vector<double>& data, double mean, double stdDev) {
    // Kurtosis measures the "tailedness" of the data distribution

    double m4 = 0.0;
    for (double value : data) {
        double diff = value - mean;
        m4 += std::pow(diff / stdDev, 4);
    }
    double n = static_cast<double>(data.size());
    double numerator = n * (n + 1) * m4 - 3 * (n - 1) * (n - 1);
    return numerator / ((n - 1) * (n - 2) * (n - 3));
}

void detectSpikesAndOutliers(const std::vector<double>& data, double mean, double stdDev) {
    std::cout << "Outliers (more than 4 STD from mean):\n";
    // a very low number of STD from the mean to compare would false flag human clicking, it should be as high as possible
    for (double value : data) {
        // Outliers are data points that are more than 2 standard deviations away from the mean
        // Normally, you wont have more than one outlier with more than 2 standard desviation
        if (std::abs(value - mean) > 4 * stdDev) {
            std::cout << value << "\n";
        }
        else {
            std::cout << "Non suspicious outliers detected\n" << std::endl;
        }
    }

    std::cout << "Spikes (adjacent differences more than 4 STD):\n";
    for (size_t i = 1; i < data.size(); ++i) {
        // Spikes are large changes between consecutive data points, you can have multiple of them
        if (std::abs(data[i] - data[i - 1]) > 4 * stdDev) {
            std::cout << data[i - 1] << " -> " << data[i] << "\n";
        }
        else {
            std::cout << "Non suspicious spikes detected\n" << std::endl;
        }
    }
}

// Measures how two variables change together.
double calculateCovariance(const std::vector<double>& dataX, const std::vector<double>& dataY, double meanX, double meanY) {
    double covariance = 0.0;
    // Here, it’s used to measure the relationship between click intervals and other statistics

    for (size_t i = 0; i < dataX.size(); ++i) {
        covariance += (dataX[i] - meanX) * (dataY[i] - meanY);
    }
    return covariance / dataX.size();
}

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

// Shannon formula btw
// Entropy measures the randomness or unpredictability of a system
// I calculate entropy based on the distribution of click intervals. High entropy suggests more randomness
double calculateEntropy(const std::vector<double>& data) {
    // Count the frequency of each unique click interval
    std::unordered_map<double, int> frequencyMap;
    for (double interval : data) {
        frequencyMap[interval]++;
    }

    // Calculate the probability of each interval
    std::vector<double> probabilities;
    for (const auto& pair : frequencyMap) {
        double probability = static_cast<double>(pair.second) / data.size();
        probabilities.push_back(probability);
    }

    // Calculate entropy
    double entropy = 0.0;
    for (double probability : probabilities) {
        entropy -= probability * std::log2(probability);
    }

    return entropy;
}

// RQA ---------------------
// Function to calculate the Euclidean distance between two points in a multidimensional space
double euclideanDistance(const std::vector<double>& point1, const std::vector<double>& point2) {
    double sum = 0.0;
    for (size_t i = 0; i < point1.size(); ++i) {
        sum += std::pow(point1[i] - point2[i], 2);
    }
    return std::sqrt(sum);
}

// Function to create a recurrence plot from a time series
std::vector<std::vector<int>> createRecurrencePlot(const std::vector<double>& timeSeries, double threshold) {
    std::vector<std::vector<int>> recurrencePlot(timeSeries.size(), std::vector<int>(timeSeries.size(), 0));
    for (size_t i = 0; i < timeSeries.size(); ++i) {
        for (size_t j = 0; j < timeSeries.size(); ++j) {
            recurrencePlot[i][j] = (euclideanDistance({ timeSeries[i] }, { timeSeries[j] }) <= threshold) ? 1 : 0;
        }
    }
    return recurrencePlot;
}

// Function to calculate RQA measures from a recurrence plot
std::tuple<double, double, double> calculateRQA(const std::vector<std::vector<int>>& recurrencePlot) {
    int numRecurrences = 0;
    int numLaminations = 0;
    int longestDiagonal = 0;

    for (size_t i = 0; i < recurrencePlot.size(); ++i) {
        for (size_t j = 0; j < recurrencePlot[i].size(); ++j) {
            if (recurrencePlot[i][j] == 1) {
                numRecurrences++;
                int diagonalLength = 1;
                while (i + diagonalLength < recurrencePlot.size() && j + diagonalLength < recurrencePlot[i].size() &&
                    recurrencePlot[i + diagonalLength][j + diagonalLength] == 1) {
                    diagonalLength++;
                }
                if (diagonalLength > longestDiagonal) {
                    longestDiagonal = diagonalLength;
                }
                if (i > 0 && recurrencePlot[i - 1][j - 1] == 0 && j > 0 && recurrencePlot[i][j - 1] == 0) {
                    numLaminations++;
                }
            }
        }
    }

    double recurrenceRate = static_cast<double>(numRecurrences) / (recurrencePlot.size() * recurrencePlot.size());
    double determinism = static_cast<double>(longestDiagonal) / recurrencePlot.size();
    double laminarity = static_cast<double>(numLaminations) / numRecurrences;

    return std::make_tuple(recurrenceRate, determinism, laminarity);
}

std::tuple<std::vector<double>, double> getIntervalsWithThreshold() {
    if (clickTimestamps.size() < 2) {
        std::cerr << "Not enough data to calculate intervals and threshold.\n";
        return std::make_tuple(std::vector<double>(), 0.0);
    }

    std::vector<double> intervals;
    for (size_t i = 1; i < clickTimestamps.size(); ++i) {
        intervals.push_back(clickTimestamps[i] - clickTimestamps[i - 1]);
    }

    double meanInterval = calculateMean(intervals);
    double stdDev = calculateStandardDeviation(intervals, meanInterval);

    return std::make_tuple(intervals, meanInterval + 2 * stdDev);
}

void printRQAMeasures(double recurrenceRate, double determinism, double laminarity) {
    std::cout << "Recurrence Rate: " << recurrenceRate << "\n";
    std::cout << "Determinism: " << determinism << "\n";
    std::cout << "Laminarity: " << laminarity << "\n";
}

void analyzeRQA(const std::vector<double>& timeSeries, double threshold) {
    std::vector<std::vector<int>> recurrencePlot = createRecurrencePlot(timeSeries, threshold);
    auto [recurrenceRate, determinism, laminarity] = calculateRQA(recurrencePlot);
    printRQAMeasures(recurrenceRate, determinism, laminarity);
}

// After getting a lot of statistical values for LEGIT data only, we can use the Gaussian distribution
// This would compare our human click behavior with actual clicking behavior

void printStatistics() {
    // This is done because we can't calculate an average of the data with only 1 sample.
    // Even if its a number like 2 or 3, we won't be able to calculate the kurtosis, skewness and covariance of the whole click data
    if (clickTimestamps.size() < 2) {
        std::cout << "Not enough data to calculate statistics.\n";
        return;
    }

    std::vector<double> intervals;
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

    // The maximum number of decimal places you can print with precise pointer precision (assuming the default precision of double type in C++) is typically around 15 to 17 decimal places.
    std::cout << std::setprecision(15);

    std::cout << "Mean Interval: " << meanInterval << " seconds\n";
    std::cout << "Standard Deviation: " << stdDev << "\n";
    std::cout << "Variance: " << variance << "\n";
    std::cout << "Skewness: " << skewness << "\n";
    std::cout << "Kurtosis: " << kurtosis << "\n";
    std::cout << "Average Clicks per Second: " << avgClicksPerSec << "\n";
    std::cout << "Serial Correlation: " << serialCorrelation << "\n";
    std::cout << "Entropy: " << entropy << "\n";

    detectSpikesAndOutliers(intervals, meanInterval, stdDev);

    // Calculate covariance between intervals and statistics
    // This is because we're interested in understanding the relationship between intervals and the overall statistical properties of the data
    std::vector<double> stats = { meanInterval, stdDev, skewness, kurtosis, avgClicksPerSec };
    double meanStats = calculateMean(stats);

    std::vector<double> deviations;
    for (double interval : intervals) {
        deviations.push_back(interval - meanInterval);
    }

    double covariance = calculateCovariance(deviations, stats, 0.0, meanStats);
    std::cout << "Covariance between intervals and statistics: " << covariance << "\n";

    auto [intervalsForRQA, threshold] = getIntervalsWithThreshold();
    analyzeRQA(intervalsForRQA, threshold);
}

int main() {
    SpawnConsole();
    askUserForDetails();
    InstallHook();
    printStatistics();
    return 0;
}
