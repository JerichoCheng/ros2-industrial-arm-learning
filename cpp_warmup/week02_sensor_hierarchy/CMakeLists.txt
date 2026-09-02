cmake_minimum_required(VERSION 3.10)
project(sensor_hierarchy CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

add_executable(sensor_hierarchy src/main.cpp)
