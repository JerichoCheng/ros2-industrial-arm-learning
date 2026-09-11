#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_srvs/srv/trigger.hpp"

class StatusSubNode : public rclcpp::Node
{
public:
  StatusSubNode() : Node("status_sub")
  {
    // 1. 订阅端保持不变（使用与 status_pub 兼容的默认/显式 QoS）
    subscription_ = this->create_subscription<std_msgs::msg::String>(
      "arm_heartbeat",
      rclcpp::QoS(rclcpp::KeepAll()).transient_local().reliable(),
      std::bind(&StatusSubNode::topic_callback, this, std::placeholders::_1));

    // 2. 新增 Service 服务端，使用 Lambda 捕获 this 实现回调
    reset_srv_ = this->create_service<std_srvs::srv::Trigger>(
      "reset_heartbeat_count",
      [this](
        const std::shared_ptr<std_srvs::srv::Trigger::Request> /*request*/,
        std::shared_ptr<std_srvs::srv::Trigger::Response> response)
      {
        int prev_count = heartbeat_count_;
        heartbeat_count_ = 0;

        response->success = true;
        response->message = "Heartbeat count reset to 0 (was " + std::to_string(prev_count) + ")";

        RCLCPP_INFO(
          this->get_logger(),
          "Service reset_heartbeat_count invoked. Reset count from %d to 0",
          prev_count
        );
      }
    );
  }

private:
  // 去掉 const，累加心跳计数并打印
  void topic_callback(const std_msgs::msg::String::SharedPtr msg)
  {
    heartbeat_count_++;
    RCLCPP_INFO(
      this->get_logger(),
      "I heard: '%s' | [Count: %d]",
      msg->data.c_str(),
      heartbeat_count_
    );
  }

  int heartbeat_count_ = 0;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr reset_srv_;
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