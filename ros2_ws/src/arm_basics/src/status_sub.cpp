#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class StatusSubNode : public rclcpp::Node
{
public:
  StatusSubNode() : Node("status_sub")
  {
    // 回调函数接收一条消息的 const 共享指针，并用 std::placeholders::_1 占位传参
    subscription_ = this->create_subscription<std_msgs::msg::String>(
    "arm_heartbeat",
    rclcpp::QoS(0).keep_all().transient_local().reliable(),
    std::bind(&StatusSubNode::topic_callback, this, std::placeholders::_1));
  }

private:
  void topic_callback(const std_msgs::msg::String::SharedPtr msg) const
  {
    RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
  }

  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::executors::SingleThreadedExecutor executor;
  auto node = std::make_shared<StatusSubNode>();
  executor.add_node(node->get_node_base_interface());
  executor.spin();
  rclcpp::shutdown();
  return 0;
}