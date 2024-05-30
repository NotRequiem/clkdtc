#pragma once

#include "hook\hook.hpp"
#include "pattern\calculations.hpp"
#include "pattern\detection.hpp"

void start() {
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