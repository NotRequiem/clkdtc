#pragma once

#include "..\math\math.h"
#include "..\include.h"

struct IntervalStats {
    double meanInterval;
    double stdDev;
    double variance;
    double skewness;
    double kurtosis;
    double avgClicksPerSec;
    double serialCorrelation;
    double entropy;
};

std::vector<IntervalStats> allStats;
const int monitorCount = 50;
const int threshold = 2;
const int repeatCount = 10;

void detect_patterns();
