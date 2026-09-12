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

- ## day 2
1. 取消路径的实际验证（Day1 遗留项）
ros2 action send_goal ... --feedback 期间按 Ctrl+C，CLI 本身会捕获 SIGINT 并主动向 server 发一个 cancel 请求（不是简单粗暴地砍掉进程）
server 端 log 精确对应：先打印 Cancel request received.（handle_cancel 回调），紧跟 Goal canceled at angle X.00（execute() 检测到状态变化后主动退出）
关键发现：is_canceling() 判断被写在 while 循环最顶部，早于 current += step 和 publish_feedback——所以取消后角度精确停在最后一次已发布 feedback 的值，不会像"先自增再判断"那种写法一样多跳一格。这是比标准示例更严谨的顺序选择
2. C++ action 客户端（rclcpp_action::Client）
三个回调与服务端生命周期的精确映射：
goal_response_callback：handle_goal 返回后触发；若被 REJECT，回调收到的 goal_handle 是 nullptr，result_callback 永远不会触发——必须在这里判空并提前 shutdown()，否则客户端会挂死等一个不会来的 result
feedback_callback：对应 publish_feedback，会被反复调用直到目标终止
result_callback：对应 execute() 的三种终态之一——SUCCEEDED/CANCELED/ABORTED（rclcpp_action::ResultCode），通过 get_result service 传回
客户端作为"一次性任务型节点"，在 result_callback 里主动调用 rclcpp::shutdown()，而不是像 server 那样常驻

- ## day 3
- tf2 整体架构
所有坐标系（frame）构成一棵树：每个 frame 只有一个 parent，可以有多个 child
分布式广播/监听：广播者和监听者不直接通信，全部通过话题解耦——/tf（动态）和 /tf_static（静态）
底层消息：geometry_msgs/Transform（translation + 四元数 rotation），套上 Header 和 child_frame_id 组成 TransformStamped
- 5.3.1 静态变换广播
类：tf2_ros::StaticTransformBroadcaster，topic：/tf_static
QoS：StaticBroadcasterQoS，depth=1 + transient_local()
关键机制：类内部维护 net_message_（一个 TFMessage，即 TransformStamped[]）累加容器。每次调用 sendTransform()，都会把该节点生命周期内所有已广播过的静态变换合并成一条完整快照重新发布——所以 depth=1 依然能保证晚加入的订阅者拿到全部历史静态变换，而不会只拿到最后一条
实践产出：static_tf_broadcaster.cpp（world → base_link，z=0.05，无旋转）
- 5.3.2 动态变换广播
类：tf2_ros::TransformBroadcaster，topic：/tf
QoS：DynamicBroadcasterQoS，depth=100（默认）+ Volatile，不支持 transient_local
原因：高频动态数据若也持久化缓存，DDS 握手瞬间会推送大量过期历史数据，网络和内存开销失控；历史轨迹的缓存与插值职责下放给监听端的 Buffer 自行处理
实践产出：dynamic_tf_broadcaster.cpp（base_link → tool0，20Hz 定时器广播，z 方向按 0.2 + 0.1*sin(t) 做往复运动）
验证对比：view_frames 里静态变换 rate=10000（占位值）/buffer_length=0，动态变换 rate≈20.2/buffer_length≈5s
关键发现：tf2_echo world tool0 能自动算出组合变换，即使没有任何节点直接广播过这两个 frame 间的关系——tf2 沿着树结构自动查找公共路径、级联多段变换矩阵相乘
- 5.3.3 监听坐标变换
两个类分工明确：tf2_ros::Buffer 负责缓存并对外提供查询接口；tf2_ros::TransformListener 只负责订阅 /tf 和 /tf_static、把数据灌入 Buffer，自己不提供查询
Buffer 构造需传入节点 Clock，用于时间戳插值和过期判断
TransformListener 的简化构造 (*tf_buffer_) 内部会自建独立节点和线程，不需要传 this
lookupTransform(target_frame, source_frame, time) 查不到时抛 tf2::TransformException 及其子类（LookupException/ConnectivityException/ExtrapolationException），必须 try-catch 防护
易错点：参数顺序与 sendTransform 相反——广播是"父在前、子在后"（header.frame_id 是父，child_frame_id 是子）；查询是"目标在前、来源在后"（target_frame, source_frame），语义是"把 source 坐标系里的点转换到 target 坐标系"
实践产出：tf_listener.cpp，10Hz 定时器查询 world → tool0，成功打印平移+四元数，失败走 RCLCPP_WARN

