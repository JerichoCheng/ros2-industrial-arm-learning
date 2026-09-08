#include "TemperatureSensor.hpp"

TemperatureSensor::TemperatureSensor(std::string sensor_name)
    : Sensor(std::move(sensor_name), 25.0),
      gen(std::random_device{}()),
      dist(-0.4, 0.4) {}

void TemperatureSensor::read() {
    value += dist(gen);
    if (value < 20.0) value = 20.0;
    if (value > 30.0) value = 30.0;

    if (callback) {
        callback(name, value);
    }
}