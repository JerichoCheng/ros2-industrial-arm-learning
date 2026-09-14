#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("debug_test_node");

  moveit::planning_interface::MoveGroupInterface* mgi = nullptr;  // 故意不初始化
  auto pose = mgi->getCurrentPose();  // 崩溃点

  RCLCPP_INFO(node->get_logger(), "pose z = %f", pose.pose.position.z);
  rclcpp::shutdown();
  return 0;
}