#include <memory>
#include <thread>
#include <cmath>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "arm_interfaces/action/arm_move.hpp"

class ArmMoveServer : public rclcpp::Node
{
public:
  using ArmMove = arm_interfaces::action::ArmMove;
  using GoalHandleArmMove = rclcpp_action::ServerGoalHandle<ArmMove>;

  explicit ArmMoveServer(const rclcpp::NodeOptions & options = rclcpp::NodeOptions())
  : Node("arm_move_server", options)
  {
    action_server_ = rclcpp_action::create_server<ArmMove>(
      this,
      "arm_move",
      std::bind(&ArmMoveServer::handle_goal, this, std::placeholders::_1, std::placeholders::_2),
      std::bind(&ArmMoveServer::handle_cancel, this, std::placeholders::_1),
      std::bind(&ArmMoveServer::handle_accepted, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "ArmMove action server ready.");
  }

private:
  rclcpp_action::Server<ArmMove>::SharedPtr action_server_;

  // 1. 目标校验
  rclcpp_action::GoalResponse handle_goal(
    const rclcpp_action::GoalUUID & uuid,
    std::shared_ptr<const ArmMove::Goal> goal)
  {
    (void)uuid;
    RCLCPP_INFO(this->get_logger(), "Received goal: target_angle=%.2f", goal->target_angle);

    if (std::isnan(goal->target_angle) || std::abs(goal->target_angle) > 180.0) {
      RCLCPP_WARN(this->get_logger(), "Rejected: target_angle out of range.");
      return rclcpp_action::GoalResponse::REJECT;
    }
    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  // 2. 取消请求
  rclcpp_action::CancelResponse handle_cancel(
    const std::shared_ptr<GoalHandleArmMove> goal_handle)
  {
    (void)goal_handle;
    RCLCPP_INFO(this->get_logger(), "Cancel request received.");
    return rclcpp_action::CancelResponse::ACCEPT;
  }

  // 3. 目标已接受 -> 甩给独立线程执行
  void handle_accepted(const std::shared_ptr<GoalHandleArmMove> goal_handle)
  {
    std::thread{std::bind(&ArmMoveServer::execute, this, std::placeholders::_1), goal_handle}.detach();
  }

  // 真正的执行逻辑
  void execute(const std::shared_ptr<GoalHandleArmMove> goal_handle)
  {
    const auto goal = goal_handle->get_goal();
    auto feedback = std::make_shared<ArmMove::Feedback>();
    auto result = std::make_shared<ArmMove::Result>();

    double current = 0.0;
    const double target = goal->target_angle;
    const double step = (target >= current) ? 1.0 : -1.0;
    rclcpp::Rate loop_rate(5);  // 5 Hz，模拟机械臂匀速转动

    while (rclcpp::ok() && std::abs(target - current) > 0.5) {
      if (goal_handle->is_canceling()) {
        result->success = false;
        result->final_angle = current;
        result->message = "Canceled by client.";
        goal_handle->canceled(result);
        RCLCPP_INFO(this->get_logger(), "Goal canceled at angle %.2f", current);
        return;
      }

      current += step;
      feedback->current_angle = current;
      goal_handle->publish_feedback(feedback);
      RCLCPP_INFO(this->get_logger(), "Feedback: current_angle=%.2f", current);

      loop_rate.sleep();
    }

    if (rclcpp::ok()) {
      result->success = true;
      result->final_angle = current;
      result->message = "Goal reached successfully.";
      goal_handle->succeed(result);
      RCLCPP_INFO(this->get_logger(), "Goal succeeded at angle %.2f", current);
    }
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ArmMoveServer>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}