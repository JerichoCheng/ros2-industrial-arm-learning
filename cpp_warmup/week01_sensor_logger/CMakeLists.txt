cmake_minimum_required(VERSION 3.10)
project(sensor_logger CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

add_executable(sensor_logger src/main.cpp)
