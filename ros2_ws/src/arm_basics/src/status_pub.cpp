#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class StatusPubNode : public rclcpp::Node
{
public:
  StatusPubNode() : Node("status_pub")
  {
    publisher_ = this->create_publisher<std_msgs::msg::String>(
    "arm_heartbeat",
    rclcpp::QoS(rclcpp::KeepAll()).transient_local().reliable()
    );
    double period_sec = this->declare_parameter<double>("publish_period_sec", 1.0);
    timer_ = this->create_wall_timer(
      std::chrono::duration<double>(period_sec),
      std::bind(&StatusPubNode::timer_callback, this));
  }

private:
  void timer_callback()
  {
    auto message = std_msgs::msg::String();
    message.data = "arm_basics alive";
    publisher_->publish(message);
    RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
  }

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::executors::SingleThreadedExecutor executor;
  auto node = std::make_shared<StatusPubNode>();
  executor.add_node(node->get_node_base_interface());
  executor.spin();
  rclcpp::shutdown();
  return 0;
}