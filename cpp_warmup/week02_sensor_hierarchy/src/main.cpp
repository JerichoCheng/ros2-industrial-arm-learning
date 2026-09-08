#include "TemperatureSensor.hpp"
#include "DistanceSensor.hpp"
#include <iostream>
#include <memory>
#include <vector>

int main() {
    std::vector<std::shared_ptr<Sensor>> sensors;
    sensors.push_back(std::make_shared<TemperatureSensor>("Cabin_Temp"));
    sensors.push_back(std::make_shared<DistanceSensor>("Front_Sonar"));
    sensors.push_back(std::make_shared<DistanceSensor>("Rear_Lidar"));

    for (auto& sensor : sensors) {
        sensor->setCallback([](const std::string& name, double val) {
            std::cout << "[Event Callback] " << name << " -> 最新数据: " << val << std::endl;
        });
    }

    std::cout << "--- 模拟数据采集流程 ---" << std::endl;
    for (int step = 1; step <= 2; ++step) {
        std::cout << "\n>> Step " << step << " 采集:" << std::endl;
        for (const auto& sensor : sensors) {
            sensor->read();
        }
    }

    std::cout << "\n--- 退出作用域 ---" << std::endl;
    return 0;
}