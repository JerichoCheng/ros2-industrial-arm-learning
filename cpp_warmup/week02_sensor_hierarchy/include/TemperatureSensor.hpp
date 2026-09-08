#pragma once

#include "Sensor.hpp"
#include <random>

class TemperatureSensor : public Sensor {
private:
    std::mt19937 gen;
    std::uniform_real_distribution<double> dist;

public:
    explicit TemperatureSensor(std::string sensor_name);
    void read() override;
};