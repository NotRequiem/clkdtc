#include "main.hpp"

int main() {
    start();

    std::thread mouseThread(install_hook);
    mouseThread.join();

    std::thread analysisThread(detect_patterns);
    analysisThread.join();

    return 0;
}