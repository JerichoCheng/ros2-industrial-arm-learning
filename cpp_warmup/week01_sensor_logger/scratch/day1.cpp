#include <iostream>

namespace sensor {
    void greet() {
        std::cout << "程科玮" << std::endl;
    }
}

namespace motor {
    void greet() {
        std::cout << "Jericho" << std::endl;
    }
}

int add(int a, int b) { return a + b; }
double add(double a, double b) { return a + b; }

int main() {
    sensor::greet();
    motor::greet();
    std::cout << add(5, 3) << std::endl;
    std::cout << add(5.5, 3.3) << std::endl;
    return 0;
}