## 卡在哪 / 怎么解决的

ros2 pkg create --dependencies action_msgs arm_interfaces(漏 --)→ --dependencies 贪婪吃掉两个词,package_name 缺失报错
修复后又因为报错命令和后续 mkdir 是两条独立命令(无 && 连接),mkdir 照常执行建出了空壳目录,导致重新 pkg create 时 Aborted! directory already exists——需要先 rm -rf 空壳目录再重建
rosidl_generate_interfaces() 报错找不到 action/ArmMove.action——mkdir -p 只建目录不写内容,.action 文件内容当时还没写入,用 cat > ... << 'EOF' 补上后编译通过
colcon build 输出大量 Clock skew detected 警告——WSL2 虚拟机时钟与 Windows 宿主机漂移导致,不影响构建产物(Finished 且退出码正常),可选 sudo hwclock -s 同步

package.xml 里 <depend>rclcpp_action</depend> 和 <depend>arm_interfaces</depend> 各自被复制粘贴重复了一遍——catkin_pkg 解析阶段直接报错，连 CMake 都没跑起来
CMakeLists.txt 漏加 arm_move_client 的 add_executable/ament_target_dependencies/install(TARGETS...) 三件套——编译"成功"但产物里没有这个可执行文件，ros2 run 报 No executable found
colcon 缓存复用造成假构建成功：补完 CMakeLists.txt 后第一次重新 colcon build 只用了 1.43 秒就"Finished"，实际没有真正编译新目标（正常完整构建要 14+ 秒）——rm -rf build/arm_basics install/arm_basics 强制干净重建后才真正生效
多 workspace 环境污染（今天最大的坑）：ros2 pkg prefix arm_basics 揭示 ROS2 实际解析到的是仓库根目录下 install/arm_basics，不是 ros2_ws/install/arm_basics——根源是早期（Week3 路径混乱期间）在仓库根目录误跑过一次 colcon build，留下从未清理的残留 build//install/；某次手动在根目录 source install/setup.bash 把这条旧路径混进了 AMENT_PREFIX_PATH，且排在 ros2_ws 前面，导致 ros2 run 一直命中那份没有 arm_move_client 的旧包。用 ros2 pkg prefix <pkg> 精确定位，删除残留目录 + 开全新终端（让 .bashrc 重新初始化，光删目录不够，已污染的环境变量还留在当前 shell 内存里）才彻底解决
副作用：ros2_ws 自己的 setup.bash 之前在被污染的环境下生成过，链式焊死引用了那个已删除的旧路径，之后 source 会报 not found 警告（不影响功能）——需要 rm -rf build install log && colcon build 在干净环境下重新生成一遍才能根治

CMake 声明顺序错误：ament_target_dependencies(static_tf_broadcaster ...) 写在了对应的 add_executable(static_tf_broadcaster ...) 之前。CMake 是自上而下顺序执行的脚本，被引用的 target 必须由同一文件里更早出现的 add_executable 先声明。用 grep -n "static_tf_broadcaster" CMakeLists.txt 快速定位行号确认。
colcon build 目录错误：一度在仓库根目录（~/projects/ros2-industrial-arm-learning）而非工作区根目录（.../ros2_ws）执行了 colcon build，凭空生成了一个"野生的" install/arm_basics，导致找不到 arm_interfaces 包（它只存在于 ros2_ws/install/ 下）。教训：换终端窗口后随手 pwd 确认路径。
DDS 握手延迟的真实体现：多次观察到"晚加入的订阅者第一次查询报错（frame does not exist），紧接着下一次就成功"的现象——这是 transient_local 缓存被推送前，DDS discovery/握手窗口期的真实反映，不是 bug。

## 检查点是否通过

- [x]

## 代码/产出链接

-

## 下周计划微调

-
