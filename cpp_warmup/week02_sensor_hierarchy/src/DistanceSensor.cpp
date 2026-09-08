#include "DistanceSensor.hpp"

DistanceSensor::DistanceSensor(std::string sensor_name)
    : Sensor(std::move(sensor_name), 2.5),
      gen(std::random_device{}()),
      step_dist(-0.05, 0.05),
      jump_dist(0.1, 5.0),
      jump_chance(0.3) {}

void DistanceSensor::read() {
    if (jump_chance(gen)) {
        value = jump_dist(gen);
    } else {
        value += step_dist(gen);
        if (value < 0.1) value = 0.1;
        if (value > 5.0) value = 5.0;
    }

    if (callback) {
        callback(name, value);
    }
}