#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <moveit/move_group_interface/move_group_interface.hpp>

using namespace std::chrono_literals;

namespace {

constexpr double kCurrentStateTimeoutSeconds = 10.0;

// 从 ROS2 参数读取位姿 [x, y, z, qx, qy, qz, qw]；未显式设置时回退到 default_values
bool loadPoseParam(const rclcpp::Node::SharedPtr & node, const std::string & param_name,
                    const std::vector<double> & default_values, geometry_msgs::msg::Pose & out_pose) {
  if (!node->has_parameter(param_name)) {
    node->declare_parameter<std::vector<double>>(param_name, default_values);
  }
  std::vector<double> values = node->get_parameter(param_name).as_double_array();

  if (values.size() != 7) {
    RCLCPP_ERROR(node->get_logger(),
      "Parameter '%s' must contain exactly 7 values [x, y, z, qx, qy, qz, qw], got %zu",
      param_name.c_str(), values.size());
    return false;
  }

  out_pose.position.x = values[0];
  out_pose.position.y = values[1];
  out_pose.position.z = values[2];
  out_pose.orientation.x = values[3];
  out_pose.orientation.y = values[4];
  out_pose.orientation.z = values[5];
  out_pose.orientation.w = values[6];
  return true;
}

bool waitForCurrentState(moveit::planning_interface::MoveGroupInterface & group,
                         const rclcpp::Logger & logger, const std::string & group_name) {
  if (group.getCurrentState(kCurrentStateTimeoutSeconds)) {
    return true;
  }

  RCLCPP_ERROR(logger,
    "Timed out waiting %.1f seconds for a current robot state for planning group '%s'. "
    "Ensure joint_state_broadcaster and controllers are active before starting this demo.",
    kCurrentStateTimeoutSeconds, group_name.c_str());
  return false;
}

bool moveGripperToNamedTarget(moveit::planning_interface::MoveGroupInterface & hand_group,
                              const rclcpp::Logger & logger, const std::string & target_name,
                              const std::string & description) {
  if (!hand_group.setNamedTarget(target_name)) {
    RCLCPP_ERROR(logger, "Named gripper target '%s' is not defined.", target_name.c_str());
    return false;
  }

  moveit::planning_interface::MoveGroupInterface::Plan plan;
  if (hand_group.plan(plan) != moveit::core::MoveItErrorCode::SUCCESS) {
    RCLCPP_ERROR(logger, "Planning %s failed!", description.c_str());
    return false;
  }

  if (hand_group.execute(plan) != moveit::core::MoveItErrorCode::SUCCESS) {
    RCLCPP_ERROR(logger, "Execution %s failed!", description.c_str());
    return false;
  }

  return true;
}

}  // namespace

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  rclcpp::NodeOptions node_options;
  node_options.automatically_declare_parameters_from_overrides(true);
  auto node = rclcpp::Node::make_shared("pick_place_demo", node_options);

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

  // TimerAction only delays process startup. It cannot prove controller state is ready.
  if (!waitForCurrentState(arm_group, node->get_logger(), "panda_arm") ||
      !waitForCurrentState(hand_group, node->get_logger(), "hand")) {
    rclcpp::shutdown();
    spinner.join();
    return 1;
  }

  // MoveIt's state monitor also needs the latched /tf_static transform for the
  // virtual joint. Give its private TF buffer a callback cycle, then refresh the
  // start state before issuing the first MoveGroup goal.
  std::this_thread::sleep_for(1s);
  if (!waitForCurrentState(arm_group, node->get_logger(), "panda_arm")) {
    rclcpp::shutdown();
    spinner.join();
    return 1;
  }
  arm_group.setStartStateToCurrentState();
  hand_group.setStartStateToCurrentState();

  // Pick / Place 目标位姿改为 ROS2 参数，默认值与原硬编码位姿保持一致
  geometry_msgs::msg::Pose pick_pose;
  geometry_msgs::msg::Pose place_pose;
  if (!loadPoseParam(node, "pick_pose", {0.28, -0.2, 0.5, 1.0, 0.0, 0.0, 0.0}, pick_pose) ||
      !loadPoseParam(node, "place_pose", {0.28, 0.2, 0.5, 1.0, 0.0, 0.0, 0.0}, place_pose)) {
    rclcpp::shutdown();
    spinner.join();
    return 1;
  }

  // ==================== Step 1: 张开夹爪 ====================
  RCLCPP_INFO(node->get_logger(), "[Step 1/4] Opening gripper...");
  if (!moveGripperToNamedTarget(hand_group, node->get_logger(), "open", "to open gripper")) {
    rclcpp::shutdown();
    spinner.join();
    return 1;
  }
  std::this_thread::sleep_for(1s);

  // ==================== Step 2: 机械臂移动到预抓取位姿 ====================
  RCLCPP_INFO(node->get_logger(), "[Step 2/4] Moving to pick pose...");
  arm_group.setPoseTarget(pick_pose);
  moveit::planning_interface::MoveGroupInterface::Plan pick_plan;
  bool pick_plan_success = (arm_group.plan(pick_plan) == moveit::core::MoveItErrorCode::SUCCESS);

  if (!pick_plan_success) {
    RCLCPP_ERROR(node->get_logger(), "Planning to pick pose failed!");
    rclcpp::shutdown();
    spinner.join();
    return 1;
  }

  if (arm_group.execute(pick_plan) != moveit::core::MoveItErrorCode::SUCCESS) {
    RCLCPP_ERROR(node->get_logger(), "Execution to pick pose failed!");
    rclcpp::shutdown();
    spinner.join();
    return 1;
  }
  std::this_thread::sleep_for(1s);

  // ==================== Step 3: 闭合夹爪（抓取物体） ====================
  RCLCPP_INFO(node->get_logger(), "[Step 3/4] Closing gripper...");
  if (!moveGripperToNamedTarget(hand_group, node->get_logger(), "close", "to close gripper")) {
    rclcpp::shutdown();
    spinner.join();
    return 1;
  }
  std::this_thread::sleep_for(1s);

  // ==================== Step 4: 机械臂移动到放置位姿 ====================
  RCLCPP_INFO(node->get_logger(), "[Step 4/4] Moving to place pose...");
  arm_group.setPoseTarget(place_pose);
  moveit::planning_interface::MoveGroupInterface::Plan place_plan;
  bool place_plan_success = (arm_group.plan(place_plan) == moveit::core::MoveItErrorCode::SUCCESS);

  if (!place_plan_success) {
    RCLCPP_ERROR(node->get_logger(), "Planning to place pose failed!");
    rclcpp::shutdown();
    spinner.join();
    return 1;
  }

  if (arm_group.execute(place_plan) != moveit::core::MoveItErrorCode::SUCCESS) {
    RCLCPP_ERROR(node->get_logger(), "Execution to place pose failed!");
    rclcpp::shutdown();
    spinner.join();
    return 1;
  }
  RCLCPP_INFO(node->get_logger(), "Pick and place sequence completed successfully!");

  rclcpp::shutdown();
  spinner.join();
  return 0;
}
