#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/trigger.hpp"

using namespace std::chrono_literals;

class StatusClientNode : public rclcpp::Node
{
public:
  StatusClientNode() : Node("status_client")
  {
    // 构造函数只做纯粹的通信句柄初始化
    client_ = this->create_client<std_srvs::srv::Trigger>("reset_heartbeat_count");
  }

  bool wait_for_server(std::chrono::seconds timeout = 5s)
{
  auto start_time = std::chrono::steady_clock::now();

  while (!client_->wait_for_service(1s)) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(this->get_logger(), "Interrupted while waiting for service.");
      return false;
    }

    // 检查是否已超时
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::steady_clock::now() - start_time);
    if (elapsed >= timeout) {
      RCLCPP_ERROR(
        this->get_logger(),
        "Service /reset_heartbeat_count not available after %ld seconds. Timed out.",
        timeout.count());
      return false;
    }

    RCLCPP_INFO(this->get_logger(), "Waiting for service /reset_heartbeat_count to become available...");
  }
  return true;
}

  void send_request()
  {
    auto request = std::make_shared<std_srvs::srv::Trigger::Request>();

    RCLCPP_INFO(this->get_logger(), "Sending request to reset heartbeat count...");

    // 异步发送请求并注册回调 Lambda
    client_->async_send_request(
      request,
      [this](rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture future) {
        auto response = future.get();
        RCLCPP_INFO(
          this->get_logger(),
          "Response received -> Success: %s | Message: '%s'",
          response->success ? "true" : "false",
          response->message.c_str()
        );

        // 回调处理完毕，触发全局关闭退出事件循环
        rclcpp::shutdown();
      }
    );
  }

private:
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr client_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<StatusClientNode>();

  // 1. 发现阶段：阻塞等待服务端上线（轮询 DDS 图事件，不依赖 spin）
  if (!node->wait_for_server()) {
    rclcpp::shutdown();
    return 1;
  }

  // 2. 发起请求并挂载回调
  node->send_request();

  // 3. 阻塞进入事件循环，等待回调接收响应并在其内部触发 shutdown()
  rclcpp::spin(node);

  return 0;
}