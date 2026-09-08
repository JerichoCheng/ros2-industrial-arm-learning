#pragma once

#include "Sensor.hpp"
#include <random>

class DistanceSensor : public Sensor {
private:
    std::mt19937 gen;
    std::uniform_real_distribution<double> step_dist;
    std::uniform_real_distribution<double> jump_dist;
    std::bernoulli_distribution jump_chance;

public:
    explicit DistanceSensor(std::string sensor_name);
    void read() override;
};