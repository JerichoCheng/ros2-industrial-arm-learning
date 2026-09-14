#include <chrono>
#include <memory>
#include <thread>
#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>

using namespace std::chrono_literals;

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("pick_place_demo");

  // 必须先拉起后台 spinner 驱动回调，否则 MoveGroupInterface 无法接收服务与状态响应
  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node);
  std::thread spinner([&executor]() { executor.spin(); });

  RCLCPP_INFO(node->get_logger(), "Initializing MoveGroup interfaces...");
  moveit::planning_interface::MoveGroupInterface arm_group(node, "panda_arm");
  moveit::planning_interface::MoveGroupInterface hand_group(node, "hand");

  // 基础速度与加速度限制（保证运行稳定）
  arm_group.setMaxVelocityScalingFactor(0.5);
  arm_group.setMaxAccelerationScalingFactor(0.5);

  // ==================== Step 1: 张开夹爪 ====================
  RCLCPP_INFO(node->get_logger(), "[Step 1/4] Opening gripper...");
  hand_group.setNamedTarget("open");
  if (hand_group.move() != moveit::core::MoveItErrorCode::SUCCESS) {
    RCLCPP_ERROR(node->get_logger(), "Failed to open gripper!");
    rclcpp::shutdown();
    spinner.join();
    return 1;
  }
  std::this_thread::sleep_for(1s);

  // ==================== Step 2: 机械臂移动到预抓取位姿 ====================
  RCLCPP_INFO(node->get_logger(), "[Step 2/4] Moving to pick pose...");
  geometry_msgs::msg::Pose pick_pose;
  pick_pose.orientation.x = 1.0;
  pick_pose.orientation.y = 0.0;
  pick_pose.orientation.z = 0.0;
  pick_pose.orientation.w = 0.0;
  pick_pose.position.x = 0.28;
  pick_pose.position.y = -0.2;
  pick_pose.position.z = 0.5;

  arm_group.setPoseTarget(pick_pose);
  moveit::planning_interface::MoveGroupInterface::Plan pick_plan;
  bool pick_plan_success = (arm_group.plan(pick_plan) == moveit::core::MoveItErrorCode::SUCCESS);

  if (!pick_plan_success) {
    RCLCPP_ERROR(node->get_logger(), "Planning to pick pose failed!");
    rclcpp::shutdown();
    spinner.join();
    return 1;
  }

  arm_group.execute(pick_plan);
  std::this_thread::sleep_for(1s);

  // ==================== Step 3: 闭合夹爪（抓取物体） ====================
  RCLCPP_INFO(node->get_logger(), "[Step 3/4] Closing gripper...");
  hand_group.setNamedTarget("close");
  if (hand_group.move() != moveit::core::MoveItErrorCode::SUCCESS) {
    RCLCPP_ERROR(node->get_logger(), "Failed to close gripper!");
    rclcpp::shutdown();
    spinner.join();
    return 1;
  }
  std::this_thread::sleep_for(1s);

  // ==================== Step 4: 机械臂移动到放置位姿 ====================
  RCLCPP_INFO(node->get_logger(), "[Step 4/4] Moving to place pose...");
  geometry_msgs::msg::Pose place_pose;
  place_pose.orientation.x = 1.0;
  place_pose.orientation.y = 0.0;
  place_pose.orientation.z = 0.0;
  place_pose.orientation.w = 0.0;
  place_pose.position.x = 0.28;
  place_pose.position.y = 0.2;
  place_pose.position.z = 0.5;

  arm_group.setPoseTarget(place_pose);
  moveit::planning_interface::MoveGroupInterface::Plan place_plan;
  bool place_plan_success = (arm_group.plan(place_plan) == moveit::core::MoveItErrorCode::SUCCESS);

  if (!place_plan_success) {
    RCLCPP_ERROR(node->get_logger(), "Planning to place pose failed!");
    rclcpp::shutdown();
    spinner.join();
    return 1;
  }

  arm_group.execute(place_plan);
  RCLCPP_INFO(node->get_logger(), "Pick and place sequence completed successfully!");

  rclcpp::shutdown();
  spinner.join();
  return 0;
}