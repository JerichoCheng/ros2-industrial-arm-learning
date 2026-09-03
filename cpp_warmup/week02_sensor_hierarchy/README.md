# Week 2 Mini-Project: Sensor Inheritance Hierarchy

## Goal

Turn Week 1's `Sensor` into an inheritance hierarchy: `TemperatureSensor : public Sensor`, `DistanceSensor : public Sensor`, tying together what I'm learning this week:
- Inheritance and polymorphism: `virtual` functions, `override`, pure virtual functions (abstract base classes)
- Lambda expressions + `std::function`
- Hand-writing a CMakeLists.txt (no ROS 2 dependency, to actually verify I understand the build process)

This is the real test of Weeks 1-2: not "I get it," but "it compiles and runs."

## Requirements

1. Turn `Sensor` into an abstract base class: at least one pure virtual function, `virtual double read() = 0;`
2. `TemperatureSensor` and `DistanceSensor` each `override read()`, returning a different flavor of simulated data
3. Manage both subclass types uniformly via `std::vector<std::shared_ptr<Sensor>>` (this is where you feel polymorphism: calling through a base-class pointer actually runs the subclass's implementation)
4. Use a `std::function<void(const Sensor&)>` callback (pass it a lambda) to "process" each sensor — e.g. print a warning if a reading exceeds some threshold

`src/main.cpp` already has the skeleton — fill in the `TODO`s.

## Build & run

```bash
mkdir build && cd build
cmake ..
make
./sensor_hierarchy
```

## Self-check

- [ ] Can explain clearly why the base class destructor needs to be `virtual` (think through what happens if it isn't)
- [ ] Can explain the difference between a "pure virtual function" and a regular virtual function in one sentence
- [ ] The lambda callback works correctly, and I can explain the difference between `[this]`, `[&]`, and `[=]` capture modes
