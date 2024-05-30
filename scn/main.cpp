#include "main.hpp"

int main() {
    start();

    std::thread hookThread(install_hook);
    hookThread.join();

    if (clickTimestamps.size() < 2) {
        std::cout << "Not enough data to calculate statistics.\n";
        system("pause");
        return -1;
    }

    std::thread statsThread(start_calculations);
    statsThread.join();

    detect_patterns();
    return 0;
}