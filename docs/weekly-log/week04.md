# Week 4：基础通信 + 启动与参数 · 学习日志

> 日期：YYYY-MM-DD ~ YYYY-MM-DD

## 本周目标

-

## 学到了什么

-## Day 1
- 创建 Publisher 的三项必要信息
消息类型（Message Type）：C++ 模板参数，决定序列化格式（本日用 std_msgs::msg::String）
Topic 名称：发布者与订阅者在 DDS 网络中互相发现、对齐的唯一凭证
QoS / 队列深度：可以是整数（语法糖，自动转换为"深度=N + 默认策略"的 QoS 对象），也可以是显式 rclcpp::QoS 对象（如 SystemDefaultsQoS()）；本日先用整数写法，QoS 细节留到 Day 2
- 为什么发布逻辑必须放在定时器回调里，而不是构造函数里
生命周期错位：构造函数执行时代码还未进入 spin()，事件调度器尚未接管节点
DDS discovery 时延：create_publisher 后，DDS 实体的发现报文（PDP/EDP）才刚开始交换，发布者与订阅者的握手、QoS 匹配需要几十到上百毫秒；此时发布的消息会直接丢失
实测验证：ros2 topic hz 刚启动时打印 WARNING: topic [...] does not appear to be published yet，正是这个 discovery 窗口期的直接证据，几十毫秒后 warning 自动消失
通过 create_wall_timer + spin()，等 discovery 完成后才持续周期性发布，避开了这个窗口期
（连接 Day 2 预告）该问题的另一种解法是 QoS 层面的 transient_local + keep_all：发布端为晚连接的订阅端缓存并补发历史消息
- executor.add_node() 传参：node vs node->get_node_base_interface()
两种写法都能编译通过，但语义不同
executor 内部只关心调度相关的最小契约，即 NodeBaseInterface（这是接口隔离原则的体现）
直接传 shared_ptr<Node> 是语法便利（内部依然会取 get_node_base_interface()）
显式传 get_node_base_interface() 更贴近底层设计意图：即便未来出现不继承 rclcpp::Node、但实现了 NodeBaseInterface 的类（如生命周期节点），也能被同一个 executor 无缝纳管
- const 成员函数与 const correctness
订阅回调写成 void topic_callback(...) const，将隐式 this 指针变为 const StatusSubNode*
函数体内所有非 mutable 成员被视为只读，若尝试修改成员变量（如 message_count_++）会编译报错
当前回调只读取 msg->data 并打日志，不修改节点状态，const 标注是对这份契约的显式声明，比教材原文更严谨

## 卡在哪 / 怎么解决的

| 问题 | 花了多久 | 怎么解决的 |
|---|---|---|
|  |  |  |

## 检查点是否通过

- [ ]

## 代码/产出链接

-

## 下周计划微调

-
