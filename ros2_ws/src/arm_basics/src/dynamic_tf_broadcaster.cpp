#include <chrono>
#include <cmath>
#include <memory>

#include "geometry_msgs/msg/transform_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_ros/transform_broadcaster.h"

using namespace std::chrono_literals;

class DynamicTfBroadcaster : public rclcpp::Node
{
public:
  DynamicTfBroadcaster()
  : Node("dynamic_tf_broadcaster"), count_(0.0)
  {
    // 初始化动态广播器
    tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);

    // 创建 20Hz 定时器持续更新并广播动态变换 (每 50ms 一次)
    timer_ = this->create_wall_timer(
      50ms, std::bind(&DynamicTfBroadcaster::broadcast_timer_callback, this));

    RCLCPP_INFO(this->get_logger(), "Dynamic TF broadcaster started: base_link -> tool0");
  }

private:
  void broadcast_timer_callback()
  {
    geometry_msgs::msg::TransformStamped t;

    // 动态变换必须盖上精确的当前时间戳，供监听端插值
    t.header.stamp = this->get_clock()->now();
    t.header.frame_id = "base_link";
    t.child_frame_id = "tool0";

    // 模拟平移：x, y 设为固定伸展距离，z 沿正弦曲线上下移动
    t.transform.translation.x = 0.3;
    t.transform.translation.y = 0.0;
    t.transform.translation.z = 0.2 + 0.1 * std::sin(count_);

    // 姿态：设为无旋转（单位四元数）
    tf2::Quaternion q;
    q.setRPY(0.0, 0.0, 0.0);
    t.transform.rotation.x = q.x();
    t.transform.rotation.y = q.y();
    t.transform.rotation.z = q.z();
    t.transform.rotation.w = q.w();

    // 广播到 /tf
    tf_broadcaster_->sendTransform(t);

    // 步进时间变量
    count_ += 0.05;
  }

  rclcpp::TimerBase::SharedPtr timer_;
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
  double count_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<DynamicTfBroadcaster>());
  rclcpp::shutdown();
  return 0;
}