cmake_minimum_required(VERSION 3.16)
project(sensor_project CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译传感器逻辑为静态库
add_library(sensor_lib
    src/TemperatureSensor.cpp
    src/DistanceSensor.cpp
)

# PUBLIC 使得自身和链接该库的目标都能使用 include 目录
target_include_directories(sensor_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 编译主程序并链接传感器库
add_executable(sensor_app src/main.cpp)
target_link_libraries(sensor_app PRIVATE sensor_lib)