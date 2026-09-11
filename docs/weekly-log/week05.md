# Week 5：扩展通信 + 坐标变换 · 学习日志

> 日期：YYYY-MM-DD ~ YYYY-MM-DD

## 本周目标

-

## 学到了什么

- ## day 1
- 为什么需要 action(相对 service 的结构性缺失)
Feedback 缺失:service 只有"未完成/已完成"两态,没有"进行中"这个合法状态,长耗时操作(如机械臂运动)中途客户端完全黑盒盲等
Cancel/抢占缺失:service 请求一旦发出不可撤回,没有中途取消的控制帧;强行用第二条 service 做"停止"会破坏事务内聚性,易引发死锁
本质:action = 3 个 service(send_goal / cancel_goal / get_result)+ 2 个 topic(feedback / status)组合出的复合通信模式,不是全新协议
客户端发起的叫 goal 而不是 request,因为它对应的是整套 5 通道生命周期的起点,不是一次简单调用
- 自定义 action 接口(.action 文件)
语法:两条 --- 分隔线,三段结构 —— Goal(客户端目标)/ Result(最终结论)/ Feedback(周期快照)
设计并实现了项目专属的 ArmMove.action:
  float64 target_angle
  ---
  bool success
  float64 final_angle
  string message
  ---
  float64 current_angle
为什么接口要独立成 xxx_interfaces 包:严格来说 colcon 技术上支持同包混放(能正确排 build target 顺序,不会死锁),但把接口和业务代码分离是社区强约定,核心原因是复用性——避免其他语言/其他包想用该接口时被迫拉入无关业务依赖
ros2 pkg create --dependencies action_msgs -- arm_interfaces 里的 --:因为 --dependencies 是 nargs='+'(贪婪吃词),不加 -- 会把位置参数 package_name 也吃进依赖列表,导致报错缺失必填参数
action_msgs 依赖的两个层面:
ros2 pkg create --dependencies → 写入 package.xml/find_package,解决构建拓扑顺序(外部环境发现)
rosidl_generate_interfaces(... DEPENDENCIES action_msgs) → 告诉代码生成器去哪找 GoalStatus/UUID 等底层类型做语法拼装
两者不是同一件事写两遍,是两个不同构建抽象层各自的声明
<buildtool_depend> vs <exec_depend>:前者是构建期用完即弃的工具(rosidl_default_generators,代码生成脚本),后者是节点运行时真正链接/引用的运行时库(rosidl_default_runtime)
- C++ action 服务端(rclcpp_action)三回调
handle_goal:目标合法性校验(关节角度范围、NaN/Inf 检查),必须在极短时间内返回 ACCEPT_AND_EXECUTE / REJECT,不能塞耗时逻辑——否则会挡住 executor,连 handle_cancel 都收不到,整个节点交互瘫痪
handle_cancel:返回值只表示"同意尝试取消",不保证真的取消成功——物理上机械臂无法瞬停,必须走减速轨迹;这是协作式取消(cooperative cancellation):框架只负责把状态标记为 CANCELING,执行线程若不主动查询 is_canceling() 并调用 canceled(),会对取消意图完全无感知,最终仍会跑到 succeed()
handle_accepted:真正执行逻辑通过 std::thread{...}.detach() 甩到独立线程,让主 executor 线程立刻回去继续 spin(),能持续响应 feedback 发布和后续的 cancel 请求
rclcpp::Rate loop_rate(5) 的作用:防止无延时忙等循环打满 CPU 单核,同时避免 publish_feedback 变成 DDS 消息风暴(挤爆网络栈/客户端回调队列)

## 卡在哪 / 怎么解决的

ros2 pkg create --dependencies action_msgs arm_interfaces(漏 --)→ --dependencies 贪婪吃掉两个词,package_name 缺失报错
修复后又因为报错命令和后续 mkdir 是两条独立命令(无 && 连接),mkdir 照常执行建出了空壳目录,导致重新 pkg create 时 Aborted! directory already exists——需要先 rm -rf 空壳目录再重建
rosidl_generate_interfaces() 报错找不到 action/ArmMove.action——mkdir -p 只建目录不写内容,.action 文件内容当时还没写入,用 cat > ... << 'EOF' 补上后编译通过
colcon build 输出大量 Clock skew detected 警告——WSL2 虚拟机时钟与 Windows 宿主机漂移导致,不影响构建产物(Finished 且退出码正常),可选 sudo hwclock -s 同步

## 检查点是否通过

- [ ]

## 代码/产出链接

-

## 下周计划微调

-
