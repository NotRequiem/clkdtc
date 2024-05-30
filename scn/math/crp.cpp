#include "crp.hpp"

#ifdef _DEBUG

// Calculate the cross recurrence plot (CRP)
/*
 the CRP is represented as a matrix of 0s and 1s.
 A value of 1 indicates that the corresponding values in the two time series are similar (above the threshold)
 while a value of 0 indicates dissimilarity (below the threshold).

 The best threshold to get is the standard desviation.
*/
std::vector<std::vector<int>> calculateCRP(const std::vector<double>& data, double threshold) {
    std::vector<std::vector<int>> crp(data.size(), std::vector<int>(data.size(), 0));

    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data.size(); ++j) {
            if (std::abs(data[i] - data[j]) <= threshold) {
                crp[i][j] = 1;
            }
        }
    }

    return crp;
}
#endif