#include <rclcpp/rclcpp.hpp>

class ArmBasicsNode : public rclcpp::Node
{
public:
    ArmBasicsNode() : Node("arm_basics_node")
    {
        RCLCPP_INFO(this->get_logger(), "arm_basics_node 已启动");
    }
};

int main(int argc, char * argv[])
{
    // 写法B（显式创建执行器）
rclcpp::init(argc, argv);
rclcpp::executors::SingleThreadedExecutor executor;
auto node = std::make_shared<ArmBasicsNode>();
executor.add_node(node->get_node_base_interface());
executor.spin();
rclcpp::shutdown();
}