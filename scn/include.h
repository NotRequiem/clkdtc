#pragma once

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
#include <thread>
#include <iomanip>
#include <mutex>
#include <condition_variable>

extern std::vector<double> clickTimestamps;
extern std::condition_variable cv;

extern int monitorClicks;
extern int clickCount;

extern WPARAM monitorButton;
extern std::mutex dataMutex;