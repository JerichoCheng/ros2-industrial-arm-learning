#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "arm_interfaces/action/arm_move.hpp"

using ArmMove = arm_interfaces::action::ArmMove;
using GoalHandleArmMove = rclcpp_action::ClientGoalHandle<ArmMove>;

class ArmMoveClient : public rclcpp::Node
{
public:
  explicit ArmMoveClient(double target_angle)
  : Node("arm_move_client"), target_angle_(target_angle)
  {
    client_ = rclcpp_action::create_client<ArmMove>(this, "arm_move");
  }

  void send_goal()
  {
    if (!client_->wait_for_action_server(std::chrono::seconds(5))) {
      RCLCPP_ERROR(get_logger(), "Action server not available after waiting");
      rclcpp::shutdown();
      return;
    }

    auto goal_msg = ArmMove::Goal();
    goal_msg.target_angle = target_angle_;

    auto options = rclcpp_action::Client<ArmMove>::SendGoalOptions();
    options.goal_response_callback =
      std::bind(&ArmMoveClient::goal_response_callback, this, std::placeholders::_1);
    options.feedback_callback =
      std::bind(&ArmMoveClient::feedback_callback, this, std::placeholders::_1, std::placeholders::_2);
    options.result_callback =
      std::bind(&ArmMoveClient::result_callback, this, std::placeholders::_1);

    RCLCPP_INFO(get_logger(), "Sending goal: target_angle=%.2f", target_angle_);
    client_->async_send_goal(goal_msg, options);
  }

private:
  // TODO 1：判断 goal_handle 是否为 nullptr（对应上面说的 REJECT 情况），
  // 并想清楚：如果被拒绝了，这个节点还有必要继续 spin 下去吗？
  void goal_response_callback(const GoalHandleArmMove::SharedPtr & goal_handle)
  {
    if (!goal_handle) {
      RCLCPP_ERROR(get_logger(), "Goal was rejected by server");
      // 既然被拒绝，不会有后续的 feedback 和 result，一次性节点没有理由继续空转
      rclcpp::shutdown();
      return;
    }
    RCLCPP_INFO(get_logger(), "Goal accepted by server, waiting for result...");
  }

  // TODO 2：打印 feedback->current_angle 即可，注意参数类型是 const shared_ptr<const Feedback>
  void feedback_callback(
    GoalHandleArmMove::SharedPtr,
    const std::shared_ptr<const ArmMove::Feedback> feedback)
  {
    RCLCPP_INFO(get_logger(), "Feedback: current_angle = %.2f", feedback->current_angle);
  }

  // TODO 3：switch (result.code) 处理三种 rclcpp_action::ResultCode
  // （SUCCEEDED / CANCELED / ABORTED），分别打印对应信息，
  // 最后别忘了调用 rclcpp::shutdown() —— 这是一次性任务型节点，不是常驻节点
  void result_callback(const GoalHandleArmMove::WrappedResult & result)
  {
    switch (result.code) {
      case rclcpp_action::ResultCode::SUCCEEDED:
        RCLCPP_INFO(
          get_logger(),
          "Goal succeeded! final_angle = %.2f, msg: %s",
          result.result->final_angle,
          result.result->message.c_str());
        break;

      case rclcpp_action::ResultCode::CANCELED:
        RCLCPP_WARN(
          get_logger(),
          "Goal was canceled! final_angle = %.2f, msg: %s",
          result.result->final_angle,
          result.result->message.c_str());
        break;

      case rclcpp_action::ResultCode::ABORTED:
        RCLCPP_ERROR(
          get_logger(),
          "Goal was aborted! final_angle = %.2f, msg: %s",
          result.result->final_angle,
          result.result->message.c_str());
        break;

      default:
        RCLCPP_ERROR(get_logger(), "Unknown result code");
        break;
    }

    // 动作周期终结，关闭上下文，让 main 中的 spin 退出
    rclcpp::shutdown();
  }

  rclcpp_action::Client<ArmMove>::SharedPtr client_;
  double target_angle_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  double target_angle = (argc > 1) ? std::stod(argv[1]) : 30.0;
  auto node = std::make_shared<ArmMoveClient>(target_angle);
  node->send_goal();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}