# Week 1 Mini-Project: Sensor Data Logger

## Goal

Build a `Sensor` class managed via `std::vector<std::shared_ptr<Sensor>>`, tying together what I'm learning this week:
- `class` encapsulation, access control, constructors/destructors
- References `&` and `const`-correctness
- RAII thinking + `std::shared_ptr`
- STL: `std::string`, `std::vector`, range-based for loops

## Requirements

1. `Sensor` should have at least: a name (`std::string`), a current reading (`double`), and a `read()` method that simulates a changing reading
2. Manage several sensor instances with `std::vector<std::shared_ptr<Sensor>>`
3. Iterate the vector with a range-based for loop, printing each sensor's state
4. Observe when destruction happens (print something in the destructor to see exactly when `shared_ptr` releases the object)

`src/main.cpp` already has the class skeleton with `TODO` markers — filling those in is this week's actual output. Don't copy a finished solution off the internet — getting stuck and working it out via [learncpp.com](https://www.learncpp.com/) or the corresponding textbook chapter is worth a lot more than typing out someone else's answer.

## Build & run

```bash
mkdir build && cd build
cmake ..
make
./sensor_logger
```

## Self-check

- [ ] Can explain why `shared_ptr` is used here instead of a raw pointer or a plain object
- [ ] Compiles cleanly with no warnings (with `-Wall` on)
- [ ] Destructors fire correctly and visibly when the program exits
