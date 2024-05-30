#pragma once

#include <vector>
#include <iostream>

void detectSpikesAndOutliers(const std::vector<double>& data, double mean, double stdDev);
