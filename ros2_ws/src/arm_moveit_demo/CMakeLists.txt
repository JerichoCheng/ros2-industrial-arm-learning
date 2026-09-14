cmake_minimum_required(VERSION 3.8)
project(arm_moveit_demo)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(moveit_ros_planning_interface REQUIRED)


add_executable(first_motion src/first_motion.cpp)
# 1. 声明新的可执行文件 target
add_executable(debug_test src/debug_test.cpp)
add_executable(pick_place_demo src/pick_place_demo.cpp)

# 2. 链接 MoveIt2 和 ROS 2 的依赖库（直接复用你现有节点的依赖列表）
ament_target_dependencies(debug_test
  rclcpp
  moveit_ros_planning_interface
)

ament_target_dependencies(pick_place_demo
  rclcpp
  moveit_ros_planning_interface
)

# 3. 安装到 lib 目录，保证 ros2 run 能找到它
install(TARGETS
  debug_test
  DESTINATION lib/${PROJECT_NAME}
)

target_link_libraries(first_motion
  rclcpp::rclcpp
  moveit_ros_planning_interface::moveit_move_group_interface)

install(TARGETS
  first_motion
  DESTINATION lib/${PROJECT_NAME})

install(TARGETS
  pick_place_demo
  DESTINATION lib/${PROJECT_NAME}
)
  
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
