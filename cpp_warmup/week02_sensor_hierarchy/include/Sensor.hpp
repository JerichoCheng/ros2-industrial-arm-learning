#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <utility>

class Sensor {
public:
    using Callback = std::function<void(const std::string&, double)>;

protected:
    std::string name;
    double value;
    Callback callback;

public:
    Sensor(std::string sensor_name, double initial_val)
        : name(std::move(sensor_name)), value(initial_val), callback(nullptr) {}

    virtual ~Sensor() {
        std::cout << "[析构] " << name << " 传感器销毁" << std::endl;
    }

    void setCallback(Callback cb) {
        callback = std::move(cb);
    }

    virtual void read() = 0;

    std::string getName() const { return name; }
    double getValue() const { return value; }
};