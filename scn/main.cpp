#include "main.hpp"

int main() {
    start();

    install_hook();

    std::cout << std::setprecision(15);
    detect_patterns();

    return 0;
}