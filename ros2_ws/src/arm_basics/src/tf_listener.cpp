#include <chrono>
#include <memory>

#include "geometry_msgs/msg/transform_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2/exceptions.h"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"

using namespace std::chrono_literals;

class TfListener : public rclcpp::Node
{
public:
  TfListener()
  : Node("tf_listener")
  {
    // 1. 初始化 Buffer，接入当前节点的时钟
    tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());

    // 2. 初始化 TransformListener，绑定 Buffer，开始在后台订阅 /tf 和 /tf_static
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

    // 3. 创建 10Hz 定时器周期查询两帧间的变换关系
    timer_ = this->create_wall_timer(
      100ms, std::bind(&TfListener::on_timer, this));

    RCLCPP_INFO(this->get_logger(), "TF listener started, querying world -> tool0...");
  }

private:
  void on_timer()
  {
    geometry_msgs::msg::TransformStamped t;

    // 4. 必须使用 try-catch 包裹 lookupTransform 防护异常
    try {
      // 查询 target_frame: world, source_frame: tool0
      // tf2::TimePointZero 表示获取 Buffer 中最新可用的一帧
      t = tf_buffer_->lookupTransform("world", "tool0", tf2::TimePointZero);
    } catch (const tf2::TransformException & ex) {
      RCLCPP_WARN(
        this->get_logger(), "Could not transform world to tool0: %s", ex.what());
      return;
    }

    // 5. 拿到变换后打印平移与四元数
    RCLCPP_INFO(
      this->get_logger(),
      "Translation: x=%.2f, y=%.2f, z=%.2f | Rotation: [x=%.2f, y=%.2f, z=%.2f, w=%.2f]",
      t.transform.translation.x,
      t.transform.translation.y,
      t.transform.translation.z,
      t.transform.rotation.x,
      t.transform.rotation.y,
      t.transform.rotation.z,
      t.transform.rotation.w);
  }

  std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TfListener>());
  rclcpp::shutdown();
  return 0;
